#include <commands/geometry.h>

using namespace ckcmd::Geometry;

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

void ckcmd::Geometry::CalculateNormals(const vector<Vector3>& vertices, const vector<Triangle> faces,
	vector<Vector3>& normals, Vector3& COM, bool sphericalNormals, bool calculateCOM) {

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


	//std::map<unsigned int, vector<Vector3>> normalMap;

	//if (normals.size() != vertices.size())
	//	normals.resize(vertices.size());
	////test faces before start
	//set<unsigned int> faceIndexes;
	//for (Triangle face : faces) {
	//	faceIndexes.insert(face.v1); faceIndexes.insert(face.v2); faceIndexes.insert(face.v3);
	//}

	//for (size_t i = 0; i < vertices.size(); i++)
	//	if (faceIndexes.find(i) == faceIndexes.end())
	//		throw runtime_error("Found unindexed vertex: " + i);

	//if (calculateCOM)
	//	COM = centeroid(vertices);

	//for (Triangle face : faces) {
	//	Vector3 v1 = vertices[face.v1].Normalized();
	//	Vector3 v2 = vertices[face.v2].Normalized();
	//	Vector3 v3 = vertices[face.v3].Normalized();

	//	Vector3 v12 = v2 - v1;
	//	Vector3 v13 = v3 - v1;
	//	Vector3 v23 = v3 - v2;

	//	//All the possible normals
	//	Vector3 n1 = v12 ^ v13;
	//	Vector3 n2 = v12 ^ v23;
	//	Vector3 n3 = v13 ^ v23;

	//	Vector3 COT = centeroid(vector<Vector3>({ v1,v2,v3 }));

	//	Vector3 COMtoCOT = Vector3(COT - COM).Normalized();

	//	//we always want a normal that is faced out of the body
	//	//if (sphericalNormals) {
	//	//	Vector3 COMv1 = Vector3(v1 - COM);
	//	//	n1 = COMv1.Normalized();
	//	//	Vector3 COMv2 = Vector3(v2 - COM);
	//	//	n2 = COMv2.Normalized();
	//	//	Vector3 COMv3 = Vector3(v3 - COM);
	//	//	n3 = COMv3.Normalized();
	//	//}
	//	//else {
	//	//	if (n1 * COMtoCOT < 0)
	//	//		n1 = Vector3(-n1.x, -n1.y, -n1.z);
	//	//	if (n2 * COMtoCOT < 0)
	//	//		n2 = Vector3(-n2.x, -n2.y, -n2.z);
	//	//	if (n3 * COMtoCOT < 0)
	//	//		n3 = Vector3(-n3.x, -n3.y, -n3.z);
	//	//}

	//	normalMap[face.v1].push_back(n1.Normalized());
	//	normalMap[face.v2].push_back(n1.Normalized());
	//	normalMap[face.v3].push_back(n1.Normalized());
	//}

	//for (size_t i = 0; i < vertices.size(); i++) {
	//	normals[i] = centeroid(normalMap[i]).Normalized();
	//}
}

bool CheckNormals(const vector<Vector3>& normals) {
	for (Vector3 v : normals)
		if (v != Vector3(0.0, 0.0, 0.0))
			return true;
	return false;
}