#include "stdafx.h"

#include <commands/fixsse.h>
#include <core/NifFile.h>

#include <deque>
#include <map>


using namespace ckcmd;
using namespace ckcmd::fixsse;
using namespace ckcmd::Geometry;
using namespace ckcmd::NIF;

using namespace Niflib;
using namespace std;

static bool BeginScan(string scanPath);

REGISTER_COMMAND_CPP(FixSSENif)

FixSSENif::FixSSENif()
{
}

FixSSENif::~FixSSENif()
{
}

string FixSSENif::GetName() const
{
	return "fixssenif";
}

string FixSSENif::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd nifscan [-i <path_to_scan>]
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " [-i <path_to_scan>] [-o <overwrite>] [<vanilla_texture_path>] \r\n";

	const char help[] =
		R"(Scan Skyrim Legendary Editions meshes and apply fixes for SSE compatibility.
		
		Arguments:
			<path_to_scan> path to models you want to check for errors)
			<path_to_out> output path)
			<overwrite> overwrite nif instead of using the /out/ path
			<vanilla_texture_path> search for textures in vanilla lot too)";

	return usage + help;
}

string FixSSENif::GetHelpShort() const
{
	return "TODO: Short help message for FixSSENif";
}

static inline hkTransform TOHKTRANSFORM(const Niflib::Matrix33& r, const Niflib::Vector4 t, const float scale = 1.0) {
	hkTransform out;
	out(0, 0) = r[0][0]; out(0, 1) = r[0][1]; out(0, 2) = r[0][2]; out(0, 3) = t[0] * scale;
	out(1, 0) = r[1][0]; out(1, 1) = r[1][1]; out(1, 2) = r[1][2]; out(1, 3) = t[1] * scale;
	out(2, 0) = r[2][0]; out(2, 1) = r[2][1]; out(2, 2) = r[2][2]; out(2, 3) = t[2] * scale;
	out(3, 0) = 0.0f;	 out(3, 1) = 0.0f;	  out(3, 2) = 0.0f;	   out(3, 3) = 1.0f;
	return out;
}


typedef pair<int, float> boneweight;

//! Helper for sorting a boneweight list
struct boneweight_equivalence
{
	bool operator()(const boneweight & lhs, const boneweight & rhs)
	{
		//if (lhs.second == 0.0) {
		//	if (rhs.second == 0.0) {
		//		return rhs.first < lhs.first;
		//	}
		//	else {
		//		return true;
		//	}

		//	return false;
		//}
		//else if (rhs.second == lhs.second) {
		//	return lhs.first < rhs.first;
		//}
		//else {
			return rhs.second < lhs.second;
		//}
	}
};

//! A bone and Triangle set
typedef struct
{
	list<int> bones;
	vector<Triangle> triangles;
} Partition;

//! Rotate a Triangle
inline void qRotate(Triangle & t)
{
	if (t[1] < t[0] && t[1] < t[2]) {
		t = { t[1], t[2], t[0] };
	}
	else if (t[2] < t[0]) {
		t = { t[2], t[0], t[1] };
	}
}

namespace std
{
	template<>
	struct less<Triangle> : public binary_function<Triangle, Triangle, bool>
	{
		bool operator()(const Triangle& s1, const Triangle& s2) const {
			int d = 0;
			if (d == 0) d = (s1[0] - s2[0]);
			if (d == 0) d = (s1[1] - s2[1]);
			if (d == 0) d = (s1[2] - s2[2]);
			return d < 0;
		}
	};
}

static list<int> mergeBones(list<int> a, list<int> b)
{
	for (const auto c : b) {
		if (find(a.begin(), a.end(),c)==a.end()) {
			a.push_back(c);
		}
	}
	return a;
}

static bool containsBones(list<int> a, list<int> b)
{
	for (const auto c : b) {
		if (find(a.begin(), a.end(),c)==a.end())
			return false;
	}
	return true;
}

#include <algorithm>
#include <tuple>

template <typename BidirIt, typename... Predicates>
void trivial_mul_part(BidirIt first, BidirIt last, Predicates... preds)
{
	std::sort(first, last,
		[=](decltype(*first) const&  lhs, decltype(*first) const&  rhs)
		{
			return std::make_tuple(preds(lhs)...) > std::make_tuple(preds(rhs)...);
		});
}

template <typename BidirIt, typename Out>
void multi_partition(BidirIt, BidirIt, Out) {}

template <typename BidirIt, typename OutputIterator,
	typename Pred, typename... Predicates>
	void multi_partition(BidirIt first, BidirIt last, OutputIterator out,
		Pred pred, Predicates... preds)
{
	auto iter = std::partition(first, last, pred);
	*out++ = iter;
	multi_partition<BidirIt>(iter, last, out, preds...);
}

template <typename T, typename Compare>
std::vector<std::size_t> sort_permutation(
	const std::vector<T>& vec,
	Compare& compare)
{
	std::vector<std::size_t> p(vec.size());
	std::iota(p.begin(), p.end(), 0);
	std::sort(p.begin(), p.end(),
		[&](std::size_t i, std::size_t j) { return compare(vec[i], vec[j]); });
	return p;
}

