#include <commands/geometry.h>
//#include <core/hkxcmd.h>
//#include <core/hkfutils.h>
//#include <core/log.h>

using namespace ckcmd::Geometry;

//BSXFlags
/*
<niobject name="BSXFlags" abstract="0" inherit="NiIntegerExtraData">
Controls animation and collision.  Integer holds flags:
Bit 0 : has Gamebryo Blending. Not applicable to NIFs that are meant to be attached to others
Bit 1 : has Havok (at least a collision or a phantom collision)
Bit 2 : has Havok Ragdoll (really means it's a skeleton model, even if there's no ragdoll constraint. has at least a BlendCollision object)
Bit 3 : has Multiple Havok Collisions (multiple bhkCollsionObjects)
Bit 4 : has AttachLight/FlameNode (will be replaced at run time with a light) / AddonNode in Skyrim
Bit 5 : has EditorMarkers
Bit 6 : is Dynamic Havok Rigid Bodies(quality != {MO_QUAL_INVALID, MO_QUAL_FIXED}, doesn't make sense without bit 1)
Bit 7 : 1 if a model has a single collision, or a single kinematic chain (or any number of phantom collisions, which is strange, the only model with 2 phantom collision may be an outlier) . This is influenced by the niswitchnode, which will result in a single branch to be displayed at a time so even if more branches have multiple collision, if a collision or kinematic chain is to be displayed at a a single time in a branch, the bit will be set
Bit 8 : bIKTarget(Skyrim)/needsTransformUpdates (never set in vanilla skyrim + DLCs)
Bit 9 : bExternalEmit(Skyrim, has Own Emit into BSLightingShaderPropery or External Emittance into BSEffectShaderProperty)
Bit 10: bMagicShaderParticles(Skyrim) (never set in vanilla skyrim + DLCs)
Bit 11: bLights(Skyrim) (never set in vanilla skyrim + DLCs)
Bit 12: bBreakable(Skyrim) (never set in vanilla skyrim + DLCs)
Bit 13: bSearchedBreakable(Skyrim) .. Runtime only? (never set in vanilla skyrim + DLCs)
</niobject>

*/

//Bit 7 needs hierarchy visitor
inline bool isSingleChain(const NiObjectRef& root, const NifInfo& info) {
	return SingleChunkFlagVerifier(*root, info).singleChunkVerified;
}



