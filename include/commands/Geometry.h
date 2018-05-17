#pragma once
#include <nif_math.h>

#include <mikktspace.h>

#include <igl/per_vertex_normals.h>
//
//#include <fbxsdk.h>
//
namespace ckcmd {
namespace Geometry {

	using namespace Niflib;

	Vector3 centeroid(const vector<Vector3>& in);

	void CalculateNormals(const vector<Vector3>& vertices, const vector<Triangle> faces,
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
		vector<Vector3> normals;

		//TSpace

		//input
		const vector<TexCoord>& uvs;

		//output
		vector<Vector3> tangents;
		vector<Vector3> bitangents;
		TriGeometryContext(const vector<Vector3>& in_vertices, Vector3 COM, const vector<Triangle>& in_faces,
			const vector<TexCoord>& in_uvs, vector<Vector3> in_normals) : vertices(in_vertices), faces(in_faces), uvs(in_uvs), normals(in_normals) {
			//if (normals.size() == 0 && !CheckNormals(normals)) {
			normals.resize(vertices.size());
			CalculateNormals(vertices, faces, normals, COM, false);
			//}
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
			((TriGeometryContext*)x)->tangents[t_index] = Vector3(t[0], t[1], t[2]).Normalized();
			((TriGeometryContext*)x)->bitangents[t_index] = (((TriGeometryContext*)x)->vertices[t_index] ^ ((TriGeometryContext*)x)->tangents[t_index]);
			((TriGeometryContext*)x)->bitangents[t_index] *= s;
			((TriGeometryContext*)x)->bitangents[t_index] = ((TriGeometryContext*)x)->bitangents[t_index].Normalized();
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