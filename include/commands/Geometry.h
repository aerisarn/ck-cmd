#pragma once

#include <nif_basic_types.h>

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

#include <nif_math.h>

#include <mikktspace.h>

#include <igl/per_vertex_normals.h>
//
//#include <fbxsdk.h>
//

#include <algorithm>
#include <tuple>
#include <bitset>

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

typedef pair<int, float> boneweight;

typedef std::bitset<12> bsx_flags_t;
namespace Niflib
{
	bsx_flags_t calculateSkyrimBSXFlags(const vector<NiObjectRef>& blocks, const NifInfo& info);

	class SingleChunkFlagVerifier : public RecursiveFieldVisitor<SingleChunkFlagVerifier> {

		int n_collisions = 0;
		int n_phantoms = 0;
		int n_constraints = 0;
		bool hasBranches = false;
		bool branchesResult = true;
		set<pair<bhkEntity*, bhkEntity*>>& entitiesPair;
		const Niflib::NifInfo& this_info;
		set<Niflib::NiObject*>& alreadyVisitedNodes;

	public:
		bool singleChunkVerified = false;

		SingleChunkFlagVerifier(NiObject& data, const NifInfo& info) :
			RecursiveFieldVisitor(*this, info), this_info(info), alreadyVisitedNodes(set<NiObject*>()), entitiesPair(set<pair<bhkEntity*, bhkEntity*>>())
		{
			data.accept(*this, info);

			bool singlechain = false;
			if (n_collisions - n_constraints == 1) {
				singlechain = true;
				singleChunkVerified = true;
			}
			if (n_phantoms > 0 && (singlechain || n_collisions == 0)) {
				singleChunkVerified = true;
			}

			if (hasBranches) {
				if (n_collisions == 0 && n_phantoms == 0)
					singleChunkVerified = singleChunkVerified || branchesResult;
				else
					singleChunkVerified = singleChunkVerified && branchesResult;
			}
		}

		SingleChunkFlagVerifier(NiObject& data, const NifInfo& info, set<NiObject*>& alreadyVisitedNodes, set<pair<bhkEntity*, bhkEntity*>>& entitiesPair) :
			RecursiveFieldVisitor(*this, info), this_info(info), alreadyVisitedNodes(alreadyVisitedNodes), entitiesPair(entitiesPair)
		{
			data.accept(*this, info);

			bool singlechain = false;
			if (n_collisions - n_constraints == 1) {
				singlechain = true;
				singleChunkVerified = true;
			}
			if (n_phantoms > 0 && (singlechain || n_collisions == 0)) {
				singleChunkVerified = true;
			}
			if (hasBranches) {
				if (n_collisions == 0 && n_phantoms == 0)
					singleChunkVerified = singleChunkVerified || branchesResult;
				else
					singleChunkVerified = singleChunkVerified && branchesResult;
			}

			if (n_phantoms == 0 && n_collisions == 0)
				singleChunkVerified = true;
		}

		template<class T>
		inline void visit_object(T& obj) {
			NiObject* ptr = (NiObject*)&obj;
			if (alreadyVisitedNodes.insert(ptr).second) {
				NiObjectRef ref = DynamicCast<NiObject>(ptr);
				if (ref->IsSameType(NiSwitchNode::TYPE)) {
					branchesResult = false;
					hasBranches = true;
					bool singleResult = true;
					NiSwitchNodeRef ref = DynamicCast<NiSwitchNode>(ptr);
					for (NiAVObjectRef child : ref->GetChildren()) {
						bool result = SingleChunkFlagVerifier(*child, this_info, alreadyVisitedNodes, entitiesPair).singleChunkVerified;
						singleResult = singleResult && result;
					}
					branchesResult = branchesResult || singleResult;
				}

				if (ref->IsDerivedType(bhkSPCollisionObject::TYPE)) {
					n_phantoms++;
				}
				if (ref->IsDerivedType(bhkCollisionObject::TYPE)) {
					n_collisions++;
				}

				if (ref->IsDerivedType(bhkConstraint::TYPE)) {
					bhkConstraintRef cref = DynamicCast<bhkConstraint>(ref);
					std::pair<bhkEntity*, bhkEntity*> p;
					p.first = *cref->GetEntities().begin();
					p.second = *(++cref->GetEntities().begin());
					if (entitiesPair.insert(p).second)
						n_constraints++;
				}
			}
		}

		template<class T>
		inline void visit_compound(T& obj) {}

		template<class T>
		inline void visit_field(T& obj) {}

	};

	class MarkerBranchVisitor : public RecursiveFieldVisitor<MarkerBranchVisitor> {
		set<NiObject*>& alreadyVisitedNodes;
		bool insideBranch = false;
		const NifInfo& this_info;
	public:
		bool marker = false;