bsx_flags_t Niflib::calculateSkyrimBSXFlags(const vector<NiObjectRef>& blocks, const NifInfo& info) {
	bsx_flags_t flags = 0;
	NiObjectRef root = GetFirstRoot(blocks);
	int num_collisions = 0;
	int num_phantom_collisions = 0;
	bool isSkeleton = false;
	bool isRootNiNode = root->IsSameType(NiNode::TYPE);
	bool isRootBSFade = root->IsSameType(BSFadeNode::TYPE);
	bool isRootBSLeaf = root->IsSameType(BSLeafAnimNode::TYPE);
	bool isRootBSTree = root->IsSameType(BSTreeNode::TYPE);
	bool hasMultiBound = false;
	bool isSkinned = false;
	bool hasExternalSkeleton = false;
	bool hasCollisionList = false;


	set<NiObject*> bones;

	for (NiObjectRef block : blocks) {
		if (block->IsDerivedType(bhkCollisionObject::TYPE)) {
			num_collisions++;
		}
		if (block->IsDerivedType(bhkSPCollisionObject::TYPE)) {
			num_phantom_collisions++;
		}
		if (block->IsDerivedType(bhkBlendCollisionObject::TYPE))
			isSkeleton = true;
		if (block->IsDerivedType(NiSkinInstance::TYPE)) {
			NiSkinInstanceRef niskini = DynamicCast<NiSkinInstance>(block);
			isSkinned = true;
			for (NiNode* bone : niskini->GetBones())
				bones.insert(bone);
		}
		if (block->IsDerivedType(bhkListShape::TYPE))
			hasCollisionList = true;
		if (block->IsSameType(BSMultiBound::TYPE))
			hasMultiBound = true;
	}



	if (isSkinned && root->IsDerivedType(NiNode::TYPE)) {
		NiNodeRef rootn = DynamicCast<NiNode>(root);
		for (NiObjectRef ref : rootn->GetChildren()) {
			set<NiObject*>::iterator it = bones.find(ref);
			if (it != bones.end()) bones.erase(it);

			//if (ref->IsDerivedType(bhkCollisionObject::TYPE))
			//	hasRootCollision = true;
		}
		if (bones.empty())
			hasExternalSkeleton = isRootNiNode;
	}

	for (NiObjectRef block : blocks) {
		if ((block->IsDerivedType(NiTimeController::TYPE) || block->IsDerivedType(BSValueNode::TYPE)) && !isSkeleton && !hasExternalSkeleton)
		{
			flags[0] = true;
		}
		if (block->IsDerivedType(bhkRigidBody::TYPE)) {
			bhkRigidBodyRef rigid_body = DynamicCast<bhkRigidBody>(block);
			if (isSkeleton || (rigid_body->GetQualityType() != hkQualityType::MO_QUAL_INVALID && rigid_body->GetQualityType() != hkQualityType::MO_QUAL_FIXED)) {
				flags[6] = true;
			}
		}
		if (isSkeleton /*&& (block->IsDerivedType(bhkMalleableConstraint::TYPE) || block->IsDerivedType(bhkRagdollConstraint::TYPE))*/) {
			flags[2] = true;
		}
		if (block->IsDerivedType(NiNode::TYPE)) {
			NiNodeRef node = DynamicCast<NiNode>(block);
			if (node->GetName().find("AddonNode") != string::npos)
				flags[4] = true;
		}
		//if (block->IsDerivedType(NiObjectNET::TYPE)) {
		//	NiObjectNETRef node = DynamicCast<NiObjectNET>(block);
		//	if (node->GetName().find("EditorMarker") != string::npos) //TODO: wrong: only set if not parented to a NiSwitchNode, needs a visitor
		//		flags[5] = true;
		//}
		if (block->IsDerivedType(BSValueNode::TYPE)) {
			flags[4] = true;
		}
		if (block->IsDerivedType(BSLightingShaderProperty::TYPE)) {
			BSLightingShaderPropertyRef shader = DynamicCast<BSLightingShaderProperty>(block);
			if (shader->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_EXTERNAL_EMITTANCE) {
				flags[9] = true;
			}
		}
		if (block->IsDerivedType(BSEffectShaderProperty::TYPE)) {
			BSEffectShaderPropertyRef shader = DynamicCast<BSEffectShaderProperty>(block);
			if (shader->GetShaderFlags1_sk() & SkyrimShaderPropertyFlags1::SLSF1_EXTERNAL_EMITTANCE) {
				flags[9] = true;
			}
		}

	}

	bool hasRootCollision = !isRootBSTree && ((isRootBSFade && DynamicCast<BSFadeNode>(root)->GetCollisionObject() != NULL &&
		DynamicCast<BSFadeNode>(root)->GetCollisionObject()->IsDerivedType(bhkCollisionObject::TYPE)) ||
		(isRootBSLeaf && DynamicCast<BSLeafAnimNode>(root)->GetCollisionObject() != NULL &&
			DynamicCast<BSLeafAnimNode>(root)->GetCollisionObject()->IsDerivedType(bhkCollisionObject::TYPE)) ||
		hasMultiBound); //wrong. may be complex but only in 6 models, need further investigation
	if (isSingleChain(root, info))
		flags[7] = true;

	if (MarkerBranchVisitor(*root, info).marker) {
		flags[5] = true;
	}

	//if ((num_collisions > 1) && !flags[7])
	//	flags[3] = true;

	//if (!hasRootCollision)
	//	flags[3] = true;

	if (num_collisions > 0 || num_phantom_collisions > 0) {
		if (!isSkeleton && num_collisions > 0 && (!hasRootCollision || num_collisions > 1))
			flags[3] = true;
		flags[1] = true;
	}



	return flags;
}

class AccessBSDismemberedSkin {};

