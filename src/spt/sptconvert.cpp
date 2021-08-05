//======Copyright © 1996-2006, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// A simple tool for converting SpeedTree .spt files into .smd files
// for use in Source
//
//===========================================================================//



#include <spt/SPT.h>
#include <SpeedTreeRT.h>

#include <core/games.h>
#include <core/bsa.h>
#include <commands/Geometry.h>

#include <map>
#include <random>
#include <assert.h>

constexpr double PI = 3.14159265358;

//NIF

#include <niflib.h>
#include <obj\NiObject.h>
#include <obj\BSFadeNode.h>

#include <objDecl.cpp>
#include <field_visitor.h>
#include <interfaces\typed_visitor.h>

//hierarchy
#include <obj/NiTimeController.h>
#include <obj/NiExtraData.h>
#include <obj/NiCollisionObject.h>
#include <obj/NiProperty.h>
#include <obj/NiDynamicEffect.h>

//Collisions
#include <obj/NiTriBasedGeom.h>
#include <obj/NiTriBasedGeomData.h>
#include <obj/bhkPackedNiTriStripsShape.h>
#include <obj/hkPackedNiTriStripsData.h>
#include <obj/bhkRigidBody.h>
#include <obj/bhkMoppBvTreeShape.h>
#include <obj/bhkNiTriStripsShape.h>
#include <obj/NiTriStripsData.h>
#include <obj/bhkCompressedMeshShape.h>
#include <obj/bhkCompressedMeshShapeData.h>

#include <Physics\Utilities\Collide\ShapeUtils\CreateShape\hkpCreateShapeUtility.h>
#include <Common\Base\Types\Geometry\hkStridedVertices.h>
#include <Common\Internal\ConvexHull\hkGeometryUtility.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Common\GeometryUtilities\Misc\hkGeometryUtils.h>
#include <Physics\Collide\Shape\Convex\ConvexVertices\hkpConvexVerticesConnectivity.h>
#include <Physics\Utilities\Collide\ShapeUtils\ShapeConverter\hkpShapeConverter.h>

#include "triangleintersects.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>
#include <utility>

#undef max
#undef min

using namespace Niflib;
using namespace ckcmd::info;
using namespace ckcmd::BSA;
using namespace ckcmd::Geometry;

using namespace std;

static Games& games = Games::Instance();

#define MAX_PATH 1024
#define COLLISION_RATIO 0.01428f

static inline Niflib::Vector3 TOVECTOR3(const hkVector4& v) {
	return Niflib::Vector3(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2));
}

static inline Matrix44 TOMATRIX44(const hkTransform& q, const float scale = 1.0f, bool inverse = false) {

	hkVector4 c0 = q.getColumn(0);
	hkVector4 c1 = q.getColumn(1);
	hkVector4 c2 = q.getColumn(2);
	hkVector4 c3 = q.getColumn(3);

	return Matrix44(
		c0.getSimdAt(0), c1.getSimdAt(0), c2.getSimdAt(0), (float)c3.getSimdAt(0) * scale,
		c0.getSimdAt(1), c1.getSimdAt(1), c2.getSimdAt(1), (float)c3.getSimdAt(1) * scale,
		c0.getSimdAt(2), c1.getSimdAt(2), c2.getSimdAt(2), (float)c3.getSimdAt(2) * scale,
		c0.getSimdAt(3), c1.getSimdAt(3), c2.getSimdAt(3), c3.getSimdAt(3)
	);
}

static inline Matrix33 TOMATRIX33(const hkRotation& q) {

	hkVector4 c0 = q.getColumn(0);
	hkVector4 c1 = q.getColumn(1);
	hkVector4 c2 = q.getColumn(2);

	return Matrix33(
		c0.getSimdAt(0), c1.getSimdAt(0), c2.getSimdAt(0),
		c0.getSimdAt(1), c1.getSimdAt(1), c2.getSimdAt(1),
		c0.getSimdAt(2), c1.getSimdAt(2), c2.getSimdAt(2)
	);
}

static inline Matrix33 TOMATRIX33(const hkTransform& q) {

	hkVector4 c0 = q.getColumn(0);
	hkVector4 c1 = q.getColumn(1);
	hkVector4 c2 = q.getColumn(2);

	return Matrix33(
		c0.getSimdAt(0), c1.getSimdAt(0), c2.getSimdAt(0),
		c0.getSimdAt(1), c1.getSimdAt(1), c2.getSimdAt(1),
		c0.getSimdAt(2), c1.getSimdAt(2), c2.getSimdAt(2)
	);
}

static inline hkTransform TOHKTRANSFORM(const Niflib::Matrix33& r, const Niflib::Vector4 t, const float scale = 1.0) {
	hkTransform out;
	out(0, 0) = r[0][0]; out(0, 1) = r[0][1]; out(0, 2) = r[0][2]; out(0, 3) = t[0] * scale;
	out(1, 0) = r[1][0]; out(1, 1) = r[1][1]; out(1, 2) = r[1][2]; out(1, 3) = t[1] * scale;
	out(2, 0) = r[2][0]; out(2, 1) = r[2][1]; out(2, 2) = r[2][2]; out(2, 3) = t[2] * scale;
	out(3, 0) = 0.0f;	 out(3, 1) = 0.0f;	  out(3, 2) = 0.0f;	   out(3, 3) = 1.0f;
	return out;
}

static inline hkVector4 TOVECTOR4(const Niflib::Vector4& v) {
	return hkVector4(v.x, v.y, v.z, v.w);
}

static inline hkVector4 TOVECTOR3(const Niflib::Float3& v) {
	return hkVector4(v[0], v[1], v[2]);
}

static inline hkRotation TOMATRIX3(const Niflib::Matrix33& q, bool inverse = false) {
	hkRotation m3;
	m3.setCols(TOVECTOR3(q.rows[0]), TOVECTOR3(q.rows[1]), TOVECTOR3(q.rows[2]));
	if (inverse) m3.invert(0.001);
	return m3;
}

static inline Niflib::hkQuaternion TOQUAT(const Niflib::Quaternion& q) {
	Niflib::hkQuaternion v;
	v.x = q.x;
	v.y = q.y;
	v.z = q.z;
	v.w = q.w;
	return v;
}



size_t first_number(const std::string& c, size_t off = 0)
{
	return c.find_first_of("0123456789", off);
}

size_t find_alpha(const std::string& c, size_t off = 0)
{
	return c.find_first_of("abcdefghijklmnopqrstuvwz", off);
}

float uv_scale(float value, float min, float max) {
	return (value * (max - min)) + min;
}

struct LeafCardData
{
	NiTriShapeDataRef data;
	BSLightingShaderPropertyRef shader;
	NiAlphaPropertyRef alpha;
};

struct MinMaxer {

	std::array<float, 3> bb_min = { numeric_limits<float>::max() };
	std::array<float, 3> bb_max = { numeric_limits<float>::min() };

	Vector3 extent() {
		return
			Vector3({ bb_max[0],bb_max[1],bb_max[2] }) -
			Vector3({ bb_min[0],bb_min[1],bb_min[2] });
	}

	void check(const vector<Vector3>& vertices) {
		for (const auto& vertex : vertices) {
			if (vertex[0] > bb_max[0]) bb_max[0] = vertex[0];
			if (vertex[1] > bb_max[1]) bb_max[1] = vertex[1];
			if (vertex[2] > bb_max[2]) bb_max[2] = vertex[2];
			if (vertex[0] < bb_min[0]) bb_min[0] = vertex[0];
			if (vertex[1] < bb_min[1]) bb_min[1] = vertex[1];
			if (vertex[2] < bb_min[2]) bb_min[2] = vertex[2];
		}
	}
};

struct DistanceOrderer {

	size_t max_index = 0;
	size_t min_index = 0;

	void check(const vector<Vector3>& vertices) {

		for (size_t i = 0; i < vertices.size(); i++) {
			const auto& vertex = vertices[i];
			if (vertex.Magnitude() > vertices[max_index].Magnitude())
				max_index = i;
			if (vertex.Magnitude() < vertices[min_index].Magnitude())
				min_index = i;
		}
	}
};

void OutputBillboardGeometry
(
	CSpeedTreeRT& speedTree, 
	CSpeedTreeRT::SGeometry& sGeom,
	MinMaxer& bb,
	const fs::path& export_path, 
	const fs::path file_in, 
	const map<string, string>& metadata,
	int lod_level
)
{
	fs::path file_out = export_path / (file_in.filename().replace_extension("").string() + string("_lod_") + to_string(lod_level) + ".nif");

	auto billboard = sGeom.m_sBillboard0;

	BSFadeNodeRef root = new BSFadeNode();
	auto root_children = root->GetChildren();

	string billboard_name = file_in.filename().string();
	NiTriShapeRef shape = new NiTriShape();
	NiTriShapeDataRef data = new NiTriShapeData();
	BSLightingShaderPropertyRef shader = new BSLightingShaderProperty();
	NiAlphaPropertyRef alpha = new NiAlphaProperty();

	float alphaTest = billboard.m_fAlphaTestValue;
	if (alphaTest >= 255)
		alphaTest = 84.;
	alpha_flags flags;
	flags.bits.alpha_test_enable = 1;
	flags.bits.alpha_test_mode = alpha_flags::GL_GREATER;
	alpha->SetFlags(flags.value);
	alpha->SetThreshold(alphaTest);

	BSShaderTextureSetRef textures = new BSShaderTextureSet();
	std::string texpath = "textures\\tes4\\trees\\billboards\\";
	vector<string> texture_names = {
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	};
	texture_names[0] = texpath + fs::path(billboard_name).replace_extension(".dds").string();
	textures->SetTextures(texture_names);
	shader->SetTextureSet(textures);
	shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(shader->GetShaderFlags1_sk() & ~SLSF1_REMAPPABLE_TEXTURES));
	shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(shader->GetShaderFlags2_sk() | SLSF2_DOUBLE_SIDED));
	shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(shader->GetShaderFlags2_sk() & ~SLSF2_VERTEX_COLORS));
	shape->SetShaderProperty(StaticCast<BSShaderProperty>(shader));
	shape->SetAlphaProperty(alpha);
	shape->SetData(StaticCast<NiGeometryData>(data));

	data->SetHasVertices(true);
	auto vertices = data->GetVertices();
	auto normals = data->GetNormals();
	auto tangents = data->GetTangents();
	auto bitangents = data->GetBitangents();
	data->SetHasNormals(true);
	data->SetHasUv(true);
	data->SetBsVectorFlags((BSVectorFlags)(data->GetBsVectorFlags() | BSVF_HAS_UV | BSVF_HAS_TANGENTS));
	shape->SetFlags(524302);
	auto uvs = vector<TexCoord>();
	data->SetHasTriangles(true);
	auto triangles = data->GetTriangles();
	data->SetConsistencyFlags(CT_STATIC);
	data->SetHasVertexColors(false);

	//const float* pCoords = billboard.m_pCoords;
	//const float* pTex = billboard.m_pTexCoords;

	float x = (bb.bb_max[2] - bb.bb_min[2]) / 2.;

	hkVector4 v1 = { -x, 0., bb.bb_min[2] };
	hkVector4 v2 = { x, 0., bb.bb_min[2] };
	hkVector4 v3 = { -x, 0., bb.bb_max[2] };
	hkVector4 v4 = { x, 0., bb.bb_max[2] };

	hkVector4 n1 = { -x, 0, bb.bb_max[2] }; n1.normalize3();
	hkVector4 n2 = { x, 0, bb.bb_max[2] }; n2.normalize3();

	hkRotation z_rot; z_rot.setAxisAngle({ 0.,0.,1. }, PI / 4);
	hkTransform z_trans; z_trans.setIdentity(); z_trans.setRotation(z_rot); //z_trans.setTranslation(delta);
	v1.setTransformedPos(z_trans, v1);
	v2.setTransformedPos(z_trans, v2);
	v3.setTransformedPos(z_trans, v3);
	v4.setTransformedPos(z_trans, v4);

	n1.setTransformedPos(z_trans, n1); n1.normalize3();
	n2.setTransformedPos(z_trans, n2); n2.normalize3();

	vertices.push_back({ v1.getSimdAt(0), v1.getSimdAt(1), v1.getSimdAt(2) });
	vertices.push_back({ v2.getSimdAt(0), v2.getSimdAt(1), v2.getSimdAt(2) });
	vertices.push_back({ v3.getSimdAt(0), v3.getSimdAt(1), v3.getSimdAt(2) });
	vertices.push_back({ v4.getSimdAt(0), v4.getSimdAt(1), v4.getSimdAt(2) });

	triangles.push_back({ (unsigned short)(vertices.size() - 4),  (unsigned short)(vertices.size() - 3),  (unsigned short)(vertices.size() - 2) });
	triangles.push_back({ (unsigned short)(vertices.size() - 3),  (unsigned short)(vertices.size() - 2),  (unsigned short)(vertices.size() - 1) });

	normals.push_back({ n1.getSimdAt(0), n1.getSimdAt(1), n1.getSimdAt(2) });
	normals.push_back({ n2.getSimdAt(0), n2.getSimdAt(1), n2.getSimdAt(2) });
	normals.push_back({ n1.getSimdAt(0), n1.getSimdAt(1), n1.getSimdAt(2) });
	normals.push_back({ n2.getSimdAt(0), n2.getSimdAt(1), n2.getSimdAt(2) });

	uvs.push_back({ 1.,  1. });
	uvs.push_back({ 0.,  1. });
	uvs.push_back({ 1.,  0. });
	uvs.push_back({ 0.,  0. });

	//hkVector4 delta({ 10., 0., 0. });
	z_rot; z_rot.setAxisAngle({ 0.,0.,1. }, PI / 2);
	z_trans; z_trans.setIdentity(); z_trans.setRotation(z_rot); //z_trans.setTranslation(delta);
	v1.setTransformedPos(z_trans, v1);
	v2.setTransformedPos(z_trans, v2);
	v3.setTransformedPos(z_trans, v3);
	v4.setTransformedPos(z_trans, v4);

	n1.setTransformedPos(z_trans, n1); n1.normalize3();
	n2.setTransformedPos(z_trans, n2); n2.normalize3();

	vertices.push_back({ v1.getSimdAt(0), v1.getSimdAt(1), v1.getSimdAt(2) });
	vertices.push_back({ v2.getSimdAt(0), v2.getSimdAt(1), v2.getSimdAt(2) });
	vertices.push_back({ v3.getSimdAt(0), v3.getSimdAt(1), v3.getSimdAt(2) });
	vertices.push_back({ v4.getSimdAt(0), v4.getSimdAt(1), v4.getSimdAt(2) });

	triangles.push_back({ (unsigned short)(vertices.size() - 4),  (unsigned short)(vertices.size() - 3),  (unsigned short)(vertices.size() - 2) });
	triangles.push_back({ (unsigned short)(vertices.size() - 3),  (unsigned short)(vertices.size() - 2),  (unsigned short)(vertices.size() - 1) });

	normals.push_back({ n1.getSimdAt(0), n1.getSimdAt(1), n1.getSimdAt(2) });
	normals.push_back({ n2.getSimdAt(0), n2.getSimdAt(1), n2.getSimdAt(2) });
	normals.push_back({ n1.getSimdAt(0), n1.getSimdAt(1), n1.getSimdAt(2) });
	normals.push_back({ n2.getSimdAt(0), n2.getSimdAt(1), n2.getSimdAt(2) });

	uvs.push_back({ 0.,  1. });
	uvs.push_back({ 1.,  1. });
	uvs.push_back({ 0.,  0. });
	uvs.push_back({ 1.,  0. });

	Vector3 COM;
	TriGeometryContext g(vertices, COM, triangles, uvs, normals);

	data->SetVertices(vertices);
	data->SetTriangles(triangles);
	data->SetNumTriangles(triangles.size());
	data->SetNumTrianglePoints(triangles.size() * 3);
	data->SetNormals(normals);
	data->SetTangents(g.tangents);
	data->SetBitangents(g.bitangents);
	data->SetUvSets({ uvs });

	root_children.push_back(StaticCast<NiAVObject>(shape));
	root->SetChildren(root_children);

	NifInfo info;
	info.userVersion = 12;
	info.userVersion2 = 83;
	info.version = Niflib::VER_20_2_0_7;

	WriteNifTree(file_out.string(), root, info);

}