		MarkerBranchVisitor(NiObject& data, const NifInfo& info) :
			RecursiveFieldVisitor(*this, info), alreadyVisitedNodes(set<NiObject*>()), this_info(info)
		{
			data.accept(*this, info);
		}

		//verifies that markers are not inside branches

		MarkerBranchVisitor(NiObject& data, const NifInfo& info, bool insideBranch, set<NiObject*>& alreadyVisitedNodes) :
			RecursiveFieldVisitor(*this, info), insideBranch(insideBranch), alreadyVisitedNodes(alreadyVisitedNodes), this_info(info)
		{
			data.accept(*this, info);
		}

		template<class T>
		inline void visit_object(T& obj) {
			NiObject* ptr = (NiObject*)&obj;
			if (alreadyVisitedNodes.insert(ptr).second) {
				NiObjectRef ref = DynamicCast<NiObject>(ptr);
				if (ref->IsSameType(NiSwitchNode::TYPE)) {
					NiSwitchNodeRef ref = DynamicCast<NiSwitchNode>(ptr);
					//For whatever reason, seems like the EditorMarker flag actually is just taking into account the first branch,
					//which is the active one by default. If the editor is in the other, like into the money bag, the flag is reset
					if (!ref->GetChildren().empty()) {
						marker = MarkerBranchVisitor(*ref->GetChildren()[0], this_info, false, alreadyVisitedNodes).marker;
					}
					//mark all as no marker, could be actually done iteratively on the blocks list instead of visiting
					for (NiAVObjectRef child : ref->GetChildren()) {
						MarkerBranchVisitor(*child, this_info, true, alreadyVisitedNodes);
					}
					//This construct is used for ambient fish to be taken and disapper, they are flagged
					//if (ref->GetChildren().size() == 2 && ref->GetChildren()[1]->IsDerivedType(NiNode::TYPE)) {
					//	NiNodeRef ref = DynamicCast<NiNode>(ref->GetChildren()[1]);
					//	if (ref->GetName().empty() && ref->GetChildren().empty())
					//		marker = MarkerBranchVisitor(*ref->GetChildren()[0], this_info, false, alreadyVisitedNodes).marker;
					//	else
					//		//mark all as no marker, could be actually done iteratively on the blocks list instead of visiting
					//		for (NiAVObjectRef child : ref->GetChildren()) {
					//			MarkerBranchVisitor(*child, this_info, true, alreadyVisitedNodes);
					//		}
					//}
					//else {
					//	//mark all as no marker, could be actually done iteratively on the blocks list instead of visiting
					//	for (NiAVObjectRef child : ref->GetChildren()) {
					//		MarkerBranchVisitor(*child, this_info, true, alreadyVisitedNodes);
					//	}
					//}
				}

				if (ref->IsSameType(BSOrderedNode::TYPE)) {
					BSOrderedNodeRef ref = DynamicCast<BSOrderedNode>(ptr);
					for (NiAVObjectRef child : ref->GetChildren()) {
						MarkerBranchVisitor(*child, this_info, true, alreadyVisitedNodes);
					}
				}

				if (!insideBranch && ref->IsDerivedType(NiObjectNET::TYPE)) {
					NiObjectNETRef node = DynamicCast<NiObjectNET>(ref);
					if (node->GetName().find("EditorMarker") != string::npos)
						marker = true;
				}

			}
		}

		template<class T>
		inline void visit_compound(T& obj) {}

		template<class T>
		inline void visit_field(T& obj) {}
	};
}

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

Niflib::NiTriShapeRef remake_partitions(Niflib::NiTriBasedGeomRef iShape, int & maxBonesPerPartition, int & maxBonesPerVertex, bool make_strips, bool pad);

namespace ckcmd {
namespace Geometry {

	using namespace Niflib;

	Vector3 centeroid(const vector<Vector3>& in);

	void CalculateNormals(const vector<Vector3>& vertices, const vector<Triangle>& faces,
		vector<Vector3>& normals, Vector3& COM, bool sphericalNormals = false, bool calculateCOM = false);

	vector<Triangle> triangulate(vector<unsigned short> strip);
	vector<Triangle> triangulate(vector<vector<unsigned short>> strips);

	struct TriGeometryContext : SMikkTSpaceContext
	{
		//input
		const vector<Vector3>& vertices;
		const vector<Triangle>& faces;
		Vector3 COM;

		//output
		vector<Vector3>& normals;

		//TSpace

		//input
		const vector<TexCoord>& uvs;