template<>
class Accessor<AccessBSDismemberedSkin>
{
public:
	Accessor(BSDismemberSkinInstanceRef bsskin) {
		bsskin->partitions.resize(bsskin->skinPartition->skinPartitionBlocks.size());
		for (int i = 0; i < bsskin->partitions.size(); i++)
		{
			bsskin->partitions[i].bodyPart = SBP_32_BODY;
			bsskin->partitions[i].partFlag = (BSPartFlag)(PF_EDITOR_VISIBLE | PF_START_NET_BONESET);
		}
	}
};

//remake partitions after triangulating
NiTriShapeRef remake_partitions(NiTriBasedGeomRef iShape, int & maxBonesPerPartition, int & maxBonesPerVertex, bool make_strips, bool pad)
{
	string iShapeType = "";
	NiTriShapeRef out;

	if (iShape->IsSameType(NiTriShape::TYPE)) {
		iShapeType = "NiTriShape";
		out = DynamicCast<NiTriShape>(iShape);
	}
	else if (iShape->IsSameType(NiTriStrips::TYPE)) {
		iShapeType = "NiTriStrips";
		out = new NiTriShape();
	}

	try
	{
		NiGeometryDataRef iData = iShape->GetData();
		NiSkinInstanceRef iSkinInst = iShape->GetSkinInstance(); // nif->getBlock(nif->getLink(iShape, "Skin Instance"), "NiSkinInstance");
		NiSkinDataRef iSkinData = iSkinInst->GetData();
		NiSkinPartitionRef iSkinPart = iSkinInst->GetSkinPartition();

		set<size_t> bones;

		if (iSkinPart == NULL)
			iSkinPart = iSkinData->GetSkinPartition();

		//bone -> {triangle, weight}
		auto blocks = iSkinPart->GetSkinPartitionBlocks();

		typedef std::array< tuple<size_t, size_t, float>, 12> triangle_data_t;

		vector<triangle_data_t> global_map;
		multimap< size_t, size_t> vertex_triangle_map;


		for (int i = 0; i < blocks.size(); i++)
		{
			auto& block = blocks[i];

			auto new_triangles = triangulate(block.strips);
			for (auto& tris : new_triangles)
				block.triangles.push_back(tris);

			for (size_t t = 0; t < block.triangles.size(); t++) {

				triangle_data_t data;

				size_t v1 = block.triangles[t].v1;
				size_t v2 = block.triangles[t].v2;
				size_t v3 = block.triangles[t].v3;



				auto vertexBonesV1 = block.boneIndices[v1];
				auto vertexWeightsV1 = block.vertexWeights[v1];

				for (size_t bi = 0; bi < vertexWeightsV1.size(); bi++)
				{
					float weight = vertexWeightsV1[bi];
					if (weight > 0.001)
					{
						size_t bindex = vertexBonesV1[bi];
						bones.insert(block.bones[bindex]);
						data[bi] = { block.vertexMap[v1], block.bones[bindex] , weight };
					}
					else {
						data[bi] = { -1, -1, 0.0 };
					}
				}

				auto vertexBonesV2 = block.boneIndices[v2];
				auto vertexWeightsV2 = block.vertexWeights[v2];

				for (size_t bi = 0; bi < vertexWeightsV2.size(); bi++)
				{
					float weight = vertexWeightsV2[bi];
					if (weight > 0.001)
					{
						size_t bindex = vertexBonesV2[bi];
						bones.insert(block.bones[bindex]);
						data[bi + 4] = { block.vertexMap[v2], block.bones[bindex] , weight };
					}
					else {
						data[bi + 4] = { -1, -1, 0.0 };
					}
				}

				auto vertexBonesV3 = block.boneIndices[v3];
				auto vertexWeightsV3 = block.vertexWeights[v3];

				for (size_t bi = 0; bi < vertexWeightsV3.size(); bi++)
				{
					float weight = vertexWeightsV3[bi];
					if (weight > 0.001)
					{
						size_t bindex = vertexBonesV3[bi];
						bones.insert(block.bones[bindex]);
						data[bi + 8] = { block.vertexMap[v3],  block.bones[bindex] , weight };
					}
					else {
						data[bi + 8] = { -1, -1, 0.0 };
					}
				}

				global_map.push_back(data);
			}
		}

		vector<vector<triangle_data_t>::iterator> iters;
		vector < vector < triangle_data_t>> new_partitions;

		do {
			iters.clear();
			multi_partition(global_map.begin(), global_map.end(), std::back_inserter(iters),
				[&new_partitions](triangle_data_t i) {
					bool result = true;
					for (int j = 0; j < 12; j++)
					{
						if (get<1>(i[j]) == -1)
							continue;
						if (get<1>(i[j]) < 60 * (new_partitions.size() + 1))
							result &= true;
						else
							result &= false;
					}
					return result;
				}
			);
			size_t it_distance = distance(global_map.begin(), iters[0]);
			if (it_distance == global_map.size())
				break;
			auto it = iters[0] - 1;
			vector <triangle_data_t> temp_partition(distance(global_map.begin(), it));
			move(global_map.begin(), it, temp_partition.begin());
			new_partitions.push_back(temp_partition);
			global_map.erase(global_map.begin(), it);
		} while (!iters.empty());

		vector < triangle_data_t> temp_partition(distance(global_map.begin(), global_map.end()));
		move(global_map.begin(), global_map.end(), temp_partition.begin());
		new_partitions.push_back(temp_partition);
		global_map.erase(global_map.begin(), global_map.end());

		assert(global_map.empty());

		vector<vector<boneweight> > weights;
		vector<Niflib::SkinPartition> new_blocks(new_partitions.size());
		for (size_t p = 0; p < new_partitions.size(); p++)
		{
			auto& new_partition = new_partitions[p];
			auto& new_block = new_blocks[p];
			for (size_t t = 0; t < new_partition.size(); t++)
			{
				auto& data = new_partition[t];
				Niflib::Triangle tris;
				for (size_t i = 0; i < 3; i++)
				{
					size_t absolute_vertex_index = -1;
					for (int w = 0; w < 4; w++)
					{
						if (get<0>(data[i * 4 + w]) != -1)
						{
							absolute_vertex_index = get<0>(data[i * 4 + w]);
							break;
						}
					}
					size_t relative_vertex_index;
					if (absolute_vertex_index != -1)
					{
						auto it = find(new_block.vertexMap.begin(), new_block.vertexMap.end(), absolute_vertex_index);
						if (it == new_block.vertexMap.end())
						{
							relative_vertex_index = new_block.vertexMap.size();
							new_block.vertexMap.push_back(absolute_vertex_index);
						}
						else {
							relative_vertex_index = distance(new_block.vertexMap.begin(), it);
						}
						tris[i] = relative_vertex_index;
						for (int w = 0; w < 4; w++)
						{
							size_t absolute_bone_index = get<1>(data[i * w]);
							if (absolute_bone_index != -1)
							{
								//relativize later
								float weight = get<2>(data[i * w]);
								if (new_block.boneIndices.size() < (relative_vertex_index + 1))
									new_block.boneIndices.resize(relative_vertex_index + 1);
								if (new_block.vertexWeights.size() < (relative_vertex_index + 1))
									new_block.vertexWeights.resize(relative_vertex_index + 1);
								auto& vertexBones = new_block.boneIndices[relative_vertex_index];
								auto& vertexWeights = new_block.vertexWeights[relative_vertex_index];
								if (find(vertexBones.begin(), vertexBones.end(), absolute_bone_index) == vertexBones.end())
								{
									vertexBones.push_back(absolute_bone_index);
									vertexWeights.push_back(weight);
								}
							}
						}
					}
				}
				new_block.triangles.push_back(tris);
				new_block.trianglesCopy.push_back(tris);
			}
		}

		//reorder by weight
		for (size_t p = 0; p < new_blocks.size(); p++)
		{
			auto& new_block = new_blocks[p];
			for (int i = 0; i < new_block.vertexWeights.size(); i++)
			{
				auto perm = sort_permutation(new_block.vertexWeights[i],
					[](float const& a, float const& b) { return a > b; });

				new_block.vertexWeights[i] = apply_permutation(new_block.vertexWeights[i], perm);
				new_block.boneIndices[i] = apply_permutation(new_block.boneIndices[i], perm);
				assert(new_block.vertexWeights[i].size() == new_block.boneIndices[i].size());
				while (new_block.vertexWeights[i].size() < 4) {
					new_block.vertexWeights[i].push_back(0.0);
					new_block.boneIndices[i].push_back(0);
				}
				while (new_block.vertexWeights[i].size() > 4) {
					new_block.vertexWeights[i].pop_back();
					new_block.boneIndices[i].pop_back();
				}
			}
		}

		//renormalize
		for (size_t p = 0; p < new_blocks.size(); p++)
		{
			auto& new_block = new_blocks[p];
			for (int i = 0; i < new_block.vertexWeights.size(); i++)
			{
				float sum = 0.0;
				for (int j = 0; j < new_block.vertexWeights[i].size(); j++)
				{
					sum += new_block.vertexWeights[i][j];
				}
				if (sum > 0.0)
				{
					for (int j = 0; j < new_block.vertexWeights[i].size(); j++)
					{
						new_block.vertexWeights[i][j] = new_block.vertexWeights[i][j] / sum;
					}
				}
			}
		}

		//relativize bone indexes
		for (size_t p = 0; p < new_blocks.size(); p++)
		{
			auto& new_block = new_blocks[p];
			for (int i = 0; i < new_block.vertexWeights.size(); i++)
			{
				for (int j = 0; j < new_block.vertexWeights[i].size(); j++)
				{
					if (new_block.vertexWeights[i][j] > 0.0)
					{
						size_t absolute_bone_index = new_block.boneIndices[i][j];
						size_t relative_bone_index;
						auto bone_it = find(new_block.bones.begin(), new_block.bones.end(), absolute_bone_index);
						if (bone_it == new_block.bones.end())
						{
							relative_bone_index = new_block.bones.size();
							new_block.bones.push_back(absolute_bone_index);
						}
						else {
							relative_bone_index = distance(new_block.bones.begin(), bone_it);
						}
						new_block.boneIndices[i][j] = relative_bone_index;
					}
				}
			}
			new_block.hasVertexMap = true;
			new_block.hasBoneIndices = true;
			new_block.hasVertexWeights = true;
			new_block.hasFaces = true;
		}

		iShape->GetSkinInstance()->GetSkinPartition()->SetSkinPartitionBlocks(new_blocks);
		BSDismemberSkinInstanceRef bsskin = DynamicCast<BSDismemberSkinInstance>(iSkinInst);
		if (bsskin != NULL)
			Accessor<AccessBSDismemberedSkin> fix(bsskin);

		return out;
	}
	catch (runtime_error err)
	{
		//Log::Error(err.what());
		return new NiTriShape();
	}
}