template <typename T>
std::vector<T> apply_permutation(
	const std::vector<T>& vec,
	const std::vector<std::size_t>& p)
{
	std::vector<T> sorted_vec(vec.size());
	std::transform(p.begin(), p.end(), sorted_vec.begin(),
		[&](std::size_t i) { return vec[i]; });
	return sorted_vec;
}

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

		typedef std::array< tuple<size_t, size_t, float>,12> triangle_data_t;

		vector<triangle_data_t> global_map;
		multimap< size_t, size_t> vertex_triangle_map;

		
		for (int i=0; i < blocks.size(); i++)
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



		size_t last_vertex_index = -1;
		size_t entries_per_vertex_index = 0;
		vector<vector<size_t>> influences(blocks.size());
		
		
		set<size_t> visited_vertices;
		size_t index = 0;
		//for (auto& partition : global_map)
		//{
		//	auto& influence = influences[index];
		//	for (auto entry = partition.begin(); entry!= partition.end(); entry++)
		//	{
		//		auto& ni_partition = new_blocks[index];
		//		auto this_triangles = vertex_triangle_map.equal_range(get<0>(*entry));
		//		for (auto triangle_it = this_triangles.first; triangle_it != this_triangles.second; this_triangles++)
		//		{
		//			
		//		}


		//		if (influence.size() <= get<0>(*entry))
		//		{
		//			influence.resize(get<0>(*entry) + 1, 0);
		//		}
		//		influence[get<0>(*entry)]++;
		//		if (bones.find(get<1>(*entry)) == bones.end())
		//			bones[get<1>(*entry)] = 0;
		//		bones[get<1>(*entry)]++;
		//		if (influence[get<0>(*entry)]>4) {
		//			Log::Info("Vertex %d has more than 4 bone influences!", get<0>(*entry));
		//		}
		//	}
		//	index++;
		//}



		// read in the weights from NiSkinData

		//int numVerts = iData->GetVertices().size(); //nif->get<int>(iData, "Num Vertices");
		//vector<vector<boneweight> > weights(numVerts);

		//auto& iBoneList = iSkinData->GetBoneList(); //nif->getIndex(iSkinData, "Bone List");
		//int numBones = iBoneList.size();

		//for (int bone = 0; bone < numBones; bone++) {
		//	auto& iVertexWeights = iBoneList[bone].vertexWeights;
		//	//QModelIndex iVertexWeights = nif->getIndex(iBoneList.child(bone, 0), "Vertex Weights");

		//	for (int r = 0; r < iVertexWeights.size(); r++) {
		//		int vertex = iVertexWeights[r].index; // nif->get<int>(iVertexWeights.child(r, 0), "Index");
		//		float weight = iVertexWeights[r].weight;  //nif->get<float>(iVertexWeights.child(r, 0), "Weight");

		//		if (vertex >= weights.size())
		//			throw runtime_error("bad NiSkinData - vertex count does not match");

		//		if (weight > 0.001)
		//			weights[vertex].push_back(boneweight(bone, weight));
		//	}
		//}

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
			size_t it_distance = distance(global_map.begin(), iters[0] );
			if (it_distance == global_map.size())
				break;
			auto it = iters[0]-1;
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
			for (size_t t = 0; t < new_partition.size(); t++ ) 
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
							relative_vertex_index = distance(new_block.vertexMap.begin(),it);
						}
						tris[i] = relative_vertex_index;
						for (int w = 0; w < 4; w++)
						{
							size_t absolute_bone_index = get<1>(data[i * w]);
							if (absolute_bone_index != -1)
							{
								//relativize later
								/*size_t relative_bone_index;
								auto bone_it = find(new_block.bones.begin(), new_block.bones.end(), absolute_bone_index);
								if (bone_it == new_block.bones.end())
								{
									relative_bone_index = new_block.bones.size();
									new_block.bones.push_back(absolute_bone_index);
								}
								else {
									relative_bone_index = *bone_it;
								}*/
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
			for (int i = 0; i< new_block.vertexWeights.size(); i++)
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
						new_block.vertexWeights[i][j] = new_block.vertexWeights[i][j]/sum;
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

		// count min and max bones per vertex


		return out;
	}
	catch (runtime_error err)
	{
		Log::Error(err.what());
		return new NiTriShape();
	}
}

class RebuildVisitor : public RecursiveFieldVisitor<RebuildVisitor> {
	set<NiObject*> objects;
public:
	vector<NiObjectRef> blocks;

	RebuildVisitor(NiObject* root, const NifInfo& info) :
		RecursiveFieldVisitor(*this, info) {
		root->accept(*this, info);

		for (NiObject* ptr : objects) {
			blocks.push_back(ptr);
		}
	}


	template<class T>
	inline void visit_object(T& obj) {
		objects.insert(&obj);
	}


	template<class T>
	inline void visit_compound(T& obj) {
	}

	template<class T>
	inline void visit_field(T& obj) {}
};