		//output
		vector<Vector3> tangents;
		vector<Vector3> bitangents;
		TriGeometryContext(const vector<Vector3>& in_vertices, Vector3 COM, const vector<Triangle>& in_faces,
			const vector<TexCoord>& in_uvs, vector<Vector3>& in_normals) : vertices(in_vertices), faces(in_faces), uvs(in_uvs), normals(in_normals) {
			if (normals.empty() || normals.size() != vertices.size()) {
				normals.resize(vertices.size());
				CalculateNormals(vertices, faces, normals, COM, false);
			}
			tangents.resize(vertices.size());
			bitangents.resize(vertices.size());
			calculate(this);
		}

		static int get_num_faces(const SMikkTSpaceContext *x) {
			return ((TriGeometryContext*)x)->faces.size();
		}

		static int get_num_vertices_of_face(const SMikkTSpaceContext *x, int f) {
			return 3;
		}

		static void get_position(const SMikkTSpaceContext *x, float *dst, int f, int v) {
			Triangle face = ((TriGeometryContext*)x)->faces[f];
			Vector3 v_out = (v == 0 ? ((TriGeometryContext*)x)->vertices[face.v1] : v == 1 ? ((TriGeometryContext*)x)->vertices[face.v2] : ((TriGeometryContext*)x)->vertices[face.v3]);
			memcpy(dst, &v_out, 3 * sizeof(float));
		}

		static void get_normal(const SMikkTSpaceContext *x, float *dst, int f, int v) {
			Triangle face = ((TriGeometryContext*)x)->faces[f];
			Vector3 n_out = (v == 0 ? ((TriGeometryContext*)x)->normals[face.v1] : v == 1 ? ((TriGeometryContext*)x)->normals[face.v2] : ((TriGeometryContext*)x)->normals[face.v3]);
			memcpy(dst, &n_out, 3 * sizeof(float));
		}

		static void get_tex_coord(const SMikkTSpaceContext *x, float *dst, int f, int v) {
			Triangle face = ((TriGeometryContext*)x)->faces[f];
			TexCoord uv_out = (v == 0 ? ((TriGeometryContext*)x)->uvs[face.v1] : v == 1 ? ((TriGeometryContext*)x)->uvs[face.v2] : ((TriGeometryContext*)x)->uvs[face.v3]);
			memcpy(dst, &uv_out, 2 * sizeof(float));
		}


		// bitangent = fSign * cross(vN, tangent);
		static void set_tspace_basic(
			const SMikkTSpaceContext *x,
			const float *t,
			float s,
			int f,
			int v
		) {
			Triangle face = ((TriGeometryContext*)x)->faces[f];
			unsigned short t_index = (v == 0 ? face.v1 : v == 1 ? face.v2 : face.v3);
			((TriGeometryContext*)x)->tangents[t_index] = Vector3(t[0], 1.0-t[1], t[2]).Normalized();
			((TriGeometryContext*)x)->bitangents[t_index] = (((TriGeometryContext*)x)->normals[t_index] ^ ((TriGeometryContext*)x)->tangents[t_index]);
			((TriGeometryContext*)x)->bitangents[t_index] *= s;
			//((TriGeometryContext*)x)->bitangents[t_index] = ((TriGeometryContext*)x)->bitangents[t_index].Normalized();
		}

		static void set_tspace(
			const SMikkTSpaceContext *x,
			const float *t,
			const float *b,
			float mag_s,
			float mag_t,
			tbool op,
			int f,
			int v
		) {
			set_tspace_basic(x, t, op != 0 ? 1.0f : -1.0f, f, v);
		}

		static void calculate(TriGeometryContext* obj) {
			using namespace std::placeholders;

			SMikkTSpaceInterface mikkt_interface;

			mikkt_interface.m_getNumFaces = get_num_faces;

			mikkt_interface.m_getNumVerticesOfFace = get_num_vertices_of_face;
			mikkt_interface.m_getPosition = get_position;
			mikkt_interface.m_getNormal = get_normal;
			mikkt_interface.m_getTexCoord = get_tex_coord;
			mikkt_interface.m_setTSpaceBasic = set_tspace_basic;
			mikkt_interface.m_setTSpace = set_tspace;

			obj->m_pInterface = &mikkt_interface;
			obj->m_pUserData = NULL;

			genTangSpaceDefault(obj);
		}

	};
//
//	using namespace Niflib;
//
//	template<class From, class To> To convert(const From& mesh_in) ;
//	template<class From, class To, class Container> To convert(const From& mesh_in, Container container);
//
//	template<> FbxMesh* convert(const NiTriShape& mesh_in, FbxScene* container) {
//		FbxMesh* mesh_out = FbxMesh::Create(container, mesh_in.GetName().c_str());
//
//		//Get materials from the shape
//
//		//move vertices
//		mesh_out->InitControlPoints(mesh_in.GetData()->GetVertices().size());
//
//		return mesh_out;
//	}
//
}
}