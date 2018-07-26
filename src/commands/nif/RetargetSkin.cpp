#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <core/HKXWrangler.h>
#include <commands/RetargetSkin.h>
#include <commands/Geometry.h>
#include <core/games.h>
#include <core/bsa.h>
#include <core/NifFile.h>
#include <commands/NifScan.h>

#include <Physics\Dynamics\Constraint\Bilateral\Ragdoll\hkpRagdollConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\BallAndSocket\hkpBallAndSocketConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Hinge\hkpHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\LimitedHinge\hkpLimitedHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Prismatic\hkpPrismaticConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\StiffSpring\hkpStiffSpringConstraintData.h>
#include <Physics\Dynamics\Constraint\Malleable\hkpMalleableConstraintData.h>

#include <Physics\Collide\Util\hkpTriangleUtil.h>

#include <limits>
#include <array>
#include <unordered_map>

static bool BeginConversion(string importPath, string importSkeletonPath, string exportPath);
static void InitializeHavok();
static void CloseHavok();

REGISTER_COMMAND_CPP(RetargetSkin)

RetargetSkin::RetargetSkin()
{
}

RetargetSkin::~RetargetSkin()
{
}

string RetargetSkin::GetName() const
{
	return "retargetskin";
}

string RetargetSkin::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd convertnif [-i <path_to_import>] [-e <path_to_export>]
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " [-i <path_to_skin>] [-s <path_to_skeleton>] [-e <path_to_export>]\r\n";

	//will need to check this help in console/
	const char help[] =
		R"(Retargets a skin instance to a new proxied skeleton. BEWARE: use a skeleton with the same bones, otherwise The behavior is undefined
		
		Arguments:
			<path_to_skin> the skin NIF to retarget
			<path_to_skeleton> path to the new skeleton
			<path_to_export> path to the output directory;

		)";

	return usage + help;
}

string RetargetSkin::GetHelpShort() const
{
	//I'm unsure about returning a string.. It doesn't show up on the console..
	//Log::Info("Convert Oblivion version models to Skyrim's format.");
	return "TODO: Short help message for ConvertNif";
}

bool RetargetSkin::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	//We can improve this later, but for now this i'd say this is a good setup.
	string importSkinPath, importSkeletonPath, exportPath;

	importSkinPath = parsedArgs["<path_to_skin>"].asString();
	importSkeletonPath = parsedArgs["<path_to_skeleton>"].asString();
	exportPath = parsedArgs["<path_to_export>"].asString();

	InitializeHavok();
	BeginConversion(importSkinPath, importSkeletonPath, exportPath);
	CloseHavok();
	return true;
}

using namespace ckcmd::info;
using namespace ckcmd::BSA;
using namespace ckcmd::Geometry;
using namespace ckcmd::NIF;
using namespace ckcmd::HKX;
using namespace ckcmd::nifscan;


static inline Niflib::Vector3 TOVECTOR3(const hkVector4& v){
	return Niflib::Vector3(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2));
}

static inline Niflib::Matrix33 TOMATRIX33(const hkMatrix3& v) {
	return Niflib::Matrix33(
		v(0, 0), v(1, 0), v(2, 0),
		v(0, 1), v(1, 1), v(2, 1),
		v(0, 2), v(1, 2), v(2, 2)
	);
}

static inline hkVector4 TOVECTOR3(const Niflib::Vector3& v) {
	return hkVector4(v.x, v.y, v.z);
}

static inline hkVector4 TOVECTOR4(const Niflib::Vector4& v){
	return hkVector4(v.x, v.y, v.z, v.w);
}

static inline hkRotation TOMATRIX3(const Niflib::Matrix33& q){
	hkRotation m3;
	m3(0, 0) = q[0][0]; m3(0, 1) = q[1][0]; m3(0, 2) = q[2][0];
	m3(1, 0) = q[0][1]; m3(1, 1) = q[1][1]; m3(1, 2) = q[2][1];
	m3(2, 0) = q[0][2]; m3(2, 1) = q[1][2]; m3(2, 2) = q[2][2];
	return m3;
}