class FixSSETargetsVisitor : public RecursiveFieldVisitor<FixSSETargetsVisitor> {
	vector<NiObjectRef>& blocks;
public:


	FixSSETargetsVisitor(NiObject* root, const NifInfo& info, vector<NiObjectRef>& blocks) :
		RecursiveFieldVisitor(*this, info), blocks(blocks) {
		root->accept(*this, info);
	}


	template<class T>
	inline void visit_object(T& obj) {}

	template<>
	inline void visit_object(NiDefaultAVObjectPalette& obj) {
		vector<AVObject > av_objects = obj.GetObjs();
		for (AVObject& av_object : av_objects) {
			for (NiObjectRef ref : blocks) {
				if (ref->IsDerivedType(NiAVObject::TYPE)) {
					NiAVObjectRef av_ref = DynamicCast<NiAVObject>(ref);
					if (av_ref->GetName() == av_object.name) {
						av_object.avObject = DynamicCast<NiAVObject>(av_ref);
					}
				}
			}
		}
		obj.SetObjs(av_objects);
	}

	template<class T>
	inline void visit_compound(T& obj) {
	}

	template<>
	void visit_compound(AVObject& avlink) {
		//relink av objects on converted nistrips;

	}

	template<class T>
	inline void visit_field(T& obj) {}
};

NiTriShapeRef destrip(NiTriStripsRef& stripsRef)
{
	//Convert NiTriStrips to NiTriShapes first of all.
	NiTriShapeRef shapeRef = new NiTriShape();
	shapeRef->SetName(stripsRef->GetName());
	shapeRef->SetExtraDataList(stripsRef->GetExtraDataList());
	shapeRef->SetTranslation(stripsRef->GetTranslation());
	shapeRef->SetRotation(stripsRef->GetRotation());
	shapeRef->SetScale(stripsRef->GetScale());
	shapeRef->SetFlags(524302);
	shapeRef->SetData(stripsRef->GetData());
	shapeRef->SetShaderProperty(stripsRef->GetShaderProperty());
	shapeRef->SetProperties(stripsRef->GetProperties());
	shapeRef->SetAlphaProperty(stripsRef->GetAlphaProperty());

	NiTriStripsDataRef stripsData = DynamicCast<NiTriStripsData>(stripsRef->GetData());
	NiTriShapeDataRef shapeData = new  NiTriShapeData();

	shapeData->SetHasVertices(stripsData->GetHasVertices());
	shapeData->SetVertices(stripsData->GetVertices());
	shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(stripsData->GetVectorFlags()));
	shapeData->SetUvSets(stripsData->GetUvSets());
	if (!shapeData->GetUvSets().empty())
		shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(shapeData->GetBsVectorFlags() | BSVF_HAS_UV));
	shapeData->SetCenter(stripsData->GetCenter());
	shapeData->SetRadius(stripsData->GetRadius());
	shapeData->SetHasVertexColors(stripsData->GetHasVertexColors());
	shapeData->SetVertexColors(stripsData->GetVertexColors());
	shapeData->SetConsistencyFlags(stripsData->GetConsistencyFlags());
	vector<Triangle> triangles = triangulate(stripsData->GetPoints());
	shapeData->SetNumTriangles(triangles.size());
	shapeData->SetNumTrianglePoints(triangles.size() * 3);
	shapeData->SetHasTriangles(1);
	shapeData->SetTriangles(triangles);

	shapeData->SetHasNormals(stripsData->GetHasNormals());
	shapeData->SetNormals(stripsData->GetNormals());

	vector<Vector3> vertices = shapeData->GetVertices();
	Vector3 COM;
	if (vertices.size() != 0)
	COM = (COM / 2) + (ckcmd::Geometry::centeroid(vertices) / 2);
	vector<Triangle> faces = shapeData->GetTriangles();
	vector<Vector3> normals = shapeData->GetNormals();

	if (vertices.size() != 0 && faces.size() != 0 && shapeData->GetUvSets().size() != 0) {
		vector<TexCoord> uvs = shapeData->GetUvSets()[0];
		TriGeometryContext g(vertices, COM, faces, uvs, normals);
		shapeData->SetHasNormals(1);
		//recalculate
		shapeData->SetNormals(g.normals);
		shapeData->SetTangents(g.tangents);
		shapeData->SetBitangents(g.bitangents);
		if (vertices.size() != g.normals.size() || vertices.size() != g.tangents.size() || vertices.size() != g.bitangents.size())
			throw runtime_error("Geometry mismatch!");
		shapeData->SetBsVectorFlags(static_cast<BSVectorFlags>(shapeData->GetBsVectorFlags() | BSVF_HAS_TANGENTS));
	}
	else {
		shapeData->SetTangents(stripsData->GetTangents());
		shapeData->SetBitangents(stripsData->GetBitangents());
	}

	shapeRef->SetData(DynamicCast<NiGeometryData>(shapeData));

	//TODO: shared normals no more supported
	shapeData->SetMatchGroups(vector<MatchGroup>{});

	shapeRef->SetSkin(stripsRef->GetSkin());
	shapeRef->SetSkinInstance(stripsRef->GetSkinInstance());

	return shapeRef;
}