hkRotation skew(const hkVector4& vv) {
	hkVector4 v = vv; v.normalize3();
	hkRotation out;
	out.setRows(
		{ 0., -v(2), v(1) },
		{ v(2), 0., -v(0) },
		{ -v(1), v(0), 0 }
	);
	return out;
}

hkRotation rotationBetweenVectors(const hkVector4& v1, const hkVector4& v2) {
	hkVector4 vv1 = v1; vv1.normalize3();
	hkVector4 vv2 = v2; vv2.normalize3();
	hkVector4 v; v.setCross(vv1, vv2);
	auto s = v.distanceToSquared3({ 0.,0.,0. });
	auto c = vv1.dot3(vv2);
	hkRotation r; r.setIdentity();
	hkRotation skewM; skewM.setCrossSkewSymmetric(v);
	hkRotation skewMM; skewMM.setMul(skewM, skewM);
	auto ratio = 1. / (1. - (float)c);
	skewMM.setMul(ratio, skewMM);
	r.add(skewM); r.add(skewMM);
	//r.renormalize();
	return r;
}

class TreeSkin {};

template<>
class Accessor<TreeSkin>
{

	void doLeaves(NiTriShapeRef shape,
		map<NiTriShapeRef, NiNodeRef> shapes_map, //clusters
		multimap<NiNodeRef, Triangle> triangle_bone_map, //vertices to clusters
		map< NiNodeRef, pair<Matrix33, Vector3>> bone_transform_map, //bone transforms;
		NiNodeRef root)
	{
		NiSkinInstanceRef skin = new NiSkinInstance();
		NiSkinDataRef data = new NiSkinData();
		NiSkinPartitionRef skin_partition = new NiSkinPartition();

		size_t num_partitions = shapes_map.size() / 60;
		if (shapes_map.size() % 60 > 0)
			num_partitions++;
		vector<SkinPartition> partitions(num_partitions);

		size_t bone_offset;
		for (int p = 0; p < num_partitions; p++)
		{
			SkinPartition& partition = partitions[p];
			bone_offset = 60 * p;
			auto bone_limit = shapes_map.size() - bone_offset;
			if (bone_limit > 60)
				bone_limit = 60;

			skin->skeletonRoot = root;
			//for (const auto& bone : shapes_map) {
			for (int bone_index = bone_offset; bone_index < bone_offset + bone_limit; bone_index++) {
				auto bone = shapes_map.begin();
				for (int k = 0; k < bone_index; k++)
					bone++;

				skin->bones.push_back(bone->second);

				BoneData bone_data;
				auto transform = bone_transform_map[bone->second];
				bone_data.skinTransform.rotation = transform.first.Transpose();
				bone_data.skinTransform.translation -= transform.first * transform.second;
				data->boneList.push_back(bone_data);
			}



			auto shape_data = DynamicCast<NiTriShapeData>(shape->data);
			//partition.numVertices = shape_data->vertices.size();
			//partition.numTriangles = shape_data->triangles.size();
			//partition.numBones = bone_limit;
			partition.numStrips = 0;
			partition.numWeightsPerVertex = 4;
			//for (int i = 0; i < bone_limit; i++)
			//	partition.bones.push_back(bone_offset + i);
			partition.hasVertexMap = true;
			partition.hasVertexWeights = true;
			partition.hasBoneIndices = true;

			for (int bone_index = 0; bone_index < bone_limit; bone_index++) {
				auto bone = shapes_map.begin();
				for (int k = 0; k < bone_index + bone_offset; k++)
					bone++;

				auto triangles = triangle_bone_map.equal_range(bone->second);
				int local_bone_index = 0;
				if (triangles.first != triangles.second)
				{
					partition.numBones++;
					local_bone_index = partition.bones.size();
					partition.bones.push_back(bone_index+bone_offset);
				}

				for (auto triangle_it = triangles.first; triangle_it != triangles.second; triangle_it++) {

					partition.triangles.resize(partition.triangles.size() + 1);
					for (size_t t = 0; t < 3; t++) {
						auto index_it = find(partition.vertexMap.begin(), partition.vertexMap.end(), triangle_it->second[t]);
						if (index_it == partition.vertexMap.end()) {
							partition.vertexMap.push_back(triangle_it->second[t]);
							index_it = partition.vertexMap.end() - 1;
						}
						size_t vertex_index = distance(partition.vertexMap.begin(), index_it);
						partition.triangles[partition.triangles.size() - 1][t] = vertex_index;
						if (partition.boneIndices.size() <= vertex_index)
							partition.boneIndices.resize(vertex_index + 1);
						if (partition.vertexWeights.size() <= vertex_index)
							partition.vertexWeights.resize(vertex_index + 1);
						auto& indices = partition.boneIndices[vertex_index];
						if (find(indices.begin(), indices.end(), local_bone_index) == indices.end())
						{
							indices.push_back((unsigned char)(local_bone_index));
							auto& weights = partition.vertexWeights[vertex_index];
							weights.push_back(1.);
						}
					}
				}
			}

			partition.numVertices = partition.vertexMap.size();
			partition.numTriangles = partition.triangles.size();
			partition.hasFaces = true;
			partition.unknownShort = 1; //????

			for (int i = 0; i < partition.numVertices; i++)
			{
				partition.boneIndices[i].resize(4);
				partition.vertexWeights[i].resize(4);
			}
			//renormalize and reorder		
			//for (auto& partition : partitions) {
			for (int i = 0; i < partition.vertexWeights.size(); i++)
			{
				float sum = 0.0;
				for (int k = 0; k < 4; k++)
				{
					sum += partition.vertexWeights[i][k];
				}
				for (int k = 0; k < 4; k++)
				{
					partition.vertexWeights[i][k] = partition.vertexWeights[i][k] / sum;
				}
				auto perm = sort_permutation(partition.vertexWeights[i],
					[](float const& a, float const& b) { return a > b; });

				partition.vertexWeights[i] = apply_permutation(partition.vertexWeights[i], perm);
				partition.boneIndices[i] = apply_permutation(partition.boneIndices[i], perm);
			}
		}

		skin_partition->SetSkinPartitionBlocks(partitions);
		skin_partition->numSkinPartitionBlocks = partitions.size();
		data->hasVertexWeights = false;
		skin->SetData(data);
		skin->SetSkinPartition(skin_partition);
		shape->SetSkinInstance(skin);
	}

	void doBranches(NiTriShapeRef shape,
		map<NiTriShapeRef, NiNodeRef> shapes_map, //clusters
		multimap<NiTriShapeRef, Triangle> triangle_bone_map, //vertices to clusters
		map< NiNodeRef, pair<Matrix33, Vector3>> bone_transform_map, //bone transforms;
		NiNodeRef root)
	{
		NiSkinInstanceRef skin = new NiSkinInstance();
		NiSkinDataRef data = new NiSkinData();
		NiSkinPartitionRef skin_partition = new NiSkinPartition();

		size_t num_partitions = shapes_map.size() / 60;
		if (shapes_map.size() % 60 > 0)
			num_partitions++;
		vector<SkinPartition> partitions(num_partitions);

		size_t bone_offset;
		for (int p = 0; p < num_partitions; p++)
		{
			SkinPartition& partition = partitions[p];
			bone_offset = 60 * p;
			auto bone_limit = shapes_map.size() - bone_offset;
			if (bone_limit > 60)
				bone_limit = 60;

			skin->skeletonRoot = root;
			//for (const auto& bone : shapes_map) {
			for (int bone_index = bone_offset; bone_index < bone_offset + bone_limit; bone_index++) {
				auto bone = shapes_map.begin();
				for (int k = 0; k< bone_index; k++)
					bone++;

				skin->bones.push_back(bone->second);

				BoneData bone_data;
				auto transform = bone_transform_map[bone->second];
				bone_data.skinTransform.rotation = transform.first.Transpose();
				bone_data.skinTransform.translation -= transform.first * transform.second;
				data->boneList.push_back(bone_data);
			}



			auto shape_data = DynamicCast<NiTriShapeData>(shape->data);
			//partition.numVertices = shape_data->vertices.size();
			//partition.numTriangles = shape_data->triangles.size();
			partition.numBones = bone_limit;
			partition.numStrips = 0;
			partition.numWeightsPerVertex = 4;
			for (int i = 0; i < bone_limit; i++)
				partition.bones.push_back(bone_offset + i);
			partition.hasVertexMap = true;
			partition.hasVertexWeights = true;
			partition.hasBoneIndices = true;

			for (int bone_index = 0; bone_index < bone_limit; bone_index++) {
				auto bone = shapes_map.begin();
				for (int k = 0; k < bone_index + bone_offset; k++)
					bone++;

				auto triangles = triangle_bone_map.equal_range(bone->first);
				for (auto triangle_it = triangles.first; triangle_it != triangles.second; triangle_it++) {
					partition.triangles.resize(partition.triangles.size() + 1);
					for (size_t t = 0; t < 3; t++) {
						auto index_it = find(partition.vertexMap.begin(), partition.vertexMap.end(), triangle_it->second[t]);
						if (index_it == partition.vertexMap.end()) {
							partition.vertexMap.push_back(triangle_it->second[t]);
							index_it = partition.vertexMap.end() - 1;
						}
						size_t vertex_index = distance(partition.vertexMap.begin(), index_it);
						partition.triangles[partition.triangles.size()-1][t] = vertex_index;
						if (partition.boneIndices.size() <= vertex_index)
							partition.boneIndices.resize(vertex_index + 1);
						if (partition.vertexWeights.size() <= vertex_index)
							partition.vertexWeights.resize(vertex_index + 1);
						auto& indices = partition.boneIndices[vertex_index];
						if (find(indices.begin(), indices.end(), bone_index) == indices.end())
						{
							indices.push_back((unsigned char)(bone_index));
							auto& weights = partition.vertexWeights[vertex_index];
							weights.push_back(1.);
						}
					}
				}
			}

			partition.numVertices = partition.vertexMap.size();
			partition.numTriangles = partition.triangles.size();
			partition.hasFaces = true;
			partition.unknownShort = 1; //????

			for (int i = 0; i < partition.numVertices; i++)
			{
				partition.boneIndices[i].resize(4);
				partition.vertexWeights[i].resize(4);
			}
			//renormalize and reorder		
			//for (auto& partition : partitions) {
			for (int i = 0; i < partition.vertexWeights.size(); i++)
			{
				float sum = 0.0;
				for (int k = 0; k < 4; k++)
				{
					sum += partition.vertexWeights[i][k];
				}
				for (int k = 0; k < 4; k++)
				{
					partition.vertexWeights[i][k] = partition.vertexWeights[i][k] / sum;
				}
				auto perm = sort_permutation(partition.vertexWeights[i],
					[](float const& a, float const& b) { return a > b; });

				partition.vertexWeights[i] = apply_permutation(partition.vertexWeights[i], perm);
				partition.boneIndices[i] = apply_permutation(partition.boneIndices[i], perm);
			}

			//Check 0 weighted
			for (int i = 0; i < partition.vertexWeights.size(); i++)
			{
				float sum = 0.0;
				for (int k = 0; k < 4; k++)
				{
					sum += partition.vertexWeights[i][k];
				}
				if (sum < 1.)
					__debugbreak();
			}
		}

		skin_partition->SetSkinPartitionBlocks(partitions);
		skin_partition->numSkinPartitionBlocks = partitions.size();
		data->hasVertexWeights = false;
		skin->SetData(data);
		skin->SetSkinPartition(skin_partition);
		shape->SetSkinInstance(skin);
	}

public:
	Accessor(
		NiTriShapeRef shape, 
		map<NiTriShapeRef, NiNodeRef> shapes_map, //clusters
		NiTriShapeRef leafcards, //leaves
		multimap<NiTriShapeRef, Triangle> triangle_bone_map, //triangles to clusters
		multimap<NiNodeRef, Triangle> leaf_triangle_bone_map, //triangles to clusters
		map< NiNodeRef, pair<Matrix33, Vector3>> bone_transform_map, //bone transforms;
		NiNodeRef root) {
		if (shapes_map.size() > 0) {
			doBranches(shape, shapes_map, triangle_bone_map, bone_transform_map, root);
			doLeaves(leafcards, shapes_map, leaf_triangle_bone_map, bone_transform_map, root);
		}

	}
};


union color_helper
{

	struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	} rgba;
	unsigned int value;
};