vector<Triangle> ckcmd::Geometry::triangulate(vector<unsigned short> strip)
{
	vector<Triangle> tris;
	unsigned short a, b = strip[0], c = strip[1];
	bool flip = false;

	for (int s = 2; s < strip.size(); s++) {
		a = b;
		b = c;
		c = strip[s];

		if (a != b && b != c && c != a) {
			if (!flip)
				tris.push_back(Triangle(a, b, c));
			else
				tris.push_back(Triangle(a, c, b));
		}

		flip = !flip;
	}

	//for (int i = 0; i < strip.size() - 2; i++) {
	//	Triangle t;
	//	if (i % 2) {
	//		t[0] = strip[i + 1];
	//		t[1] = strip[i];
	//		t[2] = strip[i + 2];
	//	}
	//	else {
	//		t[0] = strip[i];
	//		t[1] = strip[i + 1];
	//		t[2] = strip[i + 2];
	//	}
	//	tris.push_back(t);
	//}

	return tris;
}

vector<Triangle> ckcmd::Geometry::triangulate(vector<vector<unsigned short>> strips)
{
	vector<Triangle> tris;
	for (const vector<unsigned short>& strip : strips)
	{
		vector<Triangle> these_tris = triangulate(strip);
		tris.insert(tris.end(), these_tris.begin(), these_tris.end());
	}
	return tris;
}