inline void visit_object(NiNodeRef obj) {
	vector<Ref<NiAVObject>> children = obj->GetChildren();
	vector<Ref<NiProperty>> properties = obj->GetProperties();
	vector<Ref<NiAVObject>>::iterator eraser = children.begin();
	while (eraser != children.end())
	{
		if (*eraser == NULL) {
			eraser = children.erase(eraser);
		}
		else
			eraser++;
	}
	int index = 0;
	for (NiAVObjectRef& block : children)
	{
		if (block->IsSameType(NiTriStrips::TYPE)) {
			NiTriStripsRef stripsRef = DynamicCast<NiTriStrips>(block);
			NiTriShapeRef shape = destrip(stripsRef);
			children[index] = shape;
		}

		index++;
	}
	for (NiAVObjectRef& block : children)
	{
		if (block->IsSameType(NiTriShape::TYPE)) {
			bool hasStrips = false;
			NiTriShapeRef shape = DynamicCast<NiTriShape>(block);
			NiSkinInstanceRef skin = shape->GetSkinInstance();
			if (skin != NULL) {
				NiSkinDataRef iSkinData = skin->GetData();
				NiSkinPartitionRef iSkinPart = skin->GetSkinPartition();

				if (iSkinPart == NULL)
					iSkinPart = iSkinData->GetSkinPartition();
				if (iSkinPart != NULL)
				{
					vector<SkinPartition >& pblocks = iSkinPart->GetSkinPartitionBlocks();
					for (const auto& pb : pblocks)
					{
						if (pb.strips.size() > 0)
						{
							hasStrips = true;
							break;
						}
					}
				}
			}

			if (hasStrips) {
				//redo partitions destripping
				NiTriBasedGeomRef geo = StaticCast<NiTriBasedGeom>(shape);
				int bb = 60;
				int bv = 4;
				//repartitioner.cast(nif, iBlock, bb, bv, false, false);
				remake_partitions(geo, bb, bv, false, false);
			}
		}
	}

	obj->SetChildren(children);
}

void findTextureOrTryToCorrect(BSShaderTextureSetRef texture_set, const fs::path& texture_folder_path, const fs::path& vanilla_path)
{
	vector<string> textures = texture_set->GetTextures();
	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i].empty())
			continue;

		//vurt, what are you doing?
		if (textures[i].substr(1, textures[i].size() - 1) == "NOR")
		{
			textures[i].clear();
			continue;
		}

		string this_tex = textures[i];
		fs::path texture_path = texture_folder_path / this_tex;
		if (fs::exists(texture_path))
			continue;
		texture_path = texture_folder_path / "textures" / this_tex;
		if (fs::exists(texture_path))
		{
			textures[i] = (fs::path("textures") / this_tex).string().c_str();
			continue;
		}
		texture_path = vanilla_path / this_tex;
		if (fs::exists(texture_path))
			continue;
		texture_path = vanilla_path / "textures" / this_tex;
		if (fs::exists(texture_path))
		{
			textures[i] = (fs::path("textures") / this_tex).string().c_str();
			continue;
		}
	}
	texture_set->SetTextures(textures);
}