static inline hkTransform TOHKTRANSFORM(const NiAVObjectRef node) {
	Vector3 node_trans = node->GetTranslation();
	hkVector4 hk_node_trans = TOVECTOR3(node_trans);
	Matrix33 node_rot = node->GetRotation();
	hkRotation hk_node_rot = TOMATRIX3(node_rot);
	return	hkTransform(hk_node_rot, hk_node_trans);
}

static inline hkTransform TOHKTRANSFORM(const NiTransform& trans) {
	Vector3 node_trans = trans.translation;
	hkVector4 hk_node_trans = TOVECTOR3(node_trans);
	Matrix33 node_rot = trans.rotation;
	hkRotation hk_node_rot = TOMATRIX3(node_rot);
	return	hkTransform(hk_node_rot, hk_node_trans);
}

Vector3 not_normalized_centeroid(const vector<Vector3>& in) {
	Vector3 centeroid = Vector3(0.0, 0.0, 0.0);
	for (Vector3 vertex : in) {
		centeroid += vertex;
	}
	centeroid = Vector3(centeroid.x / in.size(), centeroid.y / in.size(), centeroid.z / in.size());
	return centeroid;
}

map<NiAVObjectRef, NiObjectRef> buildParentMap(const vector<NiObjectRef>& blocks) {
	map<NiAVObjectRef, NiObjectRef> parentMap;
	for (NiObjectRef niobj : blocks) {
		if (niobj->IsDerivedType(NiNode::TYPE)) {
			NiNodeRef ninode = DynamicCast<NiNode>(niobj);
			vector<NiAVObjectRef> node_children = ninode->GetChildren();
			for (NiAVObjectRef child : node_children)
				parentMap[child] = niobj;
		}
	}
	return parentMap;
}

vector<NiAVObjectRef> getParentChain(const map<NiAVObjectRef, NiObjectRef>& parentMap, NiAVObjectRef node) {
	vector<NiAVObjectRef> vresult;
	NiAVObjectRef result = node;
	vresult.insert(vresult.begin(), DynamicCast<NiAVObject>(result));
	map<NiAVObjectRef, NiObjectRef>::const_iterator parent_it = parentMap.find(node);
	while (parent_it != parentMap.end() && DynamicCast<NiAVObject>(parent_it->second)!=NULL) {
		vresult.insert(vresult.begin(), DynamicCast<NiAVObject>(parent_it->second));
		parent_it = parentMap.find(DynamicCast<NiAVObject>(parent_it->second));
	}
	return vresult;
}

NiAVObjectRef getNearestCommonAncestor(const vector<vector<NiAVObjectRef>>& chains) {
	NiAVObjectRef result = NULL;
	size_t max_size_index = 0;
	size_t max_size = 0;
	int actual_index = 0;
	for (const auto& chain : chains) {
		if (chain.size() > max_size) {
			max_size = chain.size();
			max_size_index = actual_index;
		}
		actual_index++;
	}
	const vector<NiAVObjectRef>& max_chain = chains[max_size_index];
	for (const auto& bone : max_chain) {
		for (const auto& chain : chains) {
			if (find(chain.begin(), chain.end(), bone) == chain.end()) {
				return result;
			}
		}
		if (bone->GetName().find("Accum") == string::npos)
			result = bone;
	}
	return result;
}

hkTransform accumulateTransformChain(const vector<NiAVObjectRef>& chain/*, const NiAVObjectRef& stopNode*/) {
	hkTransform result = hkTransform::getIdentity();
	//size_t start_index = 0;
	//while (start_index < chain.size() && chain[start_index] != stopNode) start_index++;
	/*for (start_index; start_index < chain.size(); start_index++) {*/
	for (const auto& item : chain) {
		result.setMulEq(TOHKTRANSFORM(item));
	}
	//result.setInverse(result);
	return result;
}