void OutputTreeGeometry
(
	CSpeedTreeRT& speedTree,
	CSpeedTreeRT::SGeometry& sGeom,
	MinMaxer& bb,
	const fs::path& export_path,
	const fs::path file_in,
	const map<string, string>& metadata,
	int lod_level
)
{
	fs::path file_out = export_path / file_in.filename().replace_extension(".nif");
	if (lod_level > 0) {
		file_out = export_path / (file_in.filename().replace_extension("").string() + "_lod_0.nif");
	}

	const Games::GamesPathMapT& installations = games.getGames();
	CSpeedTreeRT::STextures sTextures;
	speedTree.GetTextures(sTextures);

	const SEmbeddedTexCoords* pSEmbeddedTexCoords = speedTree.m_pEmbeddedTexCoords;

	char branchTextureName[MAX_PATH];
	_splitpath(sTextures.m_pBranchTextureFilename, NULL, NULL, branchTextureName, NULL);

	BSTreeNodeRef root = new BSTreeNode();
	auto root_children = root->GetChildren();

	//Trunk bone, for any reason, must be rotated by 90, 0, 90
	hkRotation roll_rot; roll_rot.setAxisAngle({ 0., 0., 1. }, -PI / 2);
	hkRotation yaw_rot; yaw_rot.setAxisAngle({ 1., 0., 0. }, -PI / 2);
	hkRotation root_rotation; root_rotation.setIdentity();
	root_rotation.mul(roll_rot);
	root_rotation.mul(yaw_rot);
	hkTransform root_transform; root_transform.setRotation(root_rotation); root_transform.setTranslation({ 0.,0.,0. });

	vector<NiTriShapeRef> shapes;
	NiTriShapeRef root_shape;

	NiTriShapeRef full_shape = new NiTriShape();
	NiTriShapeRef full_shape_lod_0 = new NiTriShape();
	NiTriShapeRef full_shape_lod_1 = new NiTriShape();
	NiTriShapeDataRef data = new NiTriShapeData();
	NiTriShapeDataRef data_lod_0 = new NiTriShapeData();
	NiTriShapeDataRef data_lod_1 = new NiTriShapeData();
	BSLightingShaderPropertyRef shader = new BSLightingShaderProperty();
	BSLightingShaderPropertyRef shader_lod_0 = new BSLightingShaderProperty();
	BSLightingShaderPropertyRef shader_lod_1 = new BSLightingShaderProperty();
	BSShaderTextureSetRef textures = new BSShaderTextureSet();
	BSShaderTextureSetRef textures_lod_0 = new BSShaderTextureSet();
	BSShaderTextureSetRef textures_lod_1 = new BSShaderTextureSet();

	std::string texpath = "textures\\tes4\\trees\\branches\\";
	vector<string> texture_names = {
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	};
	texture_names[0] = texpath + string(branchTextureName) + string(".dds");
	texture_names[1] = texpath + string(branchTextureName) + string("_n.dds");
	textures->SetTextures(texture_names);
	textures_lod_0->SetTextures(texture_names);
	textures_lod_1->SetTextures(texture_names);


	shader->SetTextureSet(textures);
	shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(shader->GetShaderFlags1_sk() | SLSF1_SKINNED));
	shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(shader->GetShaderFlags1_sk() & ~SLSF1_SPECULAR));
	shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(shader->GetShaderFlags2_sk() & ~SLSF2_VERTEX_COLORS));
	shader_lod_0->SetTextureSet(textures_lod_0);
	shader_lod_0->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(shader->GetShaderFlags2_sk() & ~SLSF2_VERTEX_COLORS));
	shader_lod_1->SetTextureSet(textures_lod_1);
	shader_lod_1->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(shader->GetShaderFlags2_sk() & ~SLSF2_VERTEX_COLORS));
	full_shape->SetShaderProperty(StaticCast<BSShaderProperty>(shader));
	full_shape->SetData(StaticCast<NiGeometryData>(data));
	full_shape_lod_0->SetShaderProperty(StaticCast<BSShaderProperty>(shader_lod_0));
	full_shape_lod_0->SetData(StaticCast<NiGeometryData>(data_lod_0));
	full_shape_lod_1->SetShaderProperty(StaticCast<BSShaderProperty>(shader_lod_1));
	full_shape_lod_1->SetData(StaticCast<NiGeometryData>(data_lod_1));

	data->SetHasVertices(true);
	data->SetHasNormals(true);
	data->SetHasUv(true);
	data->SetBsVectorFlags((BSVectorFlags)(data->GetBsVectorFlags() | BSVF_HAS_UV | BSVF_HAS_TANGENTS));
	data->SetHasTriangles(true);
	data->SetConsistencyFlags(CT_STATIC);
	data->SetHasVertexColors(false);
	full_shape->SetFlags(524302);

	data_lod_0->SetHasVertices(true);
	data_lod_0->SetHasNormals(true);
	data_lod_0->SetHasUv(true);
	data_lod_0->SetBsVectorFlags((BSVectorFlags)(data->GetBsVectorFlags() | BSVF_HAS_UV | BSVF_HAS_TANGENTS));
	data_lod_0->SetHasTriangles(true);
	data_lod_0->SetConsistencyFlags(CT_STATIC);
	data_lod_0->SetHasVertexColors(false);
	full_shape_lod_0->SetFlags(524302);

	data_lod_1->SetHasVertices(true);
	data_lod_1->SetHasNormals(true);
	data_lod_1->SetHasUv(true);
	data_lod_1->SetBsVectorFlags((BSVectorFlags)(data->GetBsVectorFlags() | BSVF_HAS_UV | BSVF_HAS_TANGENTS));
	data_lod_1->SetHasTriangles(true);
	data_lod_1->SetConsistencyFlags(CT_STATIC);
	data_lod_1->SetHasVertexColors(false);
	full_shape_lod_1->SetFlags(524302);

	auto vertices = data->GetVertices();
	auto normals = data->GetNormals();
	auto tangents = data->GetTangents();
	auto bitangents = data->GetBitangents();

	auto uvs = vector<TexCoord>();
	auto triangles = data->GetTriangles();

	vertices.resize(sGeom.m_sBranches.m_usVertexCount);
	uvs.resize(sGeom.m_sBranches.m_usVertexCount);

	for (size_t i = 0; i < sGeom.m_sBranches.m_usVertexCount; i++)
	{
		vertices[i] = {
			sGeom.m_sBranches.m_pCoords[3 * i],
			sGeom.m_sBranches.m_pCoords[3 * i + 1],
			sGeom.m_sBranches.m_pCoords[3 * i + 2]
		};

		uvs[i] = {
			sGeom.m_sBranches.m_pTexCoords0[2 * i],
			fmodf(sGeom.m_sBranches.m_pTexCoords0[2 * i + 1],2.f) -1.f,
		};
	}

	multimap<NiTriShapeRef, Triangle> triangle_bone_map;
	multimap<NiNodeRef, Triangle> leaf_triangle_bone_map;

	bhkCollisionObjectRef collision = new bhkCollisionObject();
	collision->SetFlags((bhkCOFlags)(BHKCO_ACTIVE | BHKCO_SYNC_ON_UPDATE));
	bhkRigidBodyRef body = new bhkRigidBody();
	auto layer = body->GetHavokFilter();
	layer.flagsAndPartNumber = 0;
	layer.group = 0;
	layer.layer_sk = SKYL_TREES;
	body->SetHavokFilter(layer);
	body->SetBroadPhaseType(BROAD_PHASE_ENTITY);
	auto cinfo = body->GetCinfoProperty();
	cinfo.data = 0;
	cinfo.size = 0;
	cinfo.capacityAndFlags = 2147483648;
	body->SetCinfoProperty(cinfo);
	body->SetCollisionResponse(RESPONSE_SIMPLE_CONTACT);
	body->SetProcessContactCallbackDelay(65535);
	body->SetHavokFilterCopy(layer);
	body->SetCollisionResponse2(RESPONSE_SIMPLE_CONTACT);
	body->SetProcessContactCallbackDelay2(65535);

	//trans/rot
	body->SetMass(0.);
	body->SetLinearDamping(0.099609);
	body->SetAngularDamping(0.049805);
	body->SetTimeFactor(1.);
	body->SetGravityFactor(1.);
	body->SetFriction(0.3);
	body->SetRollingFrictionMultiplier(0.);
	body->SetRestitution(0.8);
	body->SetMaxLinearVelocity(104.400002);
	body->SetMaxAngularVelocity(31.57);
	body->SetPenetrationDepth(0.15);
	body->SetMotionSystem(MO_SYS_BOX_STABILIZED);
	body->SetEnableDeactivation(true);
	body->SetSolverDeactivation(SOLVER_DEACTIVATION_OFF);
	body->SetQualityType(MO_QUAL_INVALID);

	collision->SetBody(StaticCast<bhkWorldObject>(body));


	//Tree Branches, create Hierarchy
	for (int nStrip = 0; nStrip < sGeom.m_sBranches.m_usNumStrips; nStrip++)
	{
		DistanceOrderer branch_orderer;
		NiTriShapeDataRef part_data = new NiTriShapeData();
		NiTriShapeRef branch = new NiTriShape();
		branch->SetData(StaticCast<NiGeometryData>(part_data));

		auto part_vertices = part_data->GetVertices();
		auto part_uvs = vector<TexCoord>();
		auto part_triangles = data->GetTriangles();
		auto part_normals = data->GetNormals();
		auto part_tangents = data->GetTangents();
		auto part_bitangents = data->GetBitangents();

		hkpCreateShapeUtility::CreateShapeInput input;

		int nStripLength = sGeom.m_sBranches.m_pStripLengths[nStrip];
		const unsigned short* pStripIndices = sGeom.m_sBranches.m_pStrips[nStrip];
		for (int i = 0; i < nStripLength - 2; i++)
		{
			int nIndices[3] = { pStripIndices[i], pStripIndices[i + 1], pStripIndices[i + 2] };

			if (i % 2)
			{
				int tmp = nIndices[2];
				nIndices[2] = nIndices[1];
				nIndices[1] = tmp;
			}


			for (int j = 0; j < 3; j++)
			{
				const float* pPos = &sGeom.m_sBranches.m_pCoords[nIndices[j] * 3];
				const float* pNormal = &sGeom.m_sBranches.m_pNormals[nIndices[j] * 3];
				const float* pTangents = &sGeom.m_sBranches.m_pTangents[nIndices[j] * 3];
				const float* pBinormals = &sGeom.m_sBranches.m_pBinormals[nIndices[j] * 3];
				const float* pTexCoord = &sGeom.m_sBranches.m_pTexCoords0[nIndices[j] * 2];


				part_vertices.push_back({ pPos[0], pPos[1], pPos[2] });
				if (i < 20)
					input.m_vertices.pushBack({ pPos[0], pPos[1], pPos[2] });
				part_normals.push_back({ pNormal[0],  pNormal[1], pNormal[2] });
				part_tangents.push_back({ pTangents[0],  pTangents[1], pTangents[2] });
				part_bitangents.push_back({ pBinormals[0], pBinormals[1], pBinormals[2] });
				
			}

			triangle_bone_map.insert({ branch, { (unsigned short)(nIndices[0]), (unsigned short)(nIndices[1]), (unsigned short)(nIndices[2]) } });
			triangles.push_back({ (unsigned short)(nIndices[0]), (unsigned short)(nIndices[1]), (unsigned short)(nIndices[2]) });
			part_triangles.push_back({ (unsigned short)(part_vertices.size() - 3),  (unsigned short)(part_vertices.size() - 2),  (unsigned short)(part_vertices.size() - 1) });
		}

		branch_orderer.check(part_vertices);
		bb.check(part_vertices);
		input.m_vertices.pushBack({ 0., 0., bb.bb_max[2] });
		part_data->SetVertices(part_vertices);
		part_data->SetTriangles(part_triangles);
		part_data->SetNumTriangles(part_triangles.size());
		part_data->SetNumTrianglePoints(part_triangles.size() * 3);
		part_data->SetNormals(part_normals);
		part_data->SetTangents(part_tangents);
		part_data->SetBitangents(part_bitangents);

		hkpCreateShapeUtility::ShapeInfoOutput output;
		input.m_enableAutomaticShapeShrinking = false;
		hkpCreateShapeUtility::createCapsuleShape(input, output);
		hkpCapsuleShape* shape = (hkpCapsuleShape*)output.m_shape;

		if (nStrip == 0) {
			root_shape = branch;

			bhkCapsuleShapeRef capsule = new bhkCapsuleShape();
			capsule->SetFirstPoint(TOVECTOR3(shape->getVertices()[1]) * COLLISION_RATIO);
			capsule->SetSecondPoint(TOVECTOR3(shape->getVertices()[0]) * COLLISION_RATIO);
			capsule->SetRadius(shape->getRadius() * COLLISION_RATIO);
			capsule->SetRadius1(shape->getRadius() * COLLISION_RATIO);
			capsule->SetRadius2(shape->getRadius() * COLLISION_RATIO);
			HavokMaterial temp; temp.material_sk = SKY_HAV_MAT_HEAVY_WOOD;
			capsule->SetMaterial(temp);
			body->SetShape(StaticCast<bhkShape>(capsule));
		}

		shapes.push_back(branch);

		//trunk_children.push_back(StaticCast<NiAVObject>(branch));
	}

	{
		data->SetVertices(vertices);
		data->SetTriangles(triangles);
		data->SetNumTriangles(triangles.size());
		data->SetNumTrianglePoints(triangles.size() * 3);
		Vector3 COM;
		TriGeometryContext branch(vertices, COM, triangles, uvs, normals);
		data->SetNormals(branch.normals);
		data->SetTangents(branch.tangents);
		data->SetBitangents(branch.bitangents);
		data->SetUvSets({ uvs });

		data_lod_0->SetVertices(vertices);
		data_lod_0->SetTriangles(triangles);
		data_lod_0->SetNumTriangles(triangles.size());
		data_lod_0->SetNumTrianglePoints(triangles.size() * 3);
		data_lod_0->SetNormals(branch.normals);
		data_lod_0->SetTangents(branch.tangents);
		data_lod_0->SetBitangents(branch.bitangents);
		data_lod_0->SetUvSets({ uvs });

		data_lod_1->SetVertices(vertices);
		data_lod_1->SetTriangles(triangles);
		data_lod_1->SetNumTriangles(triangles.size());
		data_lod_1->SetNumTrianglePoints(triangles.size() * 3);
		data_lod_1->SetNormals(branch.normals);
		data_lod_1->SetTangents(branch.tangents);
		data_lod_1->SetBitangents(branch.bitangents);
		data_lod_1->SetUvSets({ uvs });
		//root_children.push_back(StaticCast<NiAVObject>(full_shape));
	}

	map<NiTriShapeRef, tuple<NiTriShapeRef, Vector3, Vector3>> parents;

	for (const auto& shape : shapes) {
		multimap<size_t, tuple<NiTriShapeRef, vector<Vector3>, vector<Vector3>>> collisions;
		auto shape_vertices = shape->GetData()->GetVertices();
		auto shape_faces = DynamicCast<NiTriShapeData>(shape->GetData())->GetTriangles();
		if (shape == root_shape) {
			parents[shape] = { NULL, {0.,0.,0.},{0.,0.,1.} };
			continue;
		}
		vector<Vector3> collision_points;
		vector<Vector3> collision_normals;
		for (const auto& another_shape : shapes) {
			if (another_shape == shape) continue;
			auto another_shape_vertices = another_shape->GetData()->GetVertices();
			auto another_shape_faces = DynamicCast<NiTriShapeData>(another_shape->GetData())->GetTriangles();
			size_t distance = 0;
			//try first 20 tris
			size_t limit = shape_faces.size() > 10 ? 10 : shape_faces.size();
			for (int t = 0; t < limit; t++) {
				const auto& face = shape_faces[t];
			//for (const auto& face : shape_faces) {

				auto& v1 = shape_vertices[face.v1];
				auto& v2 = shape_vertices[face.v2];
				auto& v3 = shape_vertices[face.v3];
				for (const auto& another_face : another_shape_faces) {
					auto& a_v1 = another_shape_vertices[another_face.v1];
					auto& a_v2 = another_shape_vertices[another_face.v2];
					auto& a_v3 = another_shape_vertices[another_face.v3];
					Vector3 p1, p2;
					bool coplanar;
					if (moeller::TriangleIntersects<Vector3>::triangle(v1, v2, v3, a_v1, a_v2, a_v3, p1, p2, coplanar))
					{
						distance++;
						collision_points.push_back(v1);
						collision_points.push_back(v2);
						collision_points.push_back(v3);
						collision_points.push_back(a_v1);
						collision_points.push_back(a_v2);
						collision_points.push_back(a_v2);
						auto n1 = (v2-v1).CrossProduct(v3-v2).Normalized();
						auto n2 = (v3-v2).CrossProduct(v1-v3).Normalized();
						auto n3 = (v1-v3).CrossProduct(v2-v1).Normalized();
						auto angle1 = n1.Normalized().DotProduct(v1.Normalized());
						auto angle2 = n2.Normalized().DotProduct(v2.Normalized());
						if (angle1 < 1)
							collision_normals.push_back(n1);
						else if (angle2 <= 1)
							collision_normals.push_back(n2);
						else
							collision_normals.push_back(n3);
					}
				}
			}
			collisions.insert({ distance, {another_shape, collision_points, collision_normals}});
		}

		Vector3 center = { 0., 0., 0. };
		Vector3 normal = { 0., 0., 0. };
		//tuple<NiTriShapeRef, vector<Vector3>, vector<Vector3>> data;

		//vector<pair<size_t, tuple<NiTriShapeRef, vector<Vector3>, vector<Vector3>>>> colliders;
		//auto last_collider = collisions.rbegin();
		//while (last_collider != collisions.rend()) {
		//	if (last_collider->first != 0)
		//		colliders.push_back({ last_collider->first, last_collider->second });
		//	else
		//		break;
		//	last_collider++;
		//}

		//if (colliders.size())
		//{
		//	size_t num_colliders = colliders.size();
		//		size_t parent_index = 0;
		//		MinMaxer parent_bb; parent_bb.check(get<0>(colliders[0].second)->GetData()->GetVertices());
		//		data = colliders[0].second;
		//		for (auto& collider : colliders) {
		//			MinMaxer this_bb; this_bb.check(get<0>(collider.second)->GetData()->GetVertices());
		//			if (this_bb.extent().Magnitude() > this_bb.extent().Magnitude())
		//			{
		//				data = collider.second;
		//				parent_bb = this_bb;
		//			}
		//		}
		//}
		//else {
		//	__debugbreak();
		//}

		auto& data = collisions.rbegin()->second;
		if (get<1>(data).empty())
		{
			MinMaxer this_bb; this_bb.check(get<0>(data)->GetData()->GetVertices());
			//parents[shape] = { root_shape, {0.,0., this_bb.bb_min[2]} , { 0.,0.,1. } };
			parents[shape] = { NULL, {0.,0., 0.} , { 0.,0.,1. } };
		}
		else {
			for (const auto& point : get<1>(data)) {
				center += point;
			}
			center /= get<1>(data).size();
			for (const auto& vector : get<2>(data)) {
				normal += vector;
			}
			normal = normal.Normalized();
			auto mag = normal.Magnitude();
			if (!((mag) > 0.5 && (mag - 1.) < 10e-5)) {
				parents[shape] = { get<0>(data), center,{0.,0.,1.} };
			}
			else {
				assert((mag) > 0.5 && (mag - 1.) < 10e-5);
				parents[shape] = { get<0>(data), center, normal.Normalized() };
			}
		}
	}
	//check parenting didn't create loops

	for (auto& entry : parents) {
		vector<NiTriShapeRef> visited;
		NiTriShapeRef node = entry.first;
		while (node != NULL) {
			NiTriShapeRef next_node = get<0>(parents[node]);
			if (find(visited.begin(), visited.end(), next_node)!= visited.end()) {
				//find nearest node
				//NiTriShapeRef parent;
				//Vector3 distance = {numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max()};
				//Vector3 point = get<1>(parents[node]);
				//for (auto& another_entry : parents) {
				//	if (find(visited.begin(), visited.end(), another_entry.first) != visited.end())
				//		continue;
				//	Vector3 another_point = get<1>(another_entry.second);
				//	Vector3 current_distance = another_point - point;
				//	if (current_distance.Magnitude() < distance.Magnitude())
				//		parents[node] = { another_entry.first, another_point, get<2>(parents[node]) };
				//}
				parents[node] = { root_shape, get<1>(parents[node]), get<2>(parents[node]) };

			}
			else {
				visited.push_back(next_node);
				node = next_node;
			}
			
		}
	}


	NiNodeRef trunk;

	int i = 0;
	map<NiTriShapeRef, NiNodeRef> shapes_map;
	map< NiNodeRef, pair<Matrix33, Vector3>> transforms;
	map<NiTriShapeRef, string> _debug_shapes_names;
	for (const auto& entry : parents) {
		NiNodeRef node = new NiNode();
		node->SetName(to_string(i++));
		Vector3 center = get<1>(entry.second);
		Vector3 direction = get<2>(entry.second);
		if (get<0>(entry.second) == NULL) {
			shapes_map[entry.first] = node;
			_debug_shapes_names[entry.first] = node->GetName();
		}
		else {
			hkRotation rotation; rotation.setIdentity();
			auto hkdirection = TOVECTOR4(direction);
			if (hkdirection.equals3({ 0.,0.,1 })) 
			{
			}
			else if (hkdirection.equals3({ 0.,0.,-1 })) 
			{
				rotation.setAxisAngle({ 0.,0.,1 }, PI);
			}
			else {
				rotation = rotationBetweenVectors({ 0.,0.,1 }, TOVECTOR4(direction));
			}
			for (int r = 0; r < 3; r++) {
				hkVector4 vr; rotation.getRow(r, vr);
				for (int c = 0; c < 3; c++) {
					float value = vr.getSimdAt(c);
					if (isnan(value))
						__debugbreak();
				}
			}
			auto translation = TOVECTOR4(center);
			//hkRotation rotation; rotation.setIdentity();
			//auto translation = TOVECTOR4({ 10.,10.,10. });
			//wanted transform
			hkTransform transform(rotation, translation);		
			node->SetRotation(TOMATRIX33(transform.getRotation()));
			node->SetTranslation(TOVECTOR3(transform.getTranslation()));
			transforms[node] = { TOMATRIX33(transform.getRotation()), TOVECTOR3(transform.getTranslation()) };
			//transform.setInverse(transform);
			//adjust shape vertices
			/*NiTriShapeDataRef shape = DynamicCast<NiTriShapeData>(DynamicCast<NiTriShape>(entry.first)->GetData());
			auto& vertices = shape->GetVertices();
			for (auto& vector : vertices) {
				hkVector4 v = TOVECTOR4(vector);
				v.sub4(transform.getTranslation());
				transform.getRotation().multiplyVector(v,v);
				vector = TOVECTOR3(v);
			}
			shape->SetVertices(vertices);*/
			shapes_map[entry.first] = node;
			_debug_shapes_names[entry.first] = node->GetName();
		}
		//node->SetChildren({ DynamicCast<NiAVObject>(entry.first) });
		//root_children.push_back(StaticCast<NiAVObject>(node));
		
	}

	//multiple roots
	vector<NiAVObjectRef> roots;
	for (const auto& entry : parents) {
		NiTriShapeRef parent_shape = get<0>(entry.second);
		NiNodeRef child_node = shapes_map[entry.first];
		if (parent_shape == NULL)
		{
			roots.push_back(StaticCast<NiAVObject>(child_node));
		}
	}

	if (roots.size() > 1 ) {
		trunk = new NiNode();
		trunk->SetChildren(roots);
	}
	else if (roots.size() == 1) {
		trunk = DynamicCast<NiNode>(roots[0]);
	}
	else {
		__debugbreak();
	}
	trunk->SetName(string("TrunkBone"));
	//trunk->SetTranslation(TOVECTOR3(root_transform.getTranslation()));
	//trunk->SetRotation(TOMATRIX33(root_transform.getRotation()));

	map<string, string> _debug_parents;

	size_t debug_c = 0;
	for (const auto& entry : parents) {
		debug_c++;
		//get the parent node;
		NiTriShapeRef parent_shape = get<0>(entry.second);
		NiNodeRef child_node = shapes_map[entry.first];
		NiNodeRef parent_node;
		if (parent_shape == NULL)
		{
			continue;
		}
		else {
			parent_node = shapes_map[parent_shape];
		}

		auto child_children = child_node->GetChildren();

		while (find(child_children.begin(), child_children.end(), parent_node) != child_children.end())
		{
			parent_node = shapes_map[get<0>(parents[get<0>(entry.second)])];
			if (parent_node == child_node)
				parent_node = trunk;
		}

		string child_name = child_node->GetName();
		string parent_name = parent_node->GetName();

		_debug_parents[child_name] = parent_name;

		auto children = parent_node->GetChildren();
		children.push_back(StaticCast<NiAVObject>(child_node));
		parent_node->SetChildren(children);
	}

	//now visit and adjust branches 
	std::function<void(NiNodeRef, hkTransform)> relativize = [&](NiNodeRef root, hkTransform root_transform) {
		hkTransform fixed = TOHKTRANSFORM(root->GetRotation(), root->GetTranslation());

		hkRotation root_rotation = root_transform.getRotation();
		hkVector4 root_translation = root_transform.getTranslation();
		hkVector4 fixed_translation = fixed.getTranslation();
		hkVector4 displacement; displacement.setSub4(fixed_translation, root_translation);
		root_rotation.multiplyVector(displacement, displacement);
		hkRotation rotation; rotation.setMulInverse(fixed.getRotation(),root_rotation);

		root->SetRotation(TOMATRIX33(rotation));
		root->SetTranslation(TOVECTOR3(displacement));
		for (const auto& child : root->GetChildren()) {
			if (child->IsSameType(NiNode::TYPE)) 
			{
				relativize(DynamicCast<NiNode>(child), fixed);
			}
		}
	};

	//leaves
	CSpeedTreeRT::SGeometry::SLeaf& leaves = sGeom.m_sLeaves0;
	size_t max = numeric_limits<size_t>::min();
	for (int i = 0; i < leaves.m_usLeafCount; i++)
	{
		int index = leaves.m_pLeafMapIndices[i];
		if (index > max) max = index;
	}

	string metadata_name = "";
	string filename = file_in.filename().string();
	if (metadata.find(filename) != metadata.end())
		metadata_name = metadata.at(filename);

	float alphaTest = sGeom.m_sLeaves0.m_fAlphaTestValue;
	alpha_flags alphaflags;
	alphaflags.bits.alpha_test_enable = 1;
	alphaflags.bits.alpha_test_mode = alpha_flags::GL_GREATER;

	LeafCardData this_cluster;
	LeafCardData this_cluster_lod_0;
	LeafCardData this_cluster_lod_1;
	NiTriShapeRef leafcards = new NiTriShape();
	NiTriShapeRef leafcards_lod_0 = new NiTriShape();
	NiTriShapeRef leafcards_lod_1 = new NiTriShape();
	//if (leaftoclusters.find(cluster_index) == leaftoclusters.end()) {

	this_cluster.data = new NiTriShapeData();
	this_cluster.shader = new BSLightingShaderProperty();
	this_cluster.alpha = new NiAlphaProperty();

	this_cluster_lod_0.data = new NiTriShapeData();
	this_cluster_lod_0.shader = new BSLightingShaderProperty();
	this_cluster_lod_0.alpha = new NiAlphaProperty();

	this_cluster_lod_1.data = new NiTriShapeData();
	this_cluster_lod_1.shader = new BSLightingShaderProperty();
	this_cluster_lod_1.alpha = new NiAlphaProperty();

	this_cluster.alpha->SetFlags(alphaflags.value);
	this_cluster.alpha->SetThreshold(alphaTest);

	this_cluster_lod_0.alpha->SetFlags(alphaflags.value);
	this_cluster_lod_0.alpha->SetThreshold(alphaTest);

	this_cluster_lod_1.alpha->SetFlags(alphaflags.value);
	this_cluster_lod_1.alpha->SetThreshold(alphaTest);

	BSShaderTextureSetRef leaf_textures = new BSShaderTextureSet();
	BSShaderTextureSetRef leaf_textures_lod_0 = new BSShaderTextureSet();
	BSShaderTextureSetRef leaf_textures_lod_1 = new BSShaderTextureSet();

	std::string leaftexpath = "textures\\tes4\\trees\\leaves\\";
	vector<string> leaf_texture_names = {
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	};
	leaf_texture_names[0] = leaftexpath + fs::path(metadata_name).replace_extension(".dds").string();
	leaf_textures->SetTextures(leaf_texture_names);
	leaf_textures_lod_0->SetTextures(leaf_texture_names);
	leaf_textures_lod_1->SetTextures(leaf_texture_names);

	this_cluster.shader->SetTextureSet(leaf_textures);
	this_cluster.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster.shader->GetShaderFlags1_sk() & ~SLSF1_SPECULAR));
	this_cluster.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster.shader->GetShaderFlags1_sk() & ~SLSF1_REMAPPABLE_TEXTURES));
	this_cluster.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster.shader->GetShaderFlags1_sk() | SLSF1_SKINNED));
	this_cluster.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster.shader->GetShaderFlags1_sk() | SLSF1_VERTEX_ALPHA));
	this_cluster.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster.shader->GetShaderFlags2_sk() & ~SLSF2_VERTEX_COLORS));
	this_cluster.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster.shader->GetShaderFlags2_sk() | SLSF2_DOUBLE_SIDED));
	this_cluster.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster.shader->GetShaderFlags2_sk() | SLSF2_TREE_ANIM));
	this_cluster.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster.shader->GetShaderFlags2_sk() | SLSF2_SOFT_LIGHTING));

	this_cluster_lod_0.shader->SetTextureSet(leaf_textures_lod_0);
	this_cluster_lod_0.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster_lod_0.shader->GetShaderFlags1_sk() & ~SLSF1_SPECULAR));
	this_cluster_lod_0.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster_lod_0.shader->GetShaderFlags1_sk() & ~SLSF1_REMAPPABLE_TEXTURES));
	this_cluster_lod_0.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster_lod_0.shader->GetShaderFlags1_sk() | SLSF1_VERTEX_ALPHA));
	this_cluster_lod_0.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster_lod_0.shader->GetShaderFlags2_sk() & ~SLSF2_VERTEX_COLORS));
	this_cluster_lod_0.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster_lod_0.shader->GetShaderFlags2_sk() | SLSF2_DOUBLE_SIDED));
	this_cluster_lod_0.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster_lod_0.shader->GetShaderFlags2_sk() | SLSF2_TREE_ANIM));
	this_cluster_lod_0.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster_lod_0.shader->GetShaderFlags2_sk() | SLSF2_SOFT_LIGHTING));

	this_cluster_lod_1.shader->SetTextureSet(leaf_textures_lod_1);
	this_cluster_lod_1.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster_lod_1.shader->GetShaderFlags1_sk() & ~SLSF1_SPECULAR));
	this_cluster_lod_1.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster_lod_1.shader->GetShaderFlags1_sk() & ~SLSF1_REMAPPABLE_TEXTURES));
	this_cluster_lod_1.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster_lod_1.shader->GetShaderFlags1_sk() | SLSF1_VERTEX_ALPHA));
	this_cluster_lod_1.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster_lod_1.shader->GetShaderFlags2_sk() & ~SLSF2_VERTEX_COLORS));
	this_cluster_lod_1.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster_lod_1.shader->GetShaderFlags2_sk() | SLSF2_DOUBLE_SIDED));
	this_cluster_lod_1.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster_lod_1.shader->GetShaderFlags2_sk() | SLSF2_TREE_ANIM));
	this_cluster_lod_1.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster_lod_1.shader->GetShaderFlags2_sk() | SLSF2_SOFT_LIGHTING));

	leafcards->SetShaderProperty(StaticCast<BSShaderProperty>(this_cluster.shader));
	leafcards->SetAlphaProperty(this_cluster.alpha);
	leafcards->SetData(StaticCast<NiGeometryData>(this_cluster.data));
	leafcards->SetFlags(524302);

	leafcards_lod_0->SetShaderProperty(StaticCast<BSShaderProperty>(this_cluster_lod_0.shader));
	leafcards_lod_0->SetAlphaProperty(this_cluster_lod_0.alpha);
	leafcards_lod_0->SetData(StaticCast<NiGeometryData>(this_cluster_lod_0.data));
	leafcards_lod_0->SetFlags(524302);

	leafcards_lod_1->SetShaderProperty(StaticCast<BSShaderProperty>(this_cluster_lod_1.shader));
	leafcards_lod_1->SetAlphaProperty(this_cluster_lod_1.alpha);
	leafcards_lod_1->SetData(StaticCast<NiGeometryData>(this_cluster_lod_1.data));
	leafcards_lod_1->SetFlags(524302);

	this_cluster.data->SetHasVertices(true);
	this_cluster.data->SetHasNormals(true);
	this_cluster.data->SetHasUv(true);
	this_cluster.data->SetBsVectorFlags((BSVectorFlags)(this_cluster.data->GetBsVectorFlags() | BSVF_HAS_UV | BSVF_HAS_TANGENTS));
	this_cluster.data->SetHasTriangles(true);
	this_cluster.data->SetConsistencyFlags(CT_STATIC);
	this_cluster.data->SetHasVertexColors(true);

	this_cluster_lod_0.data->SetHasVertices(true);
	this_cluster_lod_0.data->SetHasNormals(true);
	this_cluster_lod_0.data->SetHasUv(true);
	this_cluster_lod_0.data->SetBsVectorFlags((BSVectorFlags)(this_cluster_lod_0.data->GetBsVectorFlags() | BSVF_HAS_UV | BSVF_HAS_TANGENTS));
	this_cluster_lod_0.data->SetHasTriangles(true);
	this_cluster_lod_0.data->SetConsistencyFlags(CT_STATIC);
	this_cluster_lod_0.data->SetHasVertexColors(true);

	this_cluster_lod_1.data->SetHasVertices(true);
	this_cluster_lod_1.data->SetHasNormals(true);
	this_cluster_lod_1.data->SetHasUv(true);
	this_cluster_lod_1.data->SetBsVectorFlags((BSVectorFlags)(this_cluster_lod_1.data->GetBsVectorFlags() | BSVF_HAS_UV | BSVF_HAS_TANGENTS));
	this_cluster_lod_1.data->SetHasTriangles(true);
	this_cluster_lod_1.data->SetConsistencyFlags(CT_STATIC);
	this_cluster_lod_1.data->SetHasVertexColors(true);

	vector<Vector3> original_vertices;

	auto leaf_vertices = this_cluster.data->GetVertices();
	auto leaf_normals = this_cluster.data->GetNormals();
	auto leaf_tangents = this_cluster.data->GetTangents();
	auto leaf_bitangents = this_cluster.data->GetBitangents();
	auto leaf_uvs = vector<TexCoord>();
	auto leaf_triangles = this_cluster.data->GetTriangles();
	auto leaf_colors = this_cluster.data->GetVertexColors();

	vector<Color4> const_colors;

	for (int i = 0; i < leaves.m_usLeafCount; i++)
	{
		int cluster_index = leaves.m_pLeafClusterIndices[i];
		const float* pCenters = leaves.m_pCenterCoords + i * 3;
		int index = leaves.m_pLeafMapIndices[i] >> 1;
		map<float, int> vcolors;
		float d1, d2, d3, d4;
		size_t color_index;
		size_t c_index;

		const float* pCoords = leaves.m_pLeafMapCoords[cluster_index];
		const float* pTex = leaves.m_pLeafMapTexCoords[cluster_index];
		const float* pNormal = &leaves.m_pNormals[i * 3];
		const float* pTangents = &leaves.m_pTangents[i * 3];
		const float* pBinormals = &leaves.m_pBinormals[i * 3];
		const unsigned int* pColors = &leaves.m_pColors[i];

		color_helper leaf_color; leaf_color.value = *pColors;

		const_colors = {
			{1.f, 1.f, 1.f, 0.},
			{1.f, 1.f, 1.f, 0.5},
			{1.f, 1.f, 1.f, 0.5},
			{1.f, 1.f, 1.f, 1.},
		};

		hkVector4 v1 = { pCoords[0], pCoords[1], pCoords[2] };
		hkVector4 v2 = { pCoords[4], pCoords[5], pCoords[6] };
		hkVector4 v3 = { pCoords[8], pCoords[9], pCoords[10] };
		hkVector4 v4 = { pCoords[12], pCoords[13], pCoords[14] };

		original_vertices.push_back({ pCenters[0] + pCoords[0],  pCenters[1] + pCoords[1],  pCenters[2] + pCoords[2] });
		original_vertices.push_back({ pCenters[0] + pCoords[4],  pCenters[1] + pCoords[5], pCenters[2] + pCoords[6] });
		original_vertices.push_back({ pCenters[0] + pCoords[8],  pCenters[1] + pCoords[9],  pCenters[2] + pCoords[10] });
		original_vertices.push_back({ pCenters[0] + pCoords[12], pCenters[1] + pCoords[13],  pCenters[2] + pCoords[14] });

		std::mt19937 rng(std::random_device{}()); //Unless you are using MinGW
		std::uniform_real_distribution<> dist(-PI / 4, PI / 4);

		hkRotation z_rot; z_rot.setAxisAngle({ 0.,0.,1. }, dist(rng));
		hkTransform z_trans; z_trans.setIdentity(); z_trans.setRotation(z_rot);
		v1.setTransformedPos(z_trans, v1);
		v2.setTransformedPos(z_trans, v2);
		v3.setTransformedPos(z_trans, v3);
		v4.setTransformedPos(z_trans, v4);

		d1 = v1.distanceToSquared3({ 0., 0., 0. });
		d2 = v2.distanceToSquared3({ 0., 0., 0. });
		d3 = v3.distanceToSquared3({ 0., 0., 0. });
		d4 = v4.distanceToSquared3({ 0., 0., 0. });

		vcolors.clear();
		c_index = 0;
		vcolors.insert({ d1, 0 });
		vcolors.insert({ d2, 1 });
		vcolors.insert({ d3, 2 });
		vcolors.insert({ d4, 3 });

		color_index = leaf_colors.size();
		leaf_colors.resize(leaf_colors.size() + 4);
		for (const auto& entry : vcolors)
		{
			leaf_colors[color_index + entry.second] = const_colors[c_index++];
		}



		leaf_vertices.push_back({ pCenters[0] + (float)v1.getSimdAt(0), pCenters[1] + (float)v1.getSimdAt(1), pCenters[2] + (float)v1.getSimdAt(2) });
		leaf_vertices.push_back({ pCenters[0] + (float)v2.getSimdAt(0), pCenters[1] + (float)v2.getSimdAt(1), pCenters[2] + (float)v2.getSimdAt(2) });
		leaf_vertices.push_back({ pCenters[0] + (float)v3.getSimdAt(0), pCenters[1] + (float)v3.getSimdAt(1), pCenters[2] + (float)v3.getSimdAt(2) });
		leaf_vertices.push_back({ pCenters[0] + (float)v4.getSimdAt(0), pCenters[1] + (float)v4.getSimdAt(1), pCenters[2] + (float)v4.getSimdAt(2) });

		leaf_uvs.push_back({ pTex[0] + index,  pTex[1] + index });
		leaf_uvs.push_back({ pTex[2] + index,  pTex[3] + index });
		leaf_uvs.push_back({ pTex[4] + index,  pTex[5] + index });
		leaf_uvs.push_back({ pTex[6] + index,  pTex[7] + index });

		leaf_triangles.push_back({ (unsigned short)(leaf_vertices.size() - 4),  (unsigned short)(leaf_vertices.size() - 3),  (unsigned short)(leaf_vertices.size() - 2) });
		leaf_triangles.push_back({ (unsigned short)(leaf_vertices.size() - 4),  (unsigned short)(leaf_vertices.size() - 2),  (unsigned short)(leaf_vertices.size() - 1) });

		Vector3 pCenter = { pCenters[0] , pCenters[1] , pCenters[2] };
		map<double, NiNodeRef> distances;
		for (const auto& entry : transforms) {
			auto bone_position = entry.second.second;
			double squared_distance =
				(pCenter[0] - bone_position[0]) * (pCenter[0] - bone_position[0]) +
				(pCenter[1] - bone_position[1]) * (pCenter[1] - bone_position[1]) +
				(pCenter[2] - bone_position[2]) * (pCenter[2] - bone_position[2]);
			distances[squared_distance] = entry.first;
		}

		leaf_triangle_bone_map.insert({ distances.begin()->second, leaf_triangles[leaf_triangles.size() - 2] });
		leaf_triangle_bone_map.insert({ distances.begin()->second, leaf_triangles[leaf_triangles.size() - 1] });

		hkRotation x_rot; x_rot.setAxisAngle({ 1.,0.,0. }, PI / 2 + dist(rng));
		hkTransform x_trans; x_trans.setIdentity(); x_trans.setRotation(x_rot);
		v1.setTransformedPos(x_trans, v1);
		v2.setTransformedPos(x_trans, v2);
		v3.setTransformedPos(x_trans, v3);
		v4.setTransformedPos(x_trans, v4);

		d1 = v1.distanceToSquared3({ 0., 0., 0. });
		d2 = v2.distanceToSquared3({ 0., 0., 0. });
		d3 = v3.distanceToSquared3({ 0., 0., 0. });
		d4 = v4.distanceToSquared3({ 0., 0., 0. });

		vcolors.clear();
		c_index = 0;
		vcolors.insert({ d1, 0 });
		vcolors.insert({ d2, 1 });
		vcolors.insert({ d3, 2 });
		vcolors.insert({ d4, 3 });

		color_index = leaf_colors.size();
		leaf_colors.resize(leaf_colors.size() + 4);
		for (const auto& entry : vcolors)
		{
			leaf_colors[color_index + entry.second] = const_colors[c_index++];
		}

		leaf_vertices.push_back({ pCenters[0] + (float)v1.getSimdAt(0), pCenters[1] + (float)v1.getSimdAt(1), pCenters[2] + (float)v1.getSimdAt(2) });
		leaf_vertices.push_back({ pCenters[0] + (float)v2.getSimdAt(0), pCenters[1] + (float)v2.getSimdAt(1), pCenters[2] + (float)v2.getSimdAt(2) });
		leaf_vertices.push_back({ pCenters[0] + (float)v3.getSimdAt(0), pCenters[1] + (float)v3.getSimdAt(1), pCenters[2] + (float)v3.getSimdAt(2) });
		leaf_vertices.push_back({ pCenters[0] + (float)v4.getSimdAt(0), pCenters[1] + (float)v4.getSimdAt(1), pCenters[2] + (float)v4.getSimdAt(2) });

		leaf_uvs.push_back({ pTex[0] + index,  pTex[1] + index });
		leaf_uvs.push_back({ pTex[2] + index,  pTex[3] + index });
		leaf_uvs.push_back({ pTex[4] + index,  pTex[5] + index });
		leaf_uvs.push_back({ pTex[6] + index,  pTex[7] + index });

		//leaf_vertices.push_back({ pCenters[0] + (float)v1.getSimdAt(0), pCenters[1] + (float)v1.getSimdAt(1), pCenters[2] + (float)v1.getSimdAt(2) });
		//leaf_vertices.push_back({ pCenters[0] + (float)v3.getSimdAt(0), pCenters[1] + (float)v3.getSimdAt(1), pCenters[2] + (float)v3.getSimdAt(2) });
		//

		//leaf_uvs.push_back({ pTex[0] + index,  pTex[1] + index });
		//leaf_uvs.push_back({ pTex[4] + index,  pTex[5] + index });
		

		leaf_triangles.push_back({ (unsigned short)(leaf_vertices.size() - 4),  (unsigned short)(leaf_vertices.size() - 3),  (unsigned short)(leaf_vertices.size() - 2) });
		leaf_triangles.push_back({ (unsigned short)(leaf_vertices.size() - 4),  (unsigned short)(leaf_vertices.size() - 2),  (unsigned short)(leaf_vertices.size() - 1) });

		leaf_triangle_bone_map.insert({ distances.begin()->second, leaf_triangles[leaf_triangles.size() - 2] });
		leaf_triangle_bone_map.insert({ distances.begin()->second, leaf_triangles[leaf_triangles.size() - 1] });

		z_rot.setAxisAngle({ 0.,0.,1. }, PI / 2 + dist(rng));
		z_trans; z_trans.setIdentity(); z_trans.setRotation(z_rot);
		v1.setTransformedPos(z_trans, v1);
		v2.setTransformedPos(z_trans, v2);
		v3.setTransformedPos(z_trans, v3);
		v4.setTransformedPos(z_trans, v4);

		d1 = v1.distanceToSquared3({ 0., 0., 0. });
		d2 = v2.distanceToSquared3({ 0., 0., 0. });
		d3 = v3.distanceToSquared3({ 0., 0., 0. });
		d4 = v4.distanceToSquared3({ 0., 0., 0. });

		vcolors.clear();
		c_index = 0;
		vcolors.insert({ d1, 0 });
		vcolors.insert({ d2, 1 });
		vcolors.insert({ d3, 2 });
		vcolors.insert({ d4, 3 });

		color_index = leaf_colors.size();
		leaf_colors.resize(leaf_colors.size() + 4);
		for (const auto& entry : vcolors)
		{
			leaf_colors[color_index + entry.second] = const_colors[c_index++];
		}

		leaf_vertices.push_back({ pCenters[0] + (float)v1.getSimdAt(0), pCenters[1] + (float)v1.getSimdAt(1), pCenters[2] + (float)v1.getSimdAt(2) });
		leaf_vertices.push_back({ pCenters[0] + (float)v2.getSimdAt(0), pCenters[1] + (float)v2.getSimdAt(1), pCenters[2] + (float)v2.getSimdAt(2) });
		leaf_vertices.push_back({ pCenters[0] + (float)v3.getSimdAt(0), pCenters[1] + (float)v3.getSimdAt(1), pCenters[2] + (float)v3.getSimdAt(2) });
		leaf_vertices.push_back({ pCenters[0] + (float)v4.getSimdAt(0), pCenters[1] + (float)v4.getSimdAt(1), pCenters[2] + (float)v4.getSimdAt(2) });

		leaf_uvs.push_back({ pTex[0] + index,  pTex[1] + index });
		leaf_uvs.push_back({ pTex[2] + index,  pTex[3] + index });
		leaf_uvs.push_back({ pTex[4] + index,  pTex[5] + index });
		leaf_uvs.push_back({ pTex[6] + index,  pTex[7] + index });

		//leaf_vertices.push_back({ pCenters[0] + (float)v1.getSimdAt(0), pCenters[1] + (float)v1.getSimdAt(1), pCenters[2] + (float)v1.getSimdAt(2) });
		//leaf_vertices.push_back({ pCenters[0] + (float)v3.getSimdAt(0), pCenters[1] + (float)v3.getSimdAt(1), pCenters[2] + (float)v3.getSimdAt(2) });
		//

		//leaf_uvs.push_back({ pTex[0] + index,  pTex[1] + index });
		//leaf_uvs.push_back({ pTex[4] + index,  pTex[5] + index });
		

		leaf_triangles.push_back({ (unsigned short)(leaf_vertices.size() - 4),  (unsigned short)(leaf_vertices.size() - 3),  (unsigned short)(leaf_vertices.size() - 2) });
		leaf_triangles.push_back({ (unsigned short)(leaf_vertices.size() - 4),  (unsigned short)(leaf_vertices.size() - 2),  (unsigned short)(leaf_vertices.size() - 1) });

		leaf_triangle_bone_map.insert({ distances.begin()->second, leaf_triangles[leaf_triangles.size() - 2] });
		leaf_triangle_bone_map.insert({ distances.begin()->second, leaf_triangles[leaf_triangles.size() - 1] });
	}

	//calculate normals 
	for (const auto& vertex : leaf_vertices) {
		Vector3 z_vertex = vertex;
		z_vertex.z = bb.bb_max[2];
		leaf_normals.push_back(z_vertex.Normalized());
	}

	Vector3 leaf_COM;
	TriGeometryContext g(leaf_vertices, leaf_COM, leaf_triangles, leaf_uvs, leaf_normals);

	bb.check(original_vertices);

	this_cluster.data->SetVertices(leaf_vertices);
	this_cluster.data->SetTriangles(leaf_triangles);
	this_cluster.data->SetNumTriangles(leaf_triangles.size());
	this_cluster.data->SetNumTrianglePoints(leaf_triangles.size() * 3);
	this_cluster.data->SetNormals(g.normals);
	this_cluster.data->SetTangents(g.tangents);
	this_cluster.data->SetBitangents(g.bitangents);
	this_cluster.data->SetUvSets({ leaf_uvs });
	this_cluster.data->SetVertexColors(leaf_colors);

	this_cluster_lod_0.data->SetVertices(leaf_vertices);
	this_cluster_lod_0.data->SetTriangles(leaf_triangles);
	this_cluster_lod_0.data->SetNumTriangles(leaf_triangles.size());
	this_cluster_lod_0.data->SetNumTrianglePoints(leaf_triangles.size() * 3);
	this_cluster_lod_0.data->SetNormals(g.normals);
	this_cluster_lod_0.data->SetTangents(g.tangents);
	this_cluster_lod_0.data->SetBitangents(g.bitangents);
	this_cluster_lod_0.data->SetUvSets({ leaf_uvs });
	this_cluster_lod_0.data->SetVertexColors(leaf_colors);

	this_cluster_lod_1.data->SetVertices(leaf_vertices);
	this_cluster_lod_1.data->SetTriangles(leaf_triangles);
	this_cluster_lod_1.data->SetNumTriangles(leaf_triangles.size());
	this_cluster_lod_1.data->SetNumTrianglePoints(leaf_triangles.size() * 3);
	this_cluster_lod_1.data->SetNormals(g.normals);
	this_cluster_lod_1.data->SetTangents(g.tangents);
	this_cluster_lod_1.data->SetBitangents(g.bitangents);
	this_cluster_lod_1.data->SetUvSets({ leaf_uvs });
	this_cluster_lod_1.data->SetVertexColors(leaf_colors);


	Accessor<TreeSkin> partitioner(full_shape, shapes_map, leafcards, triangle_bone_map, leaf_triangle_bone_map, transforms, StaticCast<NiNode>(root));
	hkTransform root_t; root_t.setIdentity();
	relativize(DynamicCast<NiNode>(trunk), root_t);

	BSMultiBoundNodeRef fadenode = new BSMultiBoundNode();
	fadenode->SetFlags(268959758);
	fadenode->SetCullingMode(BSCP_CULL_NORMAL);
	fadenode->SetName(string("FadeNode Anim"));

	BSMultiBoundRef bsmb = new BSMultiBound();
	BSMultiBoundOBBRef bsmbobb = new BSMultiBoundOBB();
	Vector3 bb_max = { bb.bb_max[0], bb.bb_max[1], bb.bb_max[2] };
	Vector3 bb_min = { bb.bb_min[0], bb.bb_min[1], bb.bb_min[2] };
	Vector3 center = (bb_max + bb_min) / 2;
	Vector3 size = (bb_max - bb_min) / 2;
	bsmbobb->SetCenter(center);
	bsmbobb->SetSize(size);

	bsmb->SetData(StaticCast<BSMultiBoundData>(bsmbobb));
	fadenode->SetMultiBound(bsmb);

	//Shapes
	NiSwitchNodeRef switch_lod_0 = new NiSwitchNode();
	switch_lod_0->SetName(file_out.filename().replace_extension("").string());
	switch_lod_0->SetFlags(2062);
	switch_lod_0->SetSwitchNodeFlags((NiSwitchFlags)(UPDATEONLYACTIVECHILD | UPDATECONTROLLERS));

	NiNodeRef no_lod_0 = new NiNode();
	no_lod_0->SetFlags(14);

	{
		NiSwitchNodeRef switch_lod_1 = new NiSwitchNode();
		switch_lod_1->SetName(file_out.filename().replace_extension("1").string());
		switch_lod_1->SetFlags(526350);
		switch_lod_1->SetSwitchNodeFlags((NiSwitchFlags)(UPDATEONLYACTIVECHILD));

		NiNodeRef no_lod_1 = new NiNode();
		no_lod_1->SetFlags(524302);
		no_lod_1->SetChildren({ StaticCast<NiAVObject>(trunk), StaticCast<NiAVObject>(full_shape), StaticCast<NiAVObject>(leafcards) });

		NiNodeRef lod_1 = new NiNode();
		lod_1->SetFlags(524302);
		lod_1->SetChildren({ StaticCast<NiAVObject>(full_shape_lod_1), StaticCast<NiAVObject>(leafcards_lod_1) });

		switch_lod_1->SetChildren({ StaticCast<NiAVObject>(no_lod_1), StaticCast<NiAVObject>(lod_1)});
		no_lod_0->SetChildren({ StaticCast<NiAVObject>(switch_lod_1) });
	}


	NiNodeRef lod_0 = new NiNode();
	lod_0->SetFlags(14);
	lod_0->SetChildren({ StaticCast<NiAVObject>(full_shape_lod_0), StaticCast<NiAVObject>(leafcards_lod_0) });

	switch_lod_0->SetChildren({ StaticCast<NiAVObject>(no_lod_0), StaticCast<NiAVObject>(lod_0) });

	fadenode->SetChildren({ StaticCast<NiAVObject>(switch_lod_0) });

	root_children.push_back(StaticCast<NiAVObject>(fadenode));
	root->SetChildren(root_children);
	root->SetFlags(268961806);

	BSXFlagsRef flags = new BSXFlags();
	flags->SetName(string("BSX"));
	flags->SetIntegerData(138); //always complex and articulated

	root->SetExtraDataList({ StaticCast<NiExtraData>(flags) });
	body->SetTranslation(trunk->GetTranslation());
	body->SetRotation(TOQUAT(trunk->GetRotation().AsQuaternion()));
	collision->SetTarget(trunk);
	trunk->SetCollisionObject(StaticCast<NiCollisionObject>(collision));
	root->SetBones1({ trunk });

	auto bones2 = root->GetBones();
	for (const auto& entry : shapes_map) {
		if (entry.second != trunk) {
			bones2.push_back(entry.second);
		}
	}
	root->SetBones(bones2);

	NifInfo info;
	info.userVersion = 12;
	info.userVersion2 = 83;
	info.version = Niflib::VER_20_2_0_7;

	WriteNifTree(file_out.string(), root, info);
}