inline void scanBSProperties(NiTriShapeRef shape, const fs::path& texture_folder_path, const fs::path& vanilla_path)
{
	//check texture sets
	string shape_name = shape->GetName();
	if (shape_name.find("EditorMarker") != string::npos)
		return;

	BSLightingShaderPropertyRef property = DynamicCast<BSLightingShaderProperty>(shape->GetShaderProperty());
	if (shape->GetShaderProperty() == NULL)
	{
		Log::Info("Missing any kind of shader property on trishape %s", shape_name.c_str());
		return;
	}
	if (property == NULL)
	{
		//BSEffect
		return;
	}

	if (shape->GetData() == NULL)
	{
		Log::Info("Missing Geometry Data trishape %s", shape_name.c_str());
		return;
	}

	if (property->GetTextureSet() == NULL || property->GetTextureSet()->GetTextures().empty())
	{
		Log::Info("Missing Texture Set on trishape %s", shape_name.c_str());
		return;
	}

	NiAlphaPropertyRef alpha = shape->GetAlphaProperty();
	BSShaderTextureSetRef textures = property->GetTextureSet();
	BSLightingShaderPropertyShaderType shader_type = property->GetSkyrimShaderType();

	findTextureOrTryToCorrect(textures, texture_folder_path, vanilla_path);

	bool hasVCFlag = shape->GetData()->GetHasVertexColors();
	bool hasVCArray = shape->GetData()->GetVertexColors().size() == shape->GetData()->GetVertices().size();

	if (hasVCFlag != hasVCArray)
	{
		Log::Info("Shape %s: shape VC inconsistant!", shape_name.c_str());
		if (hasVCFlag && shape->GetData()->GetVertexColors().size() != shape->GetData()->GetVertices().size())
		{
			//resizing vcs
			Log::Info("Shape %s: resizing vc!", shape_name.c_str());
			vector<Color4>& vcs = shape->GetData()->GetVertexColors();
			vcs.resize(shape->GetData()->GetVertexColors().size());
			shape->GetData()->SetVertexColors(vcs);
		}
	}
	else {
		if (hasVCFlag)
			property->SetShaderFlags2_sk((SkyrimShaderPropertyFlags2)(property->GetShaderFlags2_sk() | SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS));
		else
			property->SetShaderFlags2_sk((SkyrimShaderPropertyFlags2)(property->GetShaderFlags2_sk() & ~SkyrimShaderPropertyFlags2::SLSF2_VERTEX_COLORS));
	}

	//Check VS all black
	bool allBlack = true;
	vector<Color4>& vcs = shape->GetData()->GetVertexColors();
	if (vcs.size() > 0)
	{
		for (const auto& vc : vcs)
		{
			if (vc.r != 0.0 || vc.g != 0.0 && vc.b != 0.0)
			{
				allBlack = false;
				break;
			}
		}
		if (allBlack)
		{
			Log::Info("Shape %s: has all black Vertex Colors, turning to full white!", shape_name.c_str());
			for (auto& vc : vcs)
			{
				vc = Color4(1.0, 1.0, 1.0);
			}
			shape->GetData()->SetVertexColors(vcs);
		}
	}

	//Check first two texture slots:
	//Emissive
	fs::path texture_slot_0 = texture_folder_path / textures->GetTextures()[0];
	if (!fs::exists(texture_slot_0) && !fs::exists(vanilla_path / textures->GetTextures()[0]))
		Log::Warn("Shape %s: Unable to find Emissive texture: %s (not even in vanilla path %s)", shape_name.c_str(), texture_slot_0.string().c_str(), vanilla_path.string().c_str());
	//Normal
	fs::path texture_slot_1 = texture_folder_path / textures->GetTextures()[1];
	if (!fs::exists(texture_slot_1) && !fs::exists(vanilla_path / textures->GetTextures()[1]))
	{
		if (!textures->GetTextures()[1].empty())
		{
			Log::Warn("Shape %s: Unable to find Normal texture: %s (not even in vanilla path %s)", shape_name.c_str(), texture_slot_1.string().c_str(), vanilla_path.string().c_str());
		}
		else {
			Log::Warn("Empty Normal texture, setting default");
			vector<string> texture_s = textures->GetTextures();
			texture_s[1] = "textures\\default_n.dds";
			textures->SetTextures(texture_s);
		}
	}


	if (shader_type == BSLightingShaderPropertyShaderType::ST_ENVIRONMENT_MAP /*|| BSLightingShaderPropertyShaderType::ST_EYE_ENVMAP*/) {
		//Environment
		fs::path texture_slot_4 = texture_folder_path / textures->GetTextures()[4];
		fs::path texture_slot_5 = texture_folder_path / textures->GetTextures()[5];
		if (!fs::exists(texture_slot_4) && !fs::exists(texture_slot_5) && !fs::exists(vanilla_path / textures->GetTextures()[4]) && !fs::exists(texture_folder_path / textures->GetTextures()[5]))
			Log::Error("Shape %s: ShaderType is 'Environment', but no 'Environment' or 'Cubemap' texture is present at %s or %s ", shape_name.c_str(), texture_slot_4.string().c_str(), texture_slot_5.string().c_str());
		
		if (property->GetEnvironmentMapScale() == 0) {
			Log::Error("Shape %s: ShaderType is 'Environment', but map scale equals 0, making it obsolete. Setting to 1", shape_name.c_str());
			property->SetEnvironmentMapScale(1.0);
		}

		property->SetShaderFlags1_sk((SkyrimShaderPropertyFlags1)(property->GetShaderFlags1_sk() | SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING));
		property->SetShaderFlags2_sk((SkyrimShaderPropertyFlags2)(property->GetShaderFlags2_sk() & ~SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP));
	}
	if (shader_type == BSLightingShaderPropertyShaderType::ST_GLOW_SHADER) {
		// Glow Map
		fs::path texture_slot_2 = texture_folder_path / textures->GetTextures()[2];
		if (!fs::exists(texture_slot_2) && !fs::exists(vanilla_path / textures->GetTextures()[2]))
			Log::Error("Shape %s: ShaderType is 'Glow', but no 'Glow' texture is present %s.", shape_name.c_str(), texture_slot_2.string().c_str());

		property->SetShaderFlags1_sk((SkyrimShaderPropertyFlags1)(property->GetShaderFlags1_sk() & ~SkyrimShaderPropertyFlags1::SLSF1_ENVIRONMENT_MAPPING));
		property->SetShaderFlags1_sk((SkyrimShaderPropertyFlags1)(property->GetShaderFlags1_sk() | SkyrimShaderPropertyFlags1::SLSF1_EXTERNAL_EMITTANCE));
		property->SetShaderFlags2_sk((SkyrimShaderPropertyFlags2)(property->GetShaderFlags2_sk() | SkyrimShaderPropertyFlags2::SLSF2_GLOW_MAP));
	}

	if ((property->GetShaderFlags2_sk() & SLSF2_TREE_ANIM) > 0)
	{
		if (shape->GetData()->GetVertexColors().empty())
		{
			Log::Error("Shape %s: SLSF2_TREE_ANIM is set, creating full white vertex colors", shape_name.c_str());
			vector<Color4> vcs(shape->GetData()->GetVertices().size());
			for (auto& vc : vcs)
			{
				vc = Color4(1.0, 1.0, 1.0);
			}
			shape->GetData()->SetVertexColors(vcs);
		}
	}

}

