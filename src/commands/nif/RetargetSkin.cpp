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

#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Physics\Dynamics\Constraint\Bilateral\Ragdoll\hkpRagdollConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\BallAndSocket\hkpBallAndSocketConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Hinge\hkpHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\LimitedHinge\hkpLimitedHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Prismatic\hkpPrismaticConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\StiffSpring\hkpStiffSpringConstraintData.h>
#include <Physics\Dynamics\Constraint\Malleable\hkpMalleableConstraintData.h>

#include <Physics\Collide\Util\hkpTriangleUtil.h>
#include <Animation\Animation\Mapper\hkaSkeletonMapperUtils.h>
#include <Animation/Animation/hkaAnimationContainer.h>

#include <limits>
#include <array>
#include <unordered_map>

static bool BeginConversion(string importPath, string importSkeletonPath, string exportPath, string newSkeleton);
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
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_skin> <path_to_skeleton> [-e <path_to_export>] [-s <path_to_new_skeleton>]\r\n";

	//will need to check this help in console/
	const char help[] =
		R"(Retargets a skin instance to a new proxied skeleton. BEWARE: use a skeleton with the same bones, otherwise The behavior is undefined
		
		Arguments:
			<path_to_skin> the skin NIF to retarget
			<path_to_skeleton> path to the new skeleton
			<path_to_export> path to the output directory
			<path_to_new_skeleton> path to the new skeleton

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
	string importSkinPath, importSkeletonPath, exportPath, newSkeletonPath = "";

	importSkinPath = parsedArgs["<path_to_skin>"].asString();
	importSkeletonPath = parsedArgs["<path_to_skeleton>"].asString();
	exportPath = parsedArgs["<path_to_export>"].asString();
	if (parsedArgs["-s"].asBool())
		newSkeletonPath = parsedArgs["<path_to_new_skeleton>"].asString();

	InitializeHavok();
	BeginConversion(importSkinPath, importSkeletonPath, exportPath, newSkeletonPath);
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

	static inline Niflib::Quaternion TOQUAT(const ::hkQuaternion& q, bool inverse = false){
		Niflib::Quaternion qt(q.m_vec.getSimdAt(3), q.m_vec.getSimdAt(0), q.m_vec.getSimdAt(1), q.m_vec.getSimdAt(2));
		return inverse ? qt.Inverse() : qt;
	}

	static inline ::hkQuaternion TOQUAT(const Niflib::Quaternion& q, bool inverse = false){
		hkVector4 v(q.x, q.y, q.z, q.w);
		v.normalize4();
		::hkQuaternion qt(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
		if (inverse) qt.setInverse(qt);
		return qt;
	}

	static inline ::hkQuaternion TOQUAT(const Niflib::hkQuaternion& q, bool inverse = false){
		hkVector4 v(q.x, q.y, q.z, q.w);
		v.normalize4();
		::hkQuaternion qt(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
		if (inverse) qt.setInverse(qt);
		return qt;
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

hkRefPtr<hkaSkeleton> createHkSkeleton(const vector<NiNodeRef>& bones, const map<NiAVObjectRef, NiObjectRef>& parentMap, NiNodeRef& skeleton_root) {
	vector<int> intParentMap(bones.size());
	for (size_t i = 0; i < bones.size(); i++) {
		NiNodeRef bone = bones[i];
		map<NiAVObjectRef, NiObjectRef>::const_iterator parent_it = parentMap.find(StaticCast<NiAVObject>(bone));
		if (parent_it != parentMap.end()) {
			NiNodeRef parent = DynamicCast<NiNode>(parent_it->second);
			auto parentBone = find(bones.begin(), bones.end(), parent);
			if (parentBone != bones.end())
			{
				intParentMap[i] = parentBone - bones.begin();
			}
		}
		else {
			intParentMap[i] = -1;
			skeleton_root = bone;
		}
	}

	Log::Info("Parent map created, root bone: %s\n", skeleton_root->GetName().c_str());
	//
	hkRefPtr<hkaSkeleton> hkSkeleton = new hkaSkeleton();
	hkSkeleton->m_name = (new string(skeleton_root->GetName()))->c_str();

	//Allocate
	hkSkeleton->m_parentIndices.setSize(bones.size());
	hkSkeleton->m_bones.setSize(bones.size());
	hkSkeleton->m_referencePose.setSize(bones.size());
	string bone_name;

	for (size_t i = 0; i < bones.size(); i++) {
		NiNodeRef bone = bones[i];
		//parent map
		hkSkeleton->m_parentIndices[i] = intParentMap[i];

		//bone track
		hkaBone& hkBone = hkSkeleton->m_bones[i];
		bone_name = bone->GetName();
		hkBone.m_name = (new string(bone_name))->c_str();
		if (bone == skeleton_root)
			hkBone.m_lockTranslation = false;
		else
			hkBone.m_lockTranslation = true;

		//reference pose
		hkSkeleton->m_referencePose[i].setTranslation(TOVECTOR4(bone->GetTranslation()));
		hkSkeleton->m_referencePose[i].setRotation(TOQUAT(bone->GetRotation().AsQuaternion()));
		hkSkeleton->m_referencePose[i].setScale(hkVector4(bone->GetScale(), bone->GetScale(), bone->GetScale()));
	}

	return hkSkeleton;
}

string getOblivionToSkyrimMapping(string oblivion_bone_name) 
{
	if (oblivion_bone_name == "Bip01 Head")
		return "NPC Head [Head]";
	if (oblivion_bone_name == "Bip01 L Clavicle")
		return "NPC L Clavicle [LClv]";
	if (oblivion_bone_name == "Bip01 R Clavicle")
		return "NPC R Clavicle [RClv]";
	if (oblivion_bone_name == "Bip01 L UpperArmTwist")
		return "NPC L UpperarmTwist1 [LUt1]";
	if (oblivion_bone_name == "Bip01 R UpperArmTwist")
		return "NPC R UpperarmTwist1 [RUt1]";
	if (oblivion_bone_name == "Bip01 R ForearmTwist")
		return "NPC R ForearmTwist1 [RLt1]";
	if (oblivion_bone_name == "Bip01 L ForearmTwist")
		return "NPC L ForearmTwist1 [LLt1]";
	if (oblivion_bone_name == "Bip01 L UpperArm")
		return "NPC L UpperArm [LUar]";
	if (oblivion_bone_name == "Bip01 R UpperArm")
		return "NPC R UpperArm [RUar]";
	if (oblivion_bone_name == "Bip01 L Forearm")
		return "NPC L Forearm [LLar]";
	if (oblivion_bone_name == "Bip01 R Forearm")
		return "NPC R Forearm [RLar]";
	if (oblivion_bone_name == "Bip01 L Thigh")
		return "NPC L Thigh [LThg]";
	if (oblivion_bone_name == "Bip01 R Thigh") 
		return "NPC R Thigh [RThg]";
	if (oblivion_bone_name == "Bip01 L Hand")
		return "NPC L Hand [LHnd]";
	if (oblivion_bone_name == "Bip01 R Hand")
		return "NPC R Hand [RHnd]";
	if (oblivion_bone_name == "Bip01 Spine")
		return "NPC Spine [Spn0]";
	if (oblivion_bone_name == "Bip01 Spine1")
		return "NPC Spine1 [Spn1]";
	if (oblivion_bone_name == "Bip01 Spine2")
		return "NPC Spine2 [Spn2]";
	if (oblivion_bone_name == "Bip01 Pelvis")
		return "NPC Pelvis [Pelv]";
	if (oblivion_bone_name == "Bip01 Neck1")
		return "NPC Neck [Neck]";
	return "";
}

//int compareNames(const char* name1, const char* name2) {
//	try {
//		bool name1_sk = false;
//		bool name2_sk = false;
//		bool name1_ob = false;
//		bool name2_ob = false;
//		string name1s(name1);
//		std::transform(name1s.begin(), name1s.end(), name1s.begin(), ::tolower);
//		string name2s(name2);
//		std::transform(name2s.begin(), name2s.end(), name2s.begin(), ::tolower);
//		size_t npc_pos1 = name1s.find("npc");
//		size_t quadra_pos1 = name1s.find("[");
//		if (npc_pos1 != string::npos && name1s.size()>4) {
//			name1s = name1s.substr(4, name1s.size() - 4 - (name1s.size() - quadra_pos1));
//			name1_sk = true;
//		}
//		size_t npc_pos2 = name2s.find("npc");
//		size_t quadra_pos2 = name2s.find("[");
//		if (npc_pos2 != string::npos && name2s.size()>4) {
//			name2s = name2s.substr(4, name2s.size() - 4 - (name2s.size() - quadra_pos2));
//			name2_sk = true;
//		}
//		size_t bip_pos1 = name1s.find("bip");
//		if (bip_pos1 != string::npos && name1s.size() > 6) {
//			name1s = name1s.substr(6, name1s.size() - 6);
//			name1_ob = true;
//		}
//		size_t bip_pos2 = name2s.find("bip");
//		if (bip_pos2 != string::npos && name2s.size() > 6) {
//			name2s = name2s.substr(6, name2s.size() - 6);
//			name2_ob = true;
//		}
//		if (name1_sk && name2_ob && name1s == name2s)
//			return 0;
//		if (name1_ob && name2_sk && name1s == name2s)
//			return 0;
//		if (!name1_sk && !name2_ob && !name1_ob && !name2_sk && name1s == name2s)
//			return 0;
//		return 1;
//	}
//	catch (std::out_of_range ex) {
//		return 0;
//	}
//}

bool BeginConversion(string importSkinPath, string importSkeletonPath, string exportPath, string newSkeletonpath) {

	bool UseAnotherSkeleton = false;

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
		Log::Info("Invalid Directory: %s, using current_dir", exportPath.c_str());
		outputDir = fs::current_path();
	}
	fs::path newSkeletonModelPath = fs::path(newSkeletonpath);
	if (!fs::exists(skinModelpath) || !fs::is_regular_file(skinModelpath)) {
		Log::Info("Invalid New skeleton path: %s", importSkinPath.c_str());
	}
	else {
		UseAnotherSkeleton = true;
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
				}
			}
		}
	}

	vector<NiObjectRef> blocks_skeleton = ReadNifList(skeletonModelpath.string().c_str(), &info);
	NiObjectRef root_skeleton = GetFirstRoot(blocks_skeleton);

	//find all bones. TODO: support set
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

	if (UseAnotherSkeleton) {

		vector<NiNodeRef> old_skeleton_nodes = DynamicCast<NiNode>(blocks_skeleton);

		//vector<NiObjectRef> blocks_new_skeleton = ReadNifList(newSkeletonModelPath.string().c_str(), &info);
		//NiObjectRef root_new_skeleton = GetFirstRoot(blocks_skeleton);

		//vector<NiNodeRef> new_skeleton_nodes = DynamicCast<NiNode>(blocks_new_skeleton);

		//map<NiAVObjectRef, NiObjectRef> newSkeletonParentMap = buildParentMap(blocks_new_skeleton);

		NiNodeRef ninode_old_skeleton_root;
		//NiNodeRef ninode_new_skeleton_root;

		//RETARGET! create hk skeletons
		hkRefPtr<hkaSkeleton> old_hkskeleton = createHkSkeleton(old_skeleton_nodes, skeletonParentMap, ninode_old_skeleton_root);
		//hkRefPtr<hkaSkeleton> new_hkskeleton = createHkSkeleton(new_skeleton_nodes, newSkeletonParentMap, ninode_new_skeleton_root);

		hkRefPtr<hkaSkeleton> new_hkskeleton;
		
		{
			hkIstream stream(newSkeletonModelPath.string().c_str());
			hkStreamReader *reader = stream.getStreamReader();
			hkResource* skelResource = hkSerializeLoadResource(reader);
		
			hkRootLevelContainer* container = skelResource->getContents<hkRootLevelContainer>();
			HK_ASSERT2(0x27343437, container != HK_NULL, "Could not load asset");
			hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>(container->findObjectByType(hkaAnimationContainerClass.getName()));
		
			HK_ASSERT2(0x27343435, ac && (ac->m_skeletons.getSize() > 0), "No skeleton loaded");
			new_hkskeleton = ac->m_skeletons[0];
		}

		hkArray<hkQsTransform> modelPose(new_hkskeleton->m_referencePose.getSize());
		hkaSkeletonUtils::transformLocalPoseToModelPose(new_hkskeleton->m_referencePose.getSize(), new_hkskeleton->m_parentIndices.begin(), new_hkskeleton->m_referencePose.begin(), modelPose.begin());

		vector<hkQsTransform> pose;

		for (int i = 0; i < modelPose.getSize(); i++) {
			pose.push_back(modelPose[i]);
		}


		cout << "Created skeletons" << endl;

		hkaSkeletonMapperUtils::Params mapping_parameters;
		mapping_parameters.m_skeletonA = old_hkskeleton;
		mapping_parameters.m_skeletonB = new_hkskeleton;

		hkArray<hkaSkeletonMapperUtils::UserMapping> old_to_new;
		for (const auto& bone : old_skeleton_nodes) {
			string bone_name = bone->GetName();
			string mapping = getOblivionToSkyrimMapping(bone_name);
			if (mapping != "") {
				hkaSkeletonMapperUtils::UserMapping a_mapping;
				a_mapping.m_boneIn = (new string(bone_name))->c_str();
				a_mapping.m_boneOut = (new string(mapping))->c_str();
				old_to_new.pushBack(a_mapping);
			}
		}

		mapping_parameters.m_userMappingsAtoB = old_to_new;
		mapping_parameters.m_autodetectSimple = true;
		mapping_parameters.m_autodetectChains = false;
		mapping_parameters.m_mappingType = hkaSkeletonMapperData::HK_RETARGETING_MAPPING;

		hkaSkeletonMapperData oldToNew;
		hkaSkeletonMapperData newToOld;

		hkaSkeletonMapperUtils::createMapping(mapping_parameters, oldToNew, newToOld);

		cout << "Created mappings" << endl;

	}

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

	//update skin bones
	for (const auto& skin_bone : skin_bones) {
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
			if (skeleton_transforms.find(bones_from_list[b_index]->GetName()) == skeleton_transforms.end())
				throw runtime_error("Cannot find the skeleton transform!");
			hkTransform new_direct_transform = skeleton_transforms[bones_from_list[b_index]->GetName()];
			NiTransform& skin_transform = bone_data[b_index].skinTransform;
			hkTransform temp; temp.setInverse(new_direct_transform);
			skin_transform.translation = TOVECTOR3(temp.getTranslation());
			skin_transform.rotation = TOMATRIX33(temp.getRotation());
		}
		skin_instance->GetData()->SetBoneList(bone_data);
		shape->SetVertices(vertices);
	}

	fs::path out_path = outputDir / skinModelpath.filename();
	fs::create_directories(outputDir);
	WriteNifTree(out_path.string(), root_skin, info);

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