void OutputShrubGeometry
(
	CSpeedTreeRT& speedTree,
	CSpeedTreeRT::SGeometry& sGeom,
	MinMaxer& bb,
	const fs::path& export_path,
	const fs::path file_in,
	const map<string, string>& metadata,
	int lod_level
)
{


	fs::path file_out = export_path / file_in.filename().replace_extension(".nif");
	if (lod_level > 0) {
		file_out = export_path / (file_in.filename().replace_extension("").string() + "_lod_0.nif");
	}

	const Games::GamesPathMapT& installations = games.getGames();
	CSpeedTreeRT::STextures sTextures;
	speedTree.GetTextures(sTextures);

	const SEmbeddedTexCoords* pSEmbeddedTexCoords = speedTree.m_pEmbeddedTexCoords;

	char branchTextureName[MAX_PATH];
	_splitpath(sTextures.m_pBranchTextureFilename, NULL, NULL, branchTextureName, NULL);

	BSLeafAnimNodeRef root = new BSLeafAnimNode();
	auto root_children = root->GetChildren();

	bhkCollisionObjectRef collision = new bhkCollisionObject();
	collision->SetFlags((bhkCOFlags)(BHKCO_ACTIVE | BHKCO_SYNC_ON_UPDATE));
	bhkRigidBodyRef body = new bhkRigidBody();
	auto layer = body->GetHavokFilter();
	layer.flagsAndPartNumber = 0;
	layer.group = 0;
	layer.layer_sk = SKYL_TREES;
	body->SetHavokFilter(layer);
	body->SetBroadPhaseType(BROAD_PHASE_ENTITY);
	auto cinfo = body->GetCinfoProperty();
	cinfo.data = 0;
	cinfo.size = 0;
	cinfo.capacityAndFlags = 2147483648;
	body->SetCinfoProperty(cinfo);
	body->SetCollisionResponse(RESPONSE_SIMPLE_CONTACT);
	body->SetProcessContactCallbackDelay(65535);
	body->SetHavokFilterCopy(layer);
	body->SetCollisionResponse2(RESPONSE_SIMPLE_CONTACT);
	body->SetProcessContactCallbackDelay2(65535);

	//trans/rot
	body->SetMass(0.);
	body->SetLinearDamping(0.099609);
	body->SetAngularDamping(0.049805);
	body->SetTimeFactor(1.);
	body->SetGravityFactor(1.);
	body->SetFriction(0.3);
	body->SetRollingFrictionMultiplier(0.);
	body->SetRestitution(0.8);
	body->SetMaxLinearVelocity(104.400002);
	body->SetMaxAngularVelocity(31.57);
	body->SetPenetrationDepth(0.15);
	body->SetMotionSystem(MO_SYS_BOX_STABILIZED);
	body->SetEnableDeactivation(true);
	body->SetSolverDeactivation(SOLVER_DEACTIVATION_OFF);
	body->SetQualityType(MO_QUAL_INVALID);

	collision->SetBody(StaticCast<bhkWorldObject>(body));

	hkpCreateShapeUtility::CreateShapeInput input;

	//Branches
	for (int nStrip = 0; nStrip < sGeom.m_sBranches.m_usNumStrips; nStrip++)
	{
		NiTriShapeDataRef data = new NiTriShapeData();
		BSLightingShaderPropertyRef shader = new BSLightingShaderProperty();
		BSShaderTextureSetRef textures = new BSShaderTextureSet();

		std::string texpath = "textures\\tes4\\trees\\branches\\";
		vector<string> texture_names = {
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			""
		};
		texture_names[0] = texpath + string(branchTextureName) + string(".dds");
		texture_names[1] = texpath + string(branchTextureName) + string("_n.dds");
		textures->SetTextures(texture_names);

		shader->SetTextureSet(textures);
		shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(shader->GetShaderFlags2_sk() & ~SLSF2_VERTEX_COLORS));
		shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(shader->GetShaderFlags1_sk() & ~SLSF1_SPECULAR));
		NiTriShapeRef branch = new NiTriShape();
		branch->SetShaderProperty(StaticCast<BSShaderProperty>(shader));
		branch->SetData(StaticCast<NiGeometryData>(data));

		data->SetHasVertices(true);
		auto vertices = data->GetVertices();
		data->SetHasNormals(true);
		auto normals = data->GetNormals();
		auto tangents = data->GetTangents();
		auto bitangents = data->GetBitangents();
		data->SetHasUv(true);
		data->SetBsVectorFlags((BSVectorFlags)(data->GetBsVectorFlags() | BSVF_HAS_UV | BSVF_HAS_TANGENTS));
		branch->SetFlags(524302);
		auto uvs = vector<TexCoord>();
		data->SetHasTriangles(true);
		auto triangles = data->GetTriangles();
		data->SetConsistencyFlags(CT_STATIC);
		data->SetHasVertexColors(false);
		auto colors = data->GetVertexColors();

		int nStripLength = sGeom.m_sBranches.m_pStripLengths[nStrip];
		const unsigned short* pStripIndices = sGeom.m_sBranches.m_pStrips[nStrip];
		for (int i = 0; i < nStripLength - 2; i++)
		{
			int nIndices[3] = { pStripIndices[i], pStripIndices[i + 1], pStripIndices[i + 2] };

			if (i % 2)
			{
				int tmp = nIndices[2];
				nIndices[2] = nIndices[1];
				nIndices[1] = tmp;
			}

			for (int j = 0; j < 3; j++)
			{
				const float* pPos = &sGeom.m_sBranches.m_pCoords[nIndices[j] * 3];
				const float* pNormal = &sGeom.m_sBranches.m_pNormals[nIndices[j] * 3];
				const float* pTangents = &sGeom.m_sBranches.m_pTangents[nIndices[j] * 3];
				const float* pBinormals = &sGeom.m_sBranches.m_pBinormals[nIndices[j] * 3];
				const float* pTexCoord = &sGeom.m_sBranches.m_pTexCoords0[nIndices[j] * 2];
				

				vertices.push_back({ pPos[0], pPos[1], pPos[2] });
				input.m_vertices.pushBack({ pPos[0], pPos[1], pPos[2] });
				normals.push_back({ pNormal[0],  pNormal[1], pNormal[2] });
				tangents.push_back({ pTangents[0],  pTangents[1], pTangents[2] });
				bitangents.push_back({ pBinormals[0], pBinormals[1], pBinormals[2] });
				uvs.push_back({ pTexCoord[0], pTexCoord[1] });
			}
			triangles.push_back({ (unsigned short)(vertices.size() - 3),  (unsigned short)(vertices.size() - 2),  (unsigned short)(vertices.size() - 1) });
		}

		bb.check(vertices);
		data->SetVertices(vertices);
		data->SetTriangles(triangles);
		data->SetNumTriangles(triangles.size());
		data->SetNumTrianglePoints(triangles.size() * 3);
		data->SetNormals(normals);
		data->SetTangents(tangents);
		data->SetBitangents(bitangents);
		data->SetUvSets({ uvs });

		root_children.push_back(StaticCast<NiAVObject>(branch));
	}

	hkpCreateShapeUtility::ShapeInfoOutput output;
	input.m_enableAutomaticShapeShrinking = false;
	hkpCreateShapeUtility::createSphereShape(input, output);
	hkpSphereShape* shape = (hkpSphereShape*)output.m_shape;

	bhkSphereShapeRef sphere = new bhkSphereShape();
	HavokMaterial mat; mat.material_sk = SKY_HAV_MAT_HEAVY_WOOD;
	sphere->SetMaterial(mat);
	sphere->SetRadius(shape->getRadius() * COLLISION_RATIO * 0.8);
	bhkConvexTransformShapeRef collision_transform = new bhkConvexTransformShape();
	collision_transform->SetMaterial(mat);
	collision_transform->SetRadius(shape->getRadius()* COLLISION_RATIO * 0.8);
	collision_transform->SetShape(StaticCast<bhkShape>(sphere));
	collision_transform->SetTransform(
		TOMATRIX44(output.m_decomposedWorldT, COLLISION_RATIO)
	);
	body->SetShape(StaticCast<bhkShape>(collision_transform));


	if (sTextures.m_uiFrondTextureCount > 1) {
		__debugbreak();
	}

	//for (unsigned int i = 0; i < sTextures.m_uiFrondTextureCount; i++)
	//{
		char filestub[MAX_PATH];
		_splitpath(sTextures.m_pLeafTextureFilenames[0], NULL, NULL, filestub, NULL);
		float alphaTest = sGeom.m_sLeaves0.m_fAlphaTestValue;

	//}

	//Oblivion trees have no fronds
	for (int nStrip = 0; nStrip < sGeom.m_sFronds.m_usNumStrips; nStrip++)
	{
		int nStripLength = sGeom.m_sFronds.m_pStripLengths[nStrip];
		const unsigned short* pStripIndices = sGeom.m_sFronds.m_pStrips[nStrip];
		for (int i = 0; i < nStripLength - 2; i++)
		{
			//NiTriShapeDataRef data = new NiTriShapeData();
			//BSLightingShaderPropertyRef shader = new BSLightingShaderProperty();
			//BSShaderTextureSetRef textures = new BSShaderTextureSet();
			//const Games::GamesPathMapT& installations = games.getGames();

			//std::string texpath = "textures/tes4/trees/branches/";
			//vector<string> texture_names = {
			//	"",
			//	"",
			//	"",
			//	"",
			//	"",
			//	"",
			//	"",
			//	"",
			//	""
			//};
			//texture_names[0] = texpath + string(branchTextureName) + string(".dds");
			//texture_names[1] = texpath + string(branchTextureName) + string("_n.dds");
			//textures->SetTextures(texture_names);

			//shader->SetTextureSet(textures);
			//shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(shader->GetShaderFlags2_sk() & ~SLSF2_VERTEX_COLORS));
			//NiTriShapeRef branch = new NiTriShape();
			//branch->SetShaderProperty(StaticCast<BSShaderProperty>(shader));
			//branch->SetData(StaticCast<NiGeometryData>(data));

			//data->SetHasVertices(true);
			//auto vertices = data->GetVertices();
			//data->SetHasNormals(true);
			//auto normals = data->GetNormals();
			//auto tangents = data->GetTangents();
			//auto bitangents = data->GetBitangents();
			//data->SetHasUv(true);
			//data->SetBsVectorFlags((BSVectorFlags)(data->GetBsVectorFlags() | BSVF_HAS_UV | BSVF_HAS_TANGENTS));
			//branch->SetFlags(524302);
			//auto uvs = vector<TexCoord>();
			//data->SetHasTriangles(true);
			//auto triangles = data->GetTriangles();
			//data->SetConsistencyFlags(CT_STATIC);
			//data->SetHasVertexColors(false);
			//auto colors = data->GetVertexColors();

			//int nStripLength = sGeom.m_sBranches.m_pStripLengths[nStrip];
			//const unsigned short* pStripIndices = sGeom.m_sBranches.m_pStrips[nStrip];
			//for (int i = 0; i < nStripLength - 2; i++)
			//{
			//	int nIndices[3] = { pStripIndices[i], pStripIndices[i + 1], pStripIndices[i + 2] };

			//	if (i % 2)
			//	{
			//		int tmp = nIndices[2];
			//		nIndices[2] = nIndices[1];
			//		nIndices[1] = tmp;
			//	}

			//	//fprintf(file, "%s_VertexLit\n", branchTextureName);

			//	for (int j = 0; j < 3; j++)
			//	{
			//		const float* pPos = &sGeom.m_sBranches.m_pCoords[nIndices[j] * 3];
			//		const float* pNormal = &sGeom.m_sBranches.m_pNormals[nIndices[j] * 3];
			//		const float* pTangents = &sGeom.m_sBranches.m_pTangents[nIndices[j] * 3];
			//		const float* pBinormals = &sGeom.m_sBranches.m_pBinormals[nIndices[j] * 3];
			//		const float* pTexCoord = &sGeom.m_sBranches.m_pTexCoords0[nIndices[j] * 2];


			//		vertices.push_back({ pPos[0], pPos[1], pPos[2] });
			//		normals.push_back({ pNormal[0], pNormal[1], pNormal[2] });
			//		tangents.push_back({ pTangents[0], pTangents[1], pTangents[2] });
			//		bitangents.push_back({ pBinormals[0], pBinormals[1], pBinormals[2] });
			//		uvs.push_back({ pTexCoord[0], pTexCoord[1] });

			//		//fprintf(file, "0 %f %f %f %f %f %f %f %f 0\n", pPos[0], pPos[1], pPos[2],
			//		//	pNormal[0], pNormal[1], pNormal[2],
			//		//	pTexCoord[0], pTexCoord[1]);
			//	}
			//	triangles.push_back({ (unsigned short)(vertices.size() - 3),  (unsigned short)(vertices.size() - 2),  (unsigned short)(vertices.size() - 1) });
			//}

			//data->SetVertices(vertices);
			//data->SetTriangles(triangles);
			//data->SetNumTriangles(triangles.size());
			//data->SetNumTrianglePoints(triangles.size() * 3);
			//data->SetNormals(normals);
			//data->SetTangents(tangents);
			//data->SetBitangents(bitangents);
			//data->SetUvSets({ uvs });

			//root_children.push_back(StaticCast<NiAVObject>(branch));


			int nIndices[3] = { pStripIndices[i], pStripIndices[i + 1], pStripIndices[i + 2] };

			if (i % 2)
			{
				int tmp = nIndices[2];
				nIndices[2] = nIndices[1];
				nIndices[1] = tmp;
			}

			char frondTextureName[MAX_PATH];
			_splitpath(sTextures.m_pFrondTextureFilenames[0], NULL, NULL, frondTextureName, NULL);

			for (int j = 0; j < 3; j++)
			{
				const float* pPos = &sGeom.m_sFronds.m_pCoords[nIndices[j] * 3];
				const float* pNormal = &sGeom.m_sFronds.m_pNormals[nIndices[j] * 3];
				const float* pTexCoord = &sGeom.m_sFronds.m_pTexCoords0[nIndices[j] * 2];

			}
		}
	}

	//float* pLeafCentres = new float[sTextures.m_uiLeafTextureCount * 3];
	//int* pLeafCounts = new int[sTextures.m_uiLeafTextureCount];
	//for (unsigned int i = 0; i < sTextures.m_uiLeafTextureCount; i++)
	//{
	//	pLeafCentres[i * 3] = 0.0f;
	//	pLeafCentres[i * 3 + 1] = 0.0f;
	//	pLeafCentres[i * 3 + 2] = 0.0f;
	//	pLeafCounts[i] = 0;
	//}

	

	map<int, int> leaftoclusters;
	vector<LeafCardData>  clusters;

	vector<string> names;
	/*
	*             // leaves
            unsigned int        m_uiLeafTextureCount;       // the number of char* elements in m_pLeafTextureFilenames 
            const char**        m_pLeafTextureFilenames;    // array of null-terminated strings m_uiLeafTextureCount in size

            // fronds
            unsigned int        m_uiFrondTextureCount;      // the number of char* elements in m_pFrondTextureFilenames 
            const char**        m_pFrondTextureFilenames;   // array of null-terminated strings m_uiFrondTextureCount in size

            // composite
            const char*         m_pCompositeFilename;       // null-terminated string
	*/
	//for (int i = 0; i < sTextures.m_uiLeafTextureCount; i++)
	//	names.push_back(sTextures.m_pLeafTextureFilenames[i]);
	//for (int i = 0; i < sTextures.m_uiFrondTextureCount; i++)
	//	names.push_back(sTextures.m_pFrondTextureFilenames[i]);
	//names.push_back(sTextures.m_pCompositeFilename);
	
	CSpeedTreeRT::SGeometry::SLeaf& leaves = sGeom.m_sLeaves0;
	size_t max = numeric_limits<size_t>::min();
	for (int i = 0; i < leaves.m_usLeafCount; i++)
	{
		int index = leaves.m_pLeafMapIndices[i];
		if (index > max) max = index;
	}

	string metadata_name = "";
	string filename = file_in.filename().string();
	if (metadata.find(filename) != metadata.end())
		metadata_name = metadata.at(filename);


	LeafCardData this_cluster;
	NiTriShapeRef leafcards = new NiTriShape();
	//if (leaftoclusters.find(cluster_index) == leaftoclusters.end()) {

	this_cluster.data = new NiTriShapeData();
	this_cluster.shader = new BSLightingShaderProperty();
	this_cluster.alpha = new NiAlphaProperty();

	//NiTriShapeDataRef data = new NiTriShapeData();
	//BSLightingShaderPropertyRef shader = new BSLightingShaderProperty();
	//NiAlphaPropertyRef alpha = new NiAlphaProperty();
	BSShaderTextureSetRef textures = new BSShaderTextureSet();

	alpha_flags flags;
	flags.bits.alpha_test_enable = 1;
	flags.bits.alpha_test_mode = alpha_flags::GL_GREATER;
	this_cluster.alpha->SetFlags(flags.value);
	this_cluster.alpha->SetThreshold(alphaTest);

	std::string texpath = "textures\\tes4\\trees\\leaves\\";
	vector<string> texture_names = {
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	};
	texture_names[0] = texpath + fs::path(metadata_name).replace_extension(".dds").string();
	textures->SetTextures(texture_names);

	this_cluster.shader->SetTextureSet(textures);
	this_cluster.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster.shader->GetShaderFlags1_sk() & ~SLSF1_SPECULAR));
	this_cluster.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster.shader->GetShaderFlags1_sk() & ~SLSF1_REMAPPABLE_TEXTURES));
	this_cluster.shader->SetShaderFlags1_sk(SkyrimShaderPropertyFlags1(this_cluster.shader->GetShaderFlags1_sk() | SLSF1_VERTEX_ALPHA));
	//this_cluster.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster.shader->GetShaderFlags2_sk() | SLSF2_VERTEX_COLORS));
	this_cluster.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster.shader->GetShaderFlags2_sk() | SLSF2_DOUBLE_SIDED));
	this_cluster.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster.shader->GetShaderFlags2_sk() | SLSF2_TREE_ANIM));
	//this_cluster.shader->SetShaderFlags2_sk(SkyrimShaderPropertyFlags2(this_cluster.shader->GetShaderFlags2_sk() | SLSF2_SOFT_LIGHTING));

	leafcards->SetShaderProperty(StaticCast<BSShaderProperty>(this_cluster.shader));
	leafcards->SetAlphaProperty(this_cluster.alpha);
	leafcards->SetData(StaticCast<NiGeometryData>(this_cluster.data));

	this_cluster.data->SetHasVertices(true);
	auto vertices = this_cluster.data->GetVertices();
	auto normals = this_cluster.data->GetNormals();
	auto tangents = this_cluster.data->GetTangents();
	auto bitangents = this_cluster.data->GetBitangents();
	this_cluster.data->SetHasNormals(true);
	this_cluster.data->SetHasUv(true);
	this_cluster.data->SetBsVectorFlags((BSVectorFlags)(this_cluster.data->GetBsVectorFlags() | BSVF_HAS_UV | BSVF_HAS_TANGENTS));
	leafcards->SetFlags(524302);
	auto uvs = vector<TexCoord>();
	this_cluster.data->SetHasTriangles(true);
	auto triangles = this_cluster.data->GetTriangles();
	this_cluster.data->SetConsistencyFlags(CT_STATIC);
	this_cluster.data->SetHasVertexColors(true);
	auto colors = this_cluster.data->GetVertexColors();
	vector<Vector3> original_vertices;
	vector<Color4> const_colors;


	for (int i = 0; i < leaves.m_usLeafCount; i++)
	{
		//string texture_name = metadata_name;
		int cluster_index = leaves.m_pLeafClusterIndices[i];
		const float* pCenters = leaves.m_pCenterCoords + i * 3;

		map<float, int> vcolors;
		float d1, d2, d3, d4;
		size_t color_index;
		size_t c_index;

		int index = leaves.m_pLeafMapIndices[i] >> 1;

		const float* pCoords = leaves.m_pLeafMapCoords[cluster_index];
		const float* pTex = leaves.m_pLeafMapTexCoords[cluster_index];
		const float* pNormal = &leaves.m_pNormals[i * 3];
		const float* pTangents = &leaves.m_pTangents[i * 3];
		const float* pBinormals = &leaves.m_pBinormals[i * 3];

		//color_helper leaf_color; leaf_color.value = *pColors;

		const_colors = {
			{1.f, 1.f, 1.f, 0.},
			{1.f, 1.f, 1.f, 0.5},
			{1.f, 1.f, 1.f, 0.5},
			{1.f, 1.f, 1.f, 1.},
		};

		hkVector4 v1 = { pCoords[0], pCoords[1], pCoords[2] };
		hkVector4 v2 = { pCoords[4], pCoords[5], pCoords[6] };
		hkVector4 v3 = { pCoords[8], pCoords[9], pCoords[10] };
		hkVector4 v4 = { pCoords[12], pCoords[13], pCoords[14] };

		original_vertices.push_back({ pCenters[0] + pCoords[0], pCenters[0] + pCoords[1],pCenters[0] + pCoords[2] });
		original_vertices.push_back({ pCenters[0] + pCoords[4], pCenters[0] + pCoords[5], pCenters[0] + pCoords[6] });
		original_vertices.push_back({ pCenters[0] + pCoords[8], pCenters[0] + pCoords[9], pCenters[0] + pCoords[10] });
		original_vertices.push_back({ pCenters[0] + pCoords[12], pCenters[0] + pCoords[13], pCenters[0] + pCoords[14] });

		std::mt19937 rng(std::random_device{}()); //Unless you are using MinGW
		std::uniform_real_distribution<> dist(-PI / 4, PI / 4);

		hkRotation z_rot; z_rot.setAxisAngle({ 0.,0.,1. }, dist(rng));
		hkTransform z_trans; z_trans.setIdentity(); z_trans.setRotation(z_rot);
		v1.setTransformedPos(z_trans, v1);
		v2.setTransformedPos(z_trans, v2);
		v3.setTransformedPos(z_trans, v3);
		v4.setTransformedPos(z_trans, v4);


		vertices.push_back({ pCenters[0] + (float)v1.getSimdAt(0), pCenters[1] + (float)v1.getSimdAt(1), pCenters[2] + (float)v1.getSimdAt(2) });
		vertices.push_back({ pCenters[0] + (float)v2.getSimdAt(0), pCenters[1] + (float)v2.getSimdAt(1), pCenters[2] + (float)v2.getSimdAt(2) });
		vertices.push_back({ pCenters[0] + (float)v3.getSimdAt(0), pCenters[1] + (float)v3.getSimdAt(1), pCenters[2] + (float)v3.getSimdAt(2) });
		vertices.push_back({ pCenters[0] + (float)v4.getSimdAt(0), pCenters[1] + (float)v4.getSimdAt(1), pCenters[2] + (float)v4.getSimdAt(2) });

		d1 = vertices[vertices.size() - 4].Magnitude();
		d2 = vertices[vertices.size() - 3].Magnitude();
		d3 = vertices[vertices.size() - 2].Magnitude();
		d4 = vertices[vertices.size() - 1].Magnitude();

		vcolors.clear();
		c_index = 0;
		vcolors.insert({ d1, 0 });
		vcolors.insert({ d2, 1 });
		vcolors.insert({ d3, 2 });
		vcolors.insert({ d4, 3 });

		color_index = colors.size();
		colors.resize(colors.size() + 4);
		for (const auto& entry : vcolors)
		{
			colors[color_index + entry.second] = const_colors[c_index++];
		}


		uvs.push_back({ pTex[0]+ index,  pTex[1]+ index });
		uvs.push_back({ pTex[2]+ index,  pTex[3]+ index });
		uvs.push_back({ pTex[4]+ index,  pTex[5]+ index });
		uvs.push_back({ pTex[6] + index,  pTex[7] + index });


		triangles.push_back({ (unsigned short)(vertices.size() - 4),  (unsigned short)(vertices.size() - 3),  (unsigned short)(vertices.size() - 2) });
		triangles.push_back({ (unsigned short)(vertices.size() - 4),  (unsigned short)(vertices.size() - 2),  (unsigned short)(vertices.size() - 1) });

		hkRotation x_rot; x_rot.setAxisAngle({ 1.,0.,0. }, PI/2 + dist(rng));
		hkTransform x_trans; x_trans.setIdentity(); x_trans.setRotation(x_rot);
		v1.setTransformedPos(x_trans, v1);
		v2.setTransformedPos(x_trans, v2);
		v3.setTransformedPos(x_trans, v3);
		v4.setTransformedPos(x_trans, v4);

		vertices.push_back({ pCenters[0] + (float)v1.getSimdAt(0), pCenters[1] + (float)v1.getSimdAt(1), pCenters[2] + (float)v1.getSimdAt(2) });
		vertices.push_back({ pCenters[0] + (float)v2.getSimdAt(0), pCenters[1] + (float)v2.getSimdAt(1), pCenters[2] + (float)v2.getSimdAt(2) });
		vertices.push_back({ pCenters[0] + (float)v3.getSimdAt(0), pCenters[1] + (float)v3.getSimdAt(1), pCenters[2] + (float)v3.getSimdAt(2) });
		vertices.push_back({ pCenters[0] + (float)v4.getSimdAt(0), pCenters[1] + (float)v4.getSimdAt(1), pCenters[2] + (float)v4.getSimdAt(2) });

		d1 = vertices[vertices.size() - 4].Magnitude();
		d2 = vertices[vertices.size() - 3].Magnitude();
		d3 = vertices[vertices.size() - 2].Magnitude();
		d4 = vertices[vertices.size() - 1].Magnitude();

		vcolors.clear();
		c_index = 0;
		vcolors.insert({ d1, 0 });
		vcolors.insert({ d2, 1 });
		vcolors.insert({ d3, 2 });
		vcolors.insert({ d4, 3 });

		color_index = colors.size();
		colors.resize(colors.size() + 4);
		for (const auto& entry : vcolors)
		{
			colors[color_index + entry.second] = const_colors[c_index++];
		}

		uvs.push_back({ pTex[0] + index,  pTex[1] + index });
		uvs.push_back({ pTex[2] + index,  pTex[3] + index });
		uvs.push_back({ pTex[4] + index,  pTex[5] + index });
		uvs.push_back({ pTex[6] + index,  pTex[7] + index });

		triangles.push_back({ (unsigned short)(vertices.size() - 4),  (unsigned short)(vertices.size() - 3),  (unsigned short)(vertices.size() - 2) });
		triangles.push_back({ (unsigned short)(vertices.size() - 4),  (unsigned short)(vertices.size() - 2),  (unsigned short)(vertices.size() - 1) });

		z_rot.setAxisAngle({ 0.,0.,1. }, PI / 2 + dist(rng));
		z_trans; z_trans.setIdentity(); z_trans.setRotation(z_rot);
		v1.setTransformedPos(z_trans, v1);
		v2.setTransformedPos(z_trans, v2);
		v3.setTransformedPos(z_trans, v3);
		v4.setTransformedPos(z_trans, v4);


		vertices.push_back({ pCenters[0] + (float)v1.getSimdAt(0), pCenters[1] + (float)v1.getSimdAt(1), pCenters[2] + (float)v1.getSimdAt(2) });
		vertices.push_back({ pCenters[0] + (float)v2.getSimdAt(0), pCenters[1] + (float)v2.getSimdAt(1), pCenters[2] + (float)v2.getSimdAt(2) });
		vertices.push_back({ pCenters[0] + (float)v3.getSimdAt(0), pCenters[1] + (float)v3.getSimdAt(1), pCenters[2] + (float)v3.getSimdAt(2) });
		vertices.push_back({ pCenters[0] + (float)v4.getSimdAt(0), pCenters[1] + (float)v4.getSimdAt(1), pCenters[2] + (float)v4.getSimdAt(2) });

		d1 = vertices[vertices.size() - 4].Magnitude();
		d2 = vertices[vertices.size() - 3].Magnitude();
		d3 = vertices[vertices.size() - 2].Magnitude();
		d4 = vertices[vertices.size() - 1].Magnitude();

		vcolors.clear();
		c_index = 0;
		vcolors.insert({ d1, 0 });
		vcolors.insert({ d2, 1 });
		vcolors.insert({ d3, 2 });
		vcolors.insert({ d4, 3 });

		color_index = colors.size();
		colors.resize(colors.size() + 4);
		for (const auto& entry : vcolors)
		{
			colors[color_index + entry.second] = const_colors[c_index++];
		}

		uvs.push_back({ pTex[0] + index,  pTex[1] + index });
		uvs.push_back({ pTex[2] + index,  pTex[3] + index });
		uvs.push_back({ pTex[4] + index,  pTex[5] + index });
		uvs.push_back({ pTex[6] + index,  pTex[7] + index });

		triangles.push_back({ (unsigned short)(vertices.size() - 4),  (unsigned short)(vertices.size() - 3),  (unsigned short)(vertices.size() - 2) });
		triangles.push_back({ (unsigned short)(vertices.size() - 4),  (unsigned short)(vertices.size() - 2),  (unsigned short)(vertices.size() - 1) });

	}

	//calculate normals 
	for (const auto& vertex : vertices) {
		Vector3 z_vertex = vertex;
		z_vertex.z = bb.bb_max[2];
		normals.push_back(z_vertex.Normalized());
	}

	Vector3 COM;
	TriGeometryContext g(vertices, COM, triangles, uvs, normals);

	bb.check(original_vertices);

	this_cluster.data->SetVertices(vertices);
	this_cluster.data->SetTriangles(triangles);
	this_cluster.data->SetNumTriangles(triangles.size());
	this_cluster.data->SetNumTrianglePoints(triangles.size() * 3);
	this_cluster.data->SetNormals(normals);
	this_cluster.data->SetTangents(g.tangents);
	this_cluster.data->SetBitangents(g.bitangents);
	this_cluster.data->SetUvSets({ uvs });
	colors.resize(vertices.size(), { 1.,1.,1.,1. });
	this_cluster.data->SetVertexColors(colors);
	root_children.push_back(StaticCast<NiAVObject>(leafcards));

	//shrubs have no collision, apparently
	//collision->SetTarget(root);
	//root->SetCollisionObject(StaticCast<NiCollisionObject>(collision));
	//BSXFlagsRef bsxflags = new BSXFlags();
	//bsxflags->SetName(string("BSX"));
	//bsxflags->SetIntegerData(138); //always complex and articulated
	//root->SetExtraDataList({ StaticCast<NiExtraData>(bsxflags) });

	root->SetChildren(root_children);



	NifInfo info;
	info.userVersion = 12;
	info.userVersion2 = 83;
	info.version = Niflib::VER_20_2_0_7;

	WriteNifTree(file_out.string(), root, info);

}