extern void ScanNif(vector<NiObjectRef>& blocks, NifInfo info);

typedef bitset<12> bsx_flags_t;
namespace ckcmd {
	namespace nifscan
	{
		extern bsx_flags_t calculateSkyrimBSXFlags(const vector<NiObjectRef>& blocks, const NifInfo& info);
	}
}

void check(bhkCollisionObjectRef co)
{
	if (co->GetBody() == NULL)
	{
		Log::Error("Collision Object without Rigid Body. FATAL");
		return;
	}
	bhkRigidBodyRef rb = DynamicCast<bhkRigidBody>(co->GetBody());
	if (rb == NULL)
	{
		return;
	}
	SkyrimLayer layer = rb->GetHavokFilter().layer_sk;
	if (layer == SKYL_CLUTTER)
	{
		co->SetFlags((bhkCOFlags)(BHKCO_ACTIVE | BHKCO_SET_LOCAL | BHKCO_SYNC_ON_UPDATE));
	}
	else if (layer == SKYL_ANIMSTATIC)
	{
		co->SetFlags((bhkCOFlags)(BHKCO_ACTIVE | BHKCO_SET_LOCAL));
	}
	else if (layer == SKYL_STATIC)
	{
		co->SetFlags((bhkCOFlags)(BHKCO_ACTIVE));
	}
}

void check(bhkRigidBodyRef rb)
{
	rb->SetBroadPhaseType(BROAD_PHASE_ENTITY);
	hkWorldObjCinfoProperty cinfo;
	cinfo.data = 0;
	cinfo.size = 0;
	cinfo.capacityAndFlags = 2147483648;
	rb->SetCinfoProperty(cinfo);
	HavokFilter original = rb->GetHavokFilter();
	//check layer
	if (string(NifFile::layer_name(original.layer_sk)) == string("SKYL_UNIDENTIFIED"))
	{
		Log::Error("Faulty Havok Layer. Setting it to STATIC");
		original.layer_sk = SKYL_STATIC;
	}
	rb->SetHavokFilter(original);
	rb->SetHavokFilterCopy(original);
	rb->SetCollisionResponse(RESPONSE_SIMPLE_CONTACT);
	rb->SetCollisionResponse2(RESPONSE_SIMPLE_CONTACT);
	rb->SetProcessContactCallbackDelay(65535);
	rb->SetProcessContactCallbackDelay2(65535);
	if (rb->GetHavokFilter().layer_sk == SkyrimLayer::SKYL_CLUTTER) {
		rb->SetMotionSystem(MO_SYS_DYNAMIC);
		rb->SetSolverDeactivation(SOLVER_DEACTIVATION_LOW);
		rb->SetQualityType(MO_QUAL_MOVING);
	}
	if (rb->GetMotionSystem() == MO_SYS_KEYFRAMED)
		rb->SetMotionSystem(MO_SYS_BOX_INERTIA);
	if (rb->GetQualityType() == MO_QUAL_KEYFRAMED || rb->GetQualityType() == MO_QUAL_KEYFRAMED_REPORT)
		rb->SetQualityType(MO_QUAL_FIXED);
}

void check(bhkMoppBvTreeShapeRef mopp)
{
	if (mopp->GetMoppData().empty())
	{
		Log::Error("Empty MOPP: TODO: Recalculate");
	}
	if (mopp->GetShape() == NULL)
	{
		Log::Error("Empty MOPP SHAPE: TODO: Recalculate");
	}
}

void check_collisions(vector<NiObjectRef>& blocks)
{
	for (auto& block : blocks)
	{
		if (block->IsDerivedType(bhkRigidBody::TYPE))
		{
			check(DynamicCast<bhkRigidBody>(block));
		}
		if (block->IsDerivedType(bhkCollisionObject::TYPE))
		{
			check(DynamicCast<bhkCollisionObject>(block));
		}
		if (block->IsDerivedType(bhkMoppBvTreeShape::TYPE))
		{
			check(DynamicCast<bhkMoppBvTreeShape>(block));
		}
	}
}

void check_physics(NiNodeRef collision_parent, vector<pair<hkTransform, NiTriShapeRef>>& geometry_meshes)
{

}


typedef map<NiNodeRef, vector<pair<hkTransform, NiTriShapeRef>>> bodies_meshes_map_t;

void rebuild_collisions(NiObjectRef root, vector<NiObjectRef>& blocks) {
	bodies_meshes_map_t bodies_meshes_map;
	std::deque<NiObjectRef> visit_stack;
	set<NiNodeRef> animated_nodes;

	//build a list of animated nodes, if any
	for (const auto& block : blocks)
	{
		if (block->IsDerivedType(NiControllerSequence::TYPE))
		{
			NiControllerSequenceRef seq = DynamicCast<NiControllerSequence>(block);
			for (const auto& cb : seq->GetControlledBlocks()) {
				if (cb.controller->GetTarget()->IsDerivedType(NiNode::TYPE))
				{
					animated_nodes.insert(DynamicCast<NiNode>(cb.controller->GetTarget()));
				}
			}
		}
	}

	std::function<void(NiObjectRef, std::deque<NiObjectRef>&)> findShapesToBeCollisioned = [&](NiObjectRef current_node, std::deque<NiObjectRef>& stack) {
		//recurse until we find a shape, avoiding rb as their mesh will be taken into account later
		stack.push_front(current_node);
		if (current_node->IsDerivedType(NiNode::TYPE))
		{
			vector<NiAVObjectRef> children = DynamicCast<NiNode>(current_node)->GetChildren();
			for (int i = 0; i < children.size(); i++) {
				findShapesToBeCollisioned(StaticCast<NiObject>(children[i]), stack);
			}
		}
		if (current_node->IsDerivedType(NiTriShape::TYPE))
		{
			//I'm a mesh, find my nearest parent RB, if any
			NiNodeRef parent_collisioner = NULL;
			hkTransform transform_from_rigid_body; transform_from_rigid_body.setIdentity();
			for (const auto& element : stack)
			{
				NiObjectRef parent = element;
				if (parent->IsDerivedType(NiNode::TYPE))
				{
					NiNodeRef parent_node = DynamicCast<NiNode>(parent);
					hkTransform this_transform = TOHKTRANSFORM(parent_node->GetRotation(), parent_node->GetTranslation(), parent_node->GetScale());
					transform_from_rigid_body.setMul(this_transform, transform_from_rigid_body);

					if (parent_node->GetCollisionObject() != NULL || 
						animated_nodes.find(parent_node) != animated_nodes.end())
					{
						parent_collisioner = parent_node;
						break;
					}
				}
			}
			bodies_meshes_map_t::iterator it = bodies_meshes_map.find(parent_collisioner);
			if (it == bodies_meshes_map.end())
				bodies_meshes_map[parent_collisioner] = vector<pair<hkTransform, NiTriShapeRef>>();
			bodies_meshes_map[parent_collisioner].push_back({ transform_from_rigid_body, DynamicCast<NiTriShape>(current_node) });
		}
		stack.pop_front();
	};

	findShapesToBeCollisioned(root, visit_stack);
	Log::Info("Found shapes/collisions relation");

	//todo: avoid build collisions for vfx, skins

	for (auto& entry : bodies_meshes_map)
	{
		check_physics(entry.first, entry.second);
	}	
}


vector<NiObjectRef> fixssenif(vector<NiObjectRef> blocks, NifInfo info, const fs::path& texture_path, const fs::path& vanilla_texture_path) {

	NiObjectRef root = GetFirstRoot(blocks);
		
	for (auto& block : blocks) {
		if (block->IsDerivedType(NiNode::TYPE))
		{
			visit_object(DynamicCast<NiNode>(block));
		}
	}

	//to calculate the right flags, we need to rebuild the blocks
	vector<NiObjectRef> new_blocks = RebuildVisitor(root, info).blocks;

	//fix targets from nitrishapes substitution
	FixSSETargetsVisitor(GetFirstRoot(new_blocks), info, new_blocks);

	BSXFlagsRef bsx_flags = NULL;

	for (int i = 0; i < new_blocks.size(); i++) {
		auto& block = new_blocks[i];
		if (block->IsDerivedType(BSXFlags::TYPE))
			bsx_flags = DynamicCast<BSXFlags>(block);
		if (block->IsDerivedType(NiTriShape::TYPE))
			scanBSProperties(DynamicCast<NiTriShape>(block), texture_path, vanilla_texture_path);
		if (block->IsDerivedType(NiTriShapeData::TYPE))
		{
			NiTriShapeDataRef data = DynamicCast<NiTriShapeData>(block);
			vector<Vector3> vertices = data->GetVertices();
			Vector3 COM;
			if (vertices.size() != 0)
				COM = (COM / 2) + (ckcmd::Geometry::centeroid(vertices) / 2);
			vector<Triangle> faces = data->GetTriangles();
			vector<Vector3> normals = data->GetNormals();
			if (normals.size() != vertices.size() && (faces.empty() || data->GetUvSets().empty()))
			{
				Log::Error("Model has no normals or no faces or no UV. Won't be able to calculate tangent space");
				//normals are needed for sse, let's just fill the void here
				normals.resize(vertices.size());
				for (int i = 0; i< vertices.size(); i++)
				{
					normals[i] = COM - vertices[i];
					normals[i] = normals[i].Normalized();
				}
				data->SetHasNormals(true);
				data->SetNormals(normals);
			}
			if (vertices.size() != 0 && faces.size() != 0 && data->GetUvSets().size() != 0) {
				vector<TexCoord> uvs = data->GetUvSets()[0];

				//Seems that tangent space calculation wants the uv flipped
				for (auto& uv : uvs)
				{
					float u = uv.u;
					uv.u = uv.v;
					uv.v = u;
				}

				TriGeometryContext g(vertices, COM, faces, uvs, normals);
				data->SetHasNormals(1);
				data->SetNormals(g.normals);
				data->SetTangents(g.tangents);
				data->SetBitangents(g.bitangents);
				data->SetBsVectorFlags(static_cast<BSVectorFlags>(data->GetBsVectorFlags() | BSVF_HAS_TANGENTS));
			}
		}
		if (new_blocks[i]->IsDerivedType(NiTimeController::TYPE)) {
			NiTimeControllerRef ref = DynamicCast<NiTimeController>(new_blocks[i]);
			if (ref->GetTarget() == NULL) {
				Log::Error("Block[%i]: Controller has no target. This will increase the chances of a crash.", i);
			}
		}

		if (new_blocks[i]->IsDerivedType(NiControllerSequence::TYPE)) {
			NiSequenceRef ref = DynamicCast<NiSequence>(new_blocks[i]);

			if (ref->GetControlledBlocks().size() != 0) {
				vector<ControlledBlock> blocks = ref->GetControlledBlocks();

				for (int y = 0; y != blocks.size(); y++) {
					if (blocks[y].controllerType == "") {
						Log::Error("Block[%i]: ControlledBlock number %i, has a blank controller type.", i, y);
					}
				}
			}
		}
	}

	if (bsx_flags != NULL)
	{
		bsx_flags_t actual = ckcmd::nifscan::calculateSkyrimBSXFlags(new_blocks, info);
		bsx_flags->SetIntegerData(actual.to_ulong());
	}

	set<NiObjectRef> roots = FindRoots(new_blocks);
	if (roots.size() != 1)
		throw runtime_error("Model has multiple roots!");

	check_collisions(new_blocks);
	rebuild_collisions(GetFirstRoot(new_blocks), new_blocks);

	return move(new_blocks);
}


bool FixSSENif::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string scanPath;
	string vanilla_texture_path = "";
	bool doOverwrite = false;
	
	if (parsedArgs["<overwrite>"].asString() == "true")
		doOverwrite = true;
	if (parsedArgs.find("<vanilla_texture_path>") != parsedArgs.end() &&
		parsedArgs["<vanilla_texture_path>"].isString())
		vanilla_texture_path = parsedArgs["<vanilla_texture_path>"].asString();


	scanPath = parsedArgs["<path_to_scan>"].asString();
	Log::Info("Scan Path: %s", scanPath.c_str());
	Log::Info("Vanilla texture Path: %s", vanilla_texture_path.c_str());
	if (fs::exists(scanPath) && fs::is_directory(scanPath)) {
		vector<fs::path> nifs; find_files(scanPath, ".nif", nifs);
		fs::path texture_path = fs::path(scanPath).parent_path();
		for (size_t i = 0; i < nifs.size(); i++) {
			Log::Info("Current File: %s", nifs[i].string().c_str());
			NifInfo info;
			try {
				vector<NiObjectRef> blocks = ReadNifList(nifs[i].string().c_str(), &info);
				vector<NiObjectRef> new_blocks = fixssenif(blocks, info, texture_path, vanilla_texture_path);
				fs::path out;
				if (!doOverwrite) {
					out = fs::path(scanPath).parent_path() / fs::path("out") / relative_to(nifs[i], scanPath);
					fs::path parent_out = out.parent_path();
					if (!fs::exists(parent_out))
						fs::create_directories(parent_out);
				}
				else {
					out = nifs[i];
				}
				WriteNifTree(out.string(), GetFirstRoot(new_blocks), info);
				blocks = ReadNifList(out.string().c_str(), &info);
				for (const auto& block : blocks)
				{
					if (block->IsDerivedType(NiTriStrips::TYPE) || block->IsDerivedType(NiTriStripsData::TYPE)) {
						throw std::runtime_error("Round trip error, model not triangulated!");
					}
				}
			}
			catch (const std::exception& e) {
				Log::Info("ERROR: %s", e.what());
				fs::path out = fs::path(scanPath).parent_path() / fs::path("error") / relative_to(nifs[i], scanPath);
				fs::path parent_out = out.parent_path();
				if (!fs::exists(parent_out))
					fs::create_directories(parent_out);
				fs::copy(nifs[i], out, fs::copy_options::overwrite_existing);
			}
			catch (...) {
				Log::Info("SERIOUS ERROR");
				fs::path out = fs::path(scanPath).parent_path() / fs::path("error") / relative_to(nifs[i], scanPath);
				fs::path parent_out = out.parent_path();
				if (!fs::exists(parent_out))
					fs::create_directories(parent_out);
				fs::copy(nifs[i], out, fs::copy_options::overwrite_existing);
			}
		}
		Log::Info("Done..");
	}
	return true;
}