Vector3 ckcmd::Geometry::centeroid(const vector<Vector3>& in) {
	Vector3 centeroid = Vector3(0.0, 0.0, 0.0);
	for (Vector3 vertex : in) {
		centeroid += vertex;
	}
	centeroid = Vector3(centeroid.x / in.size(), centeroid.y / in.size(), centeroid.z / in.size()).Normalized();
	return centeroid;
}


void ckcmd::Geometry::CalculateNormals(const vector<Vector3>& vertices, const vector<Triangle>& faces,
	vector<Vector3>& normals, Vector3& COM, bool sphericalNormals, bool calculateCOM) {

	if (!sphericalNormals)
	{
		Eigen::MatrixXf V(vertices.size(), 3);
		for (int i = 0; i < vertices.size(); i++) {
			V(i, 0) = vertices[i].x;
			V(i, 1) = vertices[i].y;
			V(i, 2) = vertices[i].z;
		}
		Eigen::MatrixXi F(faces.size(), 3);
		for (int i = 0; i < faces.size(); i++) {
			F(i, 0) = faces[i].v1;
			F(i, 1) = faces[i].v2;
			F(i, 2) = faces[i].v3;
		}

		Eigen::MatrixXf N_vertices;

		igl::per_vertex_normals(V, F, igl::PER_VERTEX_NORMALS_WEIGHTING_TYPE_UNIFORM, N_vertices);

		normals.resize(vertices.size());
		for (int i = 0; i < vertices.size(); i++) {
			normals[i] = Vector3(N_vertices(i, 0), N_vertices(i, 1), N_vertices(i, 2));
		}
	}
	else {

		std::map<unsigned int, vector<Vector3>> normalMap;

		if (normals.size() != vertices.size())
			normals.resize(vertices.size());
		//test faces before start
		set<unsigned int> faceIndexes;
		for (Triangle face : faces) {
			faceIndexes.insert(face.v1); faceIndexes.insert(face.v2); faceIndexes.insert(face.v3);
		}

		for (size_t i = 0; i < vertices.size(); i++)
			if (faceIndexes.find(i) == faceIndexes.end())
				throw runtime_error("Found unindexed vertex: " + i);

		if (calculateCOM)
			COM = centeroid(vertices);

		for (Triangle face : faces) {
			Vector3 v1 = vertices[face.v1].Normalized();
			Vector3 v2 = vertices[face.v2].Normalized();
			Vector3 v3 = vertices[face.v3].Normalized();

			Vector3 v12 = v2 - v1;
			Vector3 v13 = v3 - v1;
			Vector3 v23 = v3 - v2;

			//All the possible normals
			Vector3 n1 = v12 ^ v13;
			Vector3 n2 = v12 ^ v23;
			Vector3 n3 = v13 ^ v23;

			Vector3 COT = centeroid(vector<Vector3>({ v1,v2,v3 }));

			Vector3 COMtoCOT = Vector3(COT - COM).Normalized();

			//we always want a normal that is faced out of the body
			if (sphericalNormals) {
				Vector3 COMv1 = Vector3(v1 - COM);
				n1 = COMv1.Normalized();
				Vector3 COMv2 = Vector3(v2 - COM);
				n2 = COMv2.Normalized();
				Vector3 COMv3 = Vector3(v3 - COM);
				n3 = COMv3.Normalized();
			}
			else {
				if (n1 * COMtoCOT < 0)
					n1 = Vector3(-n1.x, -n1.y, -n1.z);
				if (n2 * COMtoCOT < 0)
					n2 = Vector3(-n2.x, -n2.y, -n2.z);
				if (n3 * COMtoCOT < 0)
					n3 = Vector3(-n3.x, -n3.y, -n3.z);
			}

			normalMap[face.v1].push_back(n1.Normalized());
			normalMap[face.v2].push_back(n2.Normalized());
			normalMap[face.v3].push_back(n3.Normalized());
		}

		for (size_t i = 0; i < vertices.size(); i++) {
			normals[i] = centeroid(normalMap[i]).Normalized();
		}
	}
}

bool CheckNormals(const vector<Vector3>& normals) {
	for (Vector3 v : normals)
		if (v != Vector3(0.0, 0.0, 0.0))
			return true;
	return false;
}