bool BeginConversion(string importSkinPath, string importSkeletonPath, string exportPath) {

	fs::path skinModelpath = fs::path(importSkinPath);
	if (!fs::exists(skinModelpath) || !fs::is_regular_file(skinModelpath)) {
		Log::Info("Invalid file: %s", importSkinPath.c_str());
		return false;
	}
	fs::path skeletonModelpath = fs::path(importSkeletonPath);
	if (!fs::exists(skeletonModelpath) || !fs::is_regular_file(skeletonModelpath)) {
		Log::Info("Invalid file: %s", importSkeletonPath.c_str());
		return false;
	}
	fs::path outputDir = fs::path(exportPath);
	if (!fs::exists(outputDir) || !fs::is_directory(outputDir)) {
		Log::Info("Invalid file: %s", exportPath.c_str());
		return false;
	}

	NifInfo info;

	std::map<NiSkinInstanceRef, NiGeometryDataRef> skin_maps;
	std::set<NiSkinInstanceRef> skinInstances;
	std::set<NiNodeRef> skin_bones;
	std::set<string> skin_bone_names;
	vector<hkTransform> skin_transforms;

	std::vector<Vector3> centeroids;

	//read skin
	vector<NiObjectRef> skin_blocks = ReadNifList(skinModelpath.string().c_str(), &info);
	NiObjectRef root_skin = GetFirstRoot(skin_blocks);

	for (NiObjectRef niobj : skin_blocks) {
		if (niobj->IsSameType(NiTriShape::TYPE)) {
			NiTriShapeRef shape = DynamicCast<NiTriShape>(niobj);
			
			if (NULL != shape->GetSkinInstance() && NULL != shape->GetData()) {
				vector<Vector3> vertices = shape->GetData()->GetVertices();
				NiSkinInstanceRef skin_instance = shape->GetSkinInstance();
				skin_maps[skin_instance] = shape->GetData();
				skinInstances.insert(skin_instance);
				vector<NiNode*> bones_from_list = skin_instance->GetBones();
				vector<BoneData> bone_data = skin_instance->GetData()->GetBoneList();
				for (int b_index = 0; b_index < bones_from_list.size(); b_index++) {
					skin_bones.insert(bones_from_list[b_index]);
					skin_bone_names.insert(bones_from_list[b_index]->GetName());
					/*NiTransform& skin_transform = bone_data[b_index].skinTransform;
					hkVector4 hk_skin_trans = TOVECTOR3(skin_transform.translation);
					hkRotation hk_skin_rot = TOMATRIX3(skin_transform.rotation);
					hkTransform hk_skin_transform(hk_skin_rot, hk_skin_trans);
					skin_transforms.push_back(hk_skin_transform);
					NiNode* bone_node = bones_from_list[b_index];
					Vector3 node_trans = bone_node->GetTranslation();
					hkVector4 hk_node_trans = TOVECTOR3(node_trans);
					Matrix33 node_rot = bone_node->GetRotation();
					hkRotation hk_node_rot = TOMATRIX3(node_rot);
					hkTransform hk_node_transform(hk_node_rot, hk_node_trans);
					std::vector<Vector3> thisBoneVertices;
					for (const auto& weightData : bone_data[b_index].vertexWeights) {
						thisBoneVertices.push_back(vertices[weightData.index]);
					}
					Vector3 centeroid = not_normalized_centeroid(thisBoneVertices);
					hkTransform resm1; resm1.setMul(hk_node_transform, hk_skin_transform);
					hkTransform resm2; resm2.setMul(hk_node_transform, hk_skin_transform);
					hkTransform res1; res1.setMulMulInverse(hk_node_transform, hk_skin_transform);
					hkTransform res2; res2.setMulInverseMul(hk_node_transform, hk_skin_transform);
					hkTransform res3; res3.setMulMulInverse(hk_skin_transform, hk_node_transform);
					hkTransform res4; res4.setMulInverseMul(hk_skin_transform, hk_node_transform);*/
				}

				//for (const auto& boneData : skin_instance->GetData()->GetBoneList()) {
				//	std::vector<Vector3> thisBoneVertices;
				//	for (const auto& weightData : boneData.vertexWeights) {
				//		thisBoneVertices.push_back(vertices[weightData.index]);
				//	}
				//	centeroids.push_back(ckcmd::Geometry::centeroid(thisBoneVertices));
				//}
			}
		}
	}

	vector<NiObjectRef> blocks_skeleton = ReadNifList(skeletonModelpath.string().c_str(), &info);
	NiObjectRef root_skeleton = GetFirstRoot(blocks_skeleton);

	//temp
	//map<string, hkVector4> ob_positions;
	//map<string, hkVector4> sk_positions;

	//map<NiAVObjectRef, NiObjectRef> obParentMap = buildParentMap(blocks_skeleton);
	//map<NiAVObjectRef, NiObjectRef> skParentMap = buildParentMap(skin_blocks);

	//std::set<NiNodeRef> ob_skeleton_bones;
	//std::set<NiNodeRef> sk_skeleton_bones;

	//for (const auto& obj : blocks_skeleton) {
	//	if (obj->IsSameType(NiNode::TYPE)) {
	//		NiNodeRef skeleton_node = DynamicCast<NiNode>(obj);
	//		ob_skeleton_bones.insert(skeleton_node);
	//	}
	//}

	//for (const auto& obj : skin_blocks) {
	//	if (obj->IsSameType(NiNode::TYPE)) {
	//		NiNodeRef skeleton_node = DynamicCast<NiNode>(obj);
	//		sk_skeleton_bones.insert(skeleton_node);

	//		/*
	//		obj.SetName(IndexString("NPC L Thigh [LThg]"));
	//		obj.SetTranslation(Vector3(-6.615073f, 0.000394f, 68.911301f));
	//		obj.SetRotation(Matrix33(
	//			-0.9943f, -0.0379f, 0.0999f,
	//			-0.0414f, 0.9986f, -0.0329f,
	//			-0.0985f, -0.0369f, -0.9944f));
	//		
	//		*/
	//	}
	//}

	//vector<vector<NiAVObjectRef>> ob_skeleton_chains;
	//for (const auto& bone : ob_skeleton_bones) {
	//	ob_skeleton_chains.push_back(getParentChain(obParentMap, DynamicCast<NiAVObject>(bone)));
	//}

	//vector<vector<NiAVObjectRef>> sk_skeleton_chains;
	//for (const auto& bone : sk_skeleton_bones) {
	//	sk_skeleton_chains.push_back(getParentChain(skParentMap, DynamicCast<NiAVObject>(bone)));
	//}

	//for (const auto& chain : ob_skeleton_chains) {
	//	const NiAVObjectRef& this_bone = *chain.rbegin();
	//	hkTransform node_transform = accumulateTransformChain(chain/*, DynamicCast<NiAVObject>(root_skeleton)*/);
	//	ob_positions[this_bone->GetName()] = node_transform.getTranslation();
	//}

	//for (const auto& chain : sk_skeleton_chains) {
	//	const NiAVObjectRef& this_bone = *chain.rbegin();
	//	hkTransform node_transform = accumulateTransformChain(chain/*, DynamicCast<NiAVObject>(root_skeleton)*/);
	//	sk_positions[this_bone->GetName()] = node_transform.getTranslation();
	//	cout << "obj.SetName(IndexString(\"" << this_bone->GetName() << "\"));" << endl;
	//	cout << "obj.SetTranslation(Vector3(" << to_string(node_transform.getTranslation()(0)) << ", " << to_string(node_transform.getTranslation()(1)) << ", " << to_string(node_transform.getTranslation()(2)) << "));" << endl;
	//	cout << "obj.SetRotation(Matrix33(" << endl;
	//	Matrix33 rot = TOMATRIX33(node_transform.getRotation());
	//	cout << "\t" << to_string(rot[0][0]) << ", " << to_string(rot[0][1]) << ", " << to_string(rot[0][2]) << "," << endl;
	//	cout << "\t" << to_string(rot[1][0]) << ", " << to_string(rot[1][1]) << ", " << to_string(rot[1][2]) << "," << endl;
	//	cout << "\t" << to_string(rot[2][0]) << ", " << to_string(rot[2][1]) << ", " << to_string(rot[2][2]) << "));" << endl;
	//}

	//map<string, string> ob_to_sk_map;
	//map<string, hkReal> ob_to_sk_map_dist;
	////find nearest map
	//for (const auto & ob_tuple : ob_positions) {
	//	hkReal lastDistance = 10000;
	//	string lastMatch = "";
	//	for (const auto & sk_tuple : sk_positions) {
	//		hkReal distance = ob_tuple.second.distanceToSquared3(sk_tuple.second);
	//		if (distance < lastDistance) {
	//			lastDistance = distance;
	//			lastMatch = sk_tuple.first;
	//		}
	//	}
	//	ob_to_sk_map[ob_tuple.first] = lastMatch;
	//	ob_to_sk_map_dist[ob_tuple.first] = lastDistance;
	//}


	//find all bones
	std::set<NiObjectRef> skeleton_bones;
	for (const auto& obj : blocks_skeleton) {
		if (obj->IsDerivedType(NiNode::TYPE)) {
			NiNodeRef skeleton_node = DynamicCast<NiNode>(obj);
			if (skin_bone_names.find(skeleton_node->GetName()) != skin_bone_names.end())
				skeleton_bones.insert(obj);
		}
	}

	if (skeleton_bones.size() != skin_bones.size())
		throw std::runtime_error("Error: cannot find skin bone into the given skeleton!");

	map<NiAVObjectRef, NiObjectRef> skeletonParentMap = buildParentMap(blocks_skeleton);
	vector<vector<NiAVObjectRef>> skeleton_chains;
	for (const auto& bone : skeleton_bones) {
		skeleton_chains.push_back(getParentChain(skeletonParentMap, DynamicCast<NiAVObject>(bone)));
	}
	//NiAVObjectRef skeleton_bone_root = getNearestCommonAncestor(skeleton_chains);
	map<string, hkTransform> skeleton_transforms;
	//map<string, hkTransform> skeleton_transforms_inv;
	for (const auto& chain : skeleton_chains) {
		const NiAVObjectRef& this_bone = *chain.rbegin();
		hkTransform node_transform = accumulateTransformChain(chain/*, DynamicCast<NiAVObject>(root_skeleton)*/);
		skeleton_transforms[this_bone->GetName()] = node_transform;
		//node_transform.setInverse(node_transform);
		//skeleton_transforms_inv[this_bone->GetName()] = node_transform;
	}
	map<string, hkTransform> old_skeleton_transforms;
	//update skin bones
	for (const auto& skin_bone : skin_bones) {
		old_skeleton_transforms[skin_bone->GetName()] = TOHKTRANSFORM(StaticCast<NiAVObject>(skin_bone));
		hkTransform new_transform = skeleton_transforms[skin_bone->GetName()];
		skin_bone->SetTranslation(TOVECTOR3(new_transform.getTranslation()));
		skin_bone->SetRotation(TOMATRIX33(new_transform.getRotation()));
	}

	//update skin transforms
	for (const auto& skin_instance : skinInstances) {
		vector<NiNode*> bones_from_list = skin_instance->GetBones();
		vector<BoneData> bone_data = skin_instance->GetData()->GetBoneList();
		NiGeometryDataRef shape = skin_maps[skin_instance];
		vector<Vector3> vertices = shape->GetVertices();

		for (int b_index = 0; b_index < bones_from_list.size(); b_index++) {
			//hkTransform new_transform = skeleton_transforms_inv[bones_from_list[b_index]->GetName()];
			if (skeleton_transforms.find(bones_from_list[b_index]->GetName()) == skeleton_transforms.end())
				throw runtime_error("Cannot find the skeleton transform!");
			hkTransform new_direct_transform = skeleton_transforms[bones_from_list[b_index]->GetName()];
			//hkTransform old_transform_node = old_skeleton_transforms[bones_from_list[b_index]->GetName()];
			NiTransform& skin_transform = bone_data[b_index].skinTransform;
			//hkTransform old_transform(TOHKTRANSFORM(skin_transform));
			//hkMatrix4 old_transform_matrix; old_transform_matrix.set(old_transform);
			//hkMatrix4 new_direct_matrix; new_direct_matrix.set(new_direct_transform);
			hkTransform temp; temp.setInverse(new_direct_transform);
			//temp.setMulEq(new_transform);
			//for (const auto& vertWeight : bone_data[b_index].vertexWeights) {
			//	//reskin the mesh to the new pose
			//	hkVector4 vert = TOVECTOR4(vertices[vertWeight.index]);
			//	hkVector4 root_vert; old_transform_matrix.transformPosition(vert, root_vert);
			//	hkVector4 new_vert; new_direct_matrix.transformPosition(root_vert, new_vert);
			//	vertices[vertWeight.index] = TOVECTOR3(new_vert);
			//}

			skin_transform.translation = TOVECTOR3(temp.getTranslation());
			skin_transform.rotation = TOMATRIX33(temp.getRotation());
		}
		skin_instance->GetData()->SetBoneList(bone_data);
		shape->SetVertices(vertices);
	}

	fs::path out_path = outputDir / skinModelpath.filename();
	fs::create_directories(outputDir);
	WriteNifTree(out_path.string(), root_skin, info);


	//for (const auto& strans : skin_transforms) {
	//	bool found = false;
	//	for (const auto& sk_bone : skin_bones) {
	//		hkTransform temp = TOHKTRANSFORM(StaticCast<NiAVObject>(sk_bone));
	//		temp.setMul(strans, temp);
	//		if (temp.isApproximatelyEqual(hkTransform::getIdentity())) 
	//		{
	//			found = true;
	//			break;
	//		}
	//	}
	//	if (!found)
	//		throw std::runtime_error("Error!");
	//}

	//for (const auto& sk_bone : skin_bones) {		
	//	bool found = false;
	//	for (const auto& sktrans : skeleton_transforms) {
	//		hkTransform temp = TOHKTRANSFORM(StaticCast<NiAVObject>(sk_bone));
	//		hkTransform temp2; temp2.setMulInverseMul(temp, sktrans);
	//		if (temp2.isApproximatelyEqual(hkTransform::getIdentity(),0.5)) {
	//			found = true;
	//			break;
	//		}
	//	}
	//	if (!found)
	//		throw std::runtime_error("Error: cannot find an equivalent skin transform!");
	//}

	//for (const auto& strans : skin_transforms) {
	//	bool found = false;
	//	for (const auto& sktrans : skeleton_transforms) {
	//		hkTransform temp; temp.setMul(strans, sktrans);
	//		if (temp.isApproximatelyEqual(hkTransform::getIdentity(), 0.5))
	//		{	
	//			found = true;
	//			break;
	//		}
	//	}
	//	if (!found)
	//		throw std::runtime_error("Error: cannot find an equivalent skin transform!");
	//}

	//NifInfo info;
	//vector<NiObjectRef> blocks = ReadNifList(skeletonModelpath.string().c_str(), &info);
	//NiObjectRef root = GetFirstRoot(blocks);

	//for (NiObjectRef niobj : blocks) {
	//	if (niobj->IsSameType(NiNode::TYPE)) {
	//		NiNodeRef ninodeobj = DynamicCast<NiNode>(niobj);
	//		//if (ninodeobj->GetName().find("NPC") != string::npos) {
	//		vector<Ref<NiAVObject>> children = ninodeobj->GetChildren();
	//		vector<Ref<NiAVObject>> new_children;
	//		for (NiAVObjectRef rr : children) {
	//			NiNode* proxyRoot = new NiNode();

	//			proxyRoot->SetFlags(ninodeobj->GetFlags());
	//			proxyRoot->SetChildren({ rr });
	//			proxyRoot->SetName(IndexString("ProxyNode_" + rr->GetName()));
	//			new_children.push_back(proxyRoot);
	//		}
	//		if (!new_children.empty())
	//			ninodeobj->SetChildren(new_children);
	//		//}
	//	}
	//}

	//fs::path out_path = outputDir / "skeleton.nif";
	//fs::create_directories(outputDir);
	//WriteNifTree(out_path.string(), root, info);
	Log::Info("Done");
	return true;
}

static void HelpString(hkxcmd::HelpType type) {
	switch (type)
	{
	case hkxcmd::htShort: Log::Info("About - Help about this program."); break;
	case hkxcmd::htLong: {
		char fullName[MAX_PATH], exeName[MAX_PATH];
		GetModuleFileName(NULL, fullName, MAX_PATH);
		_splitpath(fullName, NULL, NULL, exeName, NULL);
		Log::Info("Usage: %s about", exeName);
		Log::Info("  Prints additional information about this program.");
	}
						 break;
	}
}

//Havok initialization

static void HK_CALL errorReport(const char* msg, void*)
{
	Log::Error("%s", msg);
}

static void HK_CALL debugReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}


static hkThreadMemory* threadMemory = NULL;
static char* stackBuffer = NULL;
static void InitializeHavok()
{
	// Initialize the base system including our memory system
	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
}

static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}