void sptconvert(const fs::path& spt_file, const fs::path& export_path, const map<string, string>& metadata)
{
	CSpeedTreeRT speedTree;

	if (speedTree.LoadTree(spt_file.string().c_str()))
	{
		bool flipping = speedTree.GetTextureFlip();
		speedTree.SetTextureFlip(true);

		float fSize, fVariance;
		speedTree.GetTreeSize(fSize, fVariance);
		speedTree.SetTreeSize(fSize * (10), fVariance); //scale to units from feet and then back to oblivion
		speedTree.SetDropToBillboard(true);
		
		const float transform[16] =
		{
			1., 0., 0., 0.,
			0., 0., 1., 0.,
			0., -1., 0., 0.,
			0., 0., 0., 1.,
		};

		if (speedTree.Compute(transform, 1, false))
		{
			MinMaxer bb;
			CSpeedTreeRT::SGeometry sGeom;
			speedTree.GetGeometry(sGeom, SpeedTree_AllGeometry);
			const float* mat = speedTree.GetLeafMaterial();

			if (spt_file.filename().string().find("shrub") == 0)
				OutputShrubGeometry(speedTree, sGeom, bb, export_path, spt_file, metadata, 0);
			else
				OutputTreeGeometry(speedTree, sGeom, bb, export_path, spt_file, metadata, 0);

			//speedTree.SetLodLevel(0.1);
			//speedTree.Compute(transform, 1, false);
			//speedTree.GetGeometry(sGeom, SpeedTree_AllGeometry);

			//OutputTreeGeometry(speedTree, sGeom, bb, export_path, spt_file, metadata, 1);
			
			speedTree.SetLodLevel(0.);
			speedTree.Compute(transform, 1, false);
			speedTree.GetGeometry(sGeom, SpeedTree_BillboardGeometry);
			//speedTree.SetLodLevel(0.);
			//speedTree.Compute(transform, 1, false);
			OutputBillboardGeometry(speedTree, sGeom, bb, export_path, spt_file, metadata, 0);
			OutputBillboardGeometry(speedTree, sGeom, bb, export_path, spt_file, metadata, 1);

		}
		else
		{
			// Trouble with compute
		}
	}
	else
	{
		// Trouble with load
	}
}