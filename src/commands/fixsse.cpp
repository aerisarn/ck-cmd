#include "stdafx.h"

#include <commands/fixsse.h>


using namespace ckcmd;
using namespace ckcmd::fixsse;
using namespace ckcmd::Geometry;

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

		if (iSkinPart == NULL)
			iSkinPart = iSkinData->GetSkinPartition();

		// read in the weights from NiSkinData

		int numVerts = iData->GetVertices().size(); //nif->get<int>(iData, "Num Vertices");
		vector<vector<boneweight> > weights(numVerts);

		auto& iBoneList = iSkinData->GetBoneList(); //nif->getIndex(iSkinData, "Bone List");
		int numBones = iBoneList.size();

		for (int bone = 0; bone < numBones; bone++) {
			auto& iVertexWeights = iBoneList[bone].vertexWeights;
			//QModelIndex iVertexWeights = nif->getIndex(iBoneList.child(bone, 0), "Vertex Weights");

			for (int r = 0; r < iVertexWeights.size(); r++) {
				int vertex = iVertexWeights[r].index; // nif->get<int>(iVertexWeights.child(r, 0), "Index");
				float weight = iVertexWeights[r].weight;  //nif->get<float>(iVertexWeights.child(r, 0), "Weight");

				if (vertex >= weights.size())
					throw runtime_error("bad NiSkinData - vertex count does not match");

				weights[vertex].push_back(boneweight(bone, weight));
			}
		}

		// count min and max bones per vertex

		int minBones, maxBones;
		minBones = maxBones = weights[0].size();
		for (const vector<boneweight> list : weights) {
			if (list.size() < minBones)
				minBones = list.size();

			if (list.size() > maxBones)
				maxBones = list.size();
		}

		if (minBones <= 0)
			throw runtime_error("bad NiSkinData - some vertices have no weights at all");

		//query max bones per vertex/partition

		if (maxBonesPerPartition <= 0 || maxBonesPerVertex <= 0) {

			maxBonesPerPartition = 60;
			maxBonesPerVertex = 4;
			make_strips = false;
			pad = false;
		}

		// reduce vertex influences if necessary

		if (maxBones > maxBonesPerVertex) {
			vector<vector<boneweight>>::iterator it = weights.begin();
			int c = 0;

			while (it != weights.end()) {
				vector<boneweight> & lst = *it;
				std::sort(lst.begin(), lst.end(), boneweight_equivalence());

				if (lst.size() > maxBonesPerVertex)
					c++;

				while (lst.size() > maxBonesPerVertex) {
					lst.erase(lst.end()-1);
				}

				float totalWeight = 0;
				for (const auto bw : lst) {
					totalWeight += bw.second;
				}

				for (int b = 0; b < lst.size(); b++) {
					// normalize
					lst[b].second /= totalWeight;
				}
				it++;
			}

			//				qCWarning( nsSpell ) << Spell::tr( "Reduced %1 vertices to %2 bone influences (maximum number of bones per vertex was %3)" )
			//					.arg( c )
			//					.arg( maxBonesPerVertex )
			//					.arg( maxBones );
		}

		maxBones = maxBonesPerVertex;

		// reduces bone weights so that the triangles fit into the partitions

		vector<Triangle> triangles;

		if (iShape->IsSameType(NiTriShape::TYPE)) {

			triangles = (DynamicCast<NiTriShapeData>(iData))->GetTriangles(); // nif->getArray<Triangle>(iData, "Triangles").toList();
		}
		else if (iShape->IsSameType(NiTriStrips::TYPE)) {
			// triangulate first (code copied from strippify.cpp)
			auto& iPoints = (DynamicCast<NiTriStripsData>(iData))->GetPoints();

			vector<vector<unsigned short> > strips;
			//QModelIndex iPoints = nif->getIndex(iData, "Points");

			for (int s = 0; s < iPoints.size(); s++) {
				vector<unsigned short> strip;
				auto& iStrip = iPoints[s];

				for (int p = 0; p < iStrip.size(); p++) {
					strip.push_back(iStrip[p]);
				}

				strips.push_back(strip);
			}

			triangles = triangulate(strips);
		}

		map<Triangle, unsigned int> trimap;
		int defaultPart = 0;

		//if (nif->inherits(iSkinInst, "BSDismemberSkinInstance")) {
		if (iSkinInst->IsDerivedType(BSDismemberSkinInstance::TYPE)) {
			BSDismemberSkinInstanceRef iBSSkinInst = DynamicCast<BSDismemberSkinInstance>(iSkinInst);
			// First find a partition to dump dangling faces.  Torso is prefered if available.
			unsigned int nparts = iBSSkinInst->GetPartitions().size(); // GetNnif->get<uint>(iSkinInst, "Num Partitions");
			auto& iPartData = iBSSkinInst->GetPartitions();

			for (int i = 0; i < nparts; ++i) {
				auto& iPart = iPartData[i];

				//if (!iPart.isValid())
				//	continue;

				if (iPart.bodyPart == 0 /* Torso */) {
					defaultPart = i;
					break;
				}
			}

			defaultPart = min(nparts - 1, defaultPart);

			// enumerate existing partitions and select faces into same partition
			unsigned int nskinparts = iSkinPart->GetSkinPartitionBlocks().size(); //"Num Skin Partition Blocks");
			auto& iPartBlockData = iSkinPart->GetSkinPartitionBlocks();

			for (int i = 0; i < nskinparts; ++i) {
				auto& iPart = iPartBlockData[i];

				//if (!iPart.isValid())
				//	continue;

				int finalPart = min(nparts - 1, i);

				auto& vertmap = iPart.vertexMap; // nif->getArray<int>(iPart, "Vertex Map");

				bool hasFaces = iPart.hasFaces; // nif->get<quint8>(iPart, "Has Faces");
				unsigned short numStrips = iPart.numStrips; // nif->get<quint8>(iPart, "Num Strips");
				vector<Triangle> partTriangles;

				if (hasFaces && numStrips == 0) {
					partTriangles = iPart.triangles; // nif->getArray<Triangle>(iPart, "Triangles");
				}
				else if (numStrips != 0) {
					// triangulate first (code copied from strippify.cpp)
					vector<vector<unsigned short> > strips;
					auto& iPoints = iPart.strips;
					//QModelIndex iPoints = nif->getIndex(iPart, "Strips");

					for (int s = 0; s < iPoints.size(); s++) {
						vector<unsigned short> strip;
						auto& iStrip = iPoints[s];

						for (int p = 0; p < iStrip.size(); p++) {
							strip.push_back(iStrip[p]);
						}

						strips.push_back(strip);
					}

					partTriangles = triangulate(strips);
				}

				for (int j = 0; j < partTriangles.size(); ++j) {
					Triangle t = partTriangles[j];
					Triangle tri = t;

					if (!vertmap.empty()) {
						tri[0] = vertmap[tri[0]];
						tri[1] = vertmap[tri[1]];
						tri[2] = vertmap[tri[2]];
					}

					qRotate(tri);
					trimap[tri] = finalPart;
				}
			}
		}

		multimap<int, int> match;
		typedef multimap<int, int>::iterator MatchIterator;
		bool doMatch = true;

		list<int> tribones;

		int cnt = 0;

		for (const Triangle& tri : triangles)
		{
			do {
				//tribones.clear();

				for (int c = 0; c < 3; c++) {
					for (const auto bw : weights[tri[c]]) {
						if (find(tribones.begin(), tribones.end(),bw.first) == tribones.end())
							tribones.push_back(bw.first);
					}
				}

				if (tribones.size() > maxBonesPerPartition) {
					// sum up the weights for each bone
					// bones with weight == 1 can't be removed

					map<int, float> sum;
					list<int> nono;

					for (int t = 0; t < 3; t++) {
						if (weights[tri[t]].size() == 1)
							nono.push_back(weights[tri[t]].begin()->first);

						for (const auto bw : weights[tri[t]]) {
							sum[bw.first] += bw.second;
						}
					}

					// select the bone to remove

					float minWeight = 5.0;
					int minBone = -1;

					for (const auto b : sum) {
						if (find(nono.begin(),nono.end(),b.first)!= nono.end() && sum[b.first] < minWeight) {
							minWeight = sum[b.first];
							minBone = b.first;
						}
					}

					if (minBone < 0)  // this shouldn't never happen
						throw runtime_error("internal error 0x01");

					// do a vertex match detect

					if (doMatch) {
						auto& verts = iData->GetVertices(); // nif->getArray<Vector3>(iData, "Vertices");

						for (int a = 0; a < verts.size(); a++) {
							match.insert(pair<int, int>({ a, a }));

							for (int b = a + 1; b < verts.size(); b++) {
								if (verts[a] == verts[b] && weights[a] == weights[b]) {
									match.insert(pair<int, int>({ a, b }));
									match.insert(pair<int, int>({ b, a }));
								}
							}
						}
					}

					// now remove that bone from all vertices of this triangle and from all matching vertices too

					for (int t = 0; t < 3; t++) {
						bool rem = false;
						pair<MatchIterator, MatchIterator> val = match.equal_range(tri[t]);
						for (MatchIterator it = val.first; it != val.second; it++)
						{
							auto& v = it->second;
							vector<boneweight> & bws = weights[v];
							vector<boneweight>::iterator bit = bws.begin();

							while (bit != bws.end()) {
								boneweight & bw = *bit; 

								if (bw.first == minBone) {
									bit = bws.erase(bit);
									rem = true;
								}
								else
									bit++;
							}

							float totalWeight = 0;
							for (const auto bw : bws) {
								totalWeight += bw.second;
							}

							if (totalWeight == 0)
								throw runtime_error("internal error 0x02");

							for (int b = 0; b < bws.size(); b++) {
								// normalize
								bws[b].second /= totalWeight;
							}
						}

						if (rem)
							cnt++;
					}
				}
			} while (tribones.size() > maxBonesPerPartition);
		}

		if (cnt > 0)
			Log::Info("Removed %d bone influences",cnt);

		// split the triangles into partitions

		bool merged = true;

		vector<Partition> parts;

		if (!trimap.empty()) {
			vector<Triangle>::iterator it = triangles.begin();

			while (it != triangles.end()) {
				Triangle& tri = *it;
				qRotate(tri);
				map<Triangle, unsigned int> ::iterator partItr = trimap.find(tri);
				int partIdx = (partItr != trimap.end()) ? partItr->second : defaultPart;
				bool erase = false;
				if (partIdx >= 0) {
					//Triangle & tri = *it;

					// Ensure enough partitions
					while (partIdx >= int(parts.size()))
						parts.push_back(Partition());

					Partition & part = parts[partIdx];

					list<int> tribones;

					for (int c = 0; c < 3; c++) {
						for (const auto bw : weights[tri[c]]) {
							if (find(tribones.begin(), tribones.end(), bw.first) == tribones.end())
								tribones.push_back(bw.first);
						}
					}

					part.bones = mergeBones(part.bones, tribones);
					part.triangles.push_back(tri);
					erase = true;					
				}
				if (erase)
					it = triangles.erase(it);
				else
					it++;
			}

			merged = false; // when explicit mapping enabled, no merging is allowed
		}

		while (!triangles.empty()) {
			Partition part;

			unordered_map<int, bool> usedVerts;

			bool addtriangles;

			do {
				vector<Triangle>::iterator it = triangles.begin();

				while (it != triangles.end()) {
					Triangle & tri = *it;

					list<int> tribones;

					for (int c = 0; c < 3; c++) {
						for (const auto bw : weights[tri[c]]) {
							if (find(tribones.begin(),tribones.end(), bw.first)==tribones.end()) //!tribones.contains(bw.first))
								tribones.push_back(bw.first);
						}
					}

					if (part.bones.empty() || containsBones(part.bones, tribones)) {
						part.bones = mergeBones(part.bones, tribones);
						part.triangles.push_back(tri);
						usedVerts[tri[0]] = true;
						usedVerts[tri[1]] = true;
						usedVerts[tri[2]] = true;
						it = triangles.erase(it);
					}
					else {
						it++;
					}

				}

				addtriangles = false;

				if (part.bones.size() < maxBonesPerPartition) {
					// if we have room left in the partition then add an adjacent triangle
					it = triangles.begin();

					while (it!= triangles.end()) {
						Triangle & tri = *it;

						if (usedVerts.find(tri[0]) != usedVerts.end() 
							|| usedVerts.find(tri[1]) != usedVerts.end() 
							|| usedVerts.find(tri[2]) != usedVerts.end()) {
							list<int> tribones;

							for (int c = 0; c < 3; c++) {
								for (const auto bw : weights[tri[c]]) {
									if (find(tribones.begin(),tribones.end(), bw.first) == tribones.end()) //tribones.contains(bw.first))
										tribones.push_back(bw.first);
								}
							}

							tribones = mergeBones(part.bones, tribones);

							if (tribones.size() <= maxBonesPerPartition) {
								part.bones = tribones;
								part.triangles.push_back(tri);
								usedVerts[tri[0]] = true;
								usedVerts[tri[1]] = true;
								usedVerts[tri[2]] = true;
								it = triangles.erase(it); //.remove();
								addtriangles = true;
								//break;
							}
							else
								it++;
						}
						else
							it++;
					}
				}
			} while (addtriangles);

			parts.push_back(part);
		}

		//qDebug() << parts.count() << "small partitions";

		// merge partitions

		while (merged) {
			merged = false;

			for (int p1 = 0; p1 < parts.size() && !merged; p1++) {
				if (parts[p1].bones.size() < maxBonesPerPartition) {
					for (int p2 = p1 + 1; p2 < parts.size() && !merged; p2++) {
						list<int> mergedBones = mergeBones(parts[p1].bones, parts[p2].bones);

						if (mergedBones.size() <= maxBonesPerPartition) {
							parts[p1].bones = mergedBones;
							parts[p1].triangles.insert(parts[p1].triangles.end(), parts[p2].triangles.begin(), parts[p2].triangles.end());
							parts.erase(parts.begin() + p2);
							merged = true;
						}
					}
				}
			}
		}

		//qDebug() << parts.count() << "partitions";

		// create the NiSkinPartition if it doesn't exist yet

		if (iSkinPart == NULL) {
			iSkinPart = new NiSkinPartition(); // nif->insertNiBlock("NiSkinPartition", nif->getBlockNumber(iSkinData) + 1);
			iSkinInst->SetSkinPartition(iSkinPart);
			iSkinData->SetSkinPartition(iSkinPart);
			//nif->setLink(iSkinInst, "Skin Partition", nif->getBlockNumber(iSkinPart));
			//nif->setLink(iSkinData, "Skin Partition", nif->getBlockNumber(iSkinPart));
		}

		// start writing NiSkinPartition

		//nif->set<int>(iSkinPart, "Num Skin Partition Blocks", parts.count());
		vector<SkinPartition >& partitions = iSkinPart->GetPartition();
		partitions.resize(parts.size());
		//nif->updateArray(iSkinPart, "Skin Partition Blocks");

		vector<BodyPartList > iBSSkinInstPartData;

		if (iSkinInst->IsDerivedType(BSDismemberSkinInstance::TYPE)) {
			BSDismemberSkinInstanceRef temp_data = DynamicCast<BSDismemberSkinInstance>(iSkinInst);
			int nparts = temp_data->GetPartitions().size(); // nif->get<uint>(iSkinInst, "Num Partitions");
			iBSSkinInstPartData = temp_data->GetPartitions();

			// why is QList.count() signed? cast to squash warning
			if (nparts != parts.size()) {
				Log::Warn("BSDismemberSkinInstance partition count does not match Skin Partition count.  Adjusting to fit.");
				iBSSkinInstPartData.resize(parts.size());
			}
		}

		list<int> prevPartBones;

		vector<SkinPartition > pparts = iSkinPart->GetSkinPartitionBlocks();
		pparts.resize(parts.size());

		for (int p = 0; p < parts.size(); p++) {
			
			auto& iPart = pparts[p]; //nif->getIndex(iSkinPart, "Skin Partition Blocks").child(p, 0);

			list<int> bones = parts[p].bones;
			bones.sort(less<int>());
			//sort(bones.begin(), bones.end(), less<int>());

			// set partition flags for bs skin instance if present
			if (!iBSSkinInstPartData.empty() && p < iBSSkinInstPartData.size()) {
				if (bones != prevPartBones) {
					prevPartBones = bones;
					iBSSkinInstPartData[p].partFlag = (BSPartFlag)257;
					//nif->set<uint>(iBSSkinInstPartData.child(p, 0), "Part Flag", 257);
				}
			}

			vector<Triangle> triangles = parts[p].triangles;

			// Create the vertex map

			int idx = 0;
			vector<int> vidx(numVerts, -1);
			for (const Triangle& tri : triangles) {
				for (int t = 0; t < 3; t++) {
					int v = tri[t];

					if (vidx[v] < 0)
						vidx[v] = idx++;
				}
			}
			vector<unsigned short> vertices(idx, -1);

			for (int i = 0; i < numVerts; ++i) {
				int v = vidx[i];

				if (v >= 0) {
					vertices[v] = i;
				}
			}

			// map the vertices

			for (int tri = 0; tri < triangles.size(); tri++) {
				for (int t = 0; t < 3; t++) {
					vector<unsigned short>::iterator vit = find(vertices.begin(), vertices.end(), triangles[tri][t]);
					triangles[tri][t] = distance(vertices.begin(), vit); //  vertices.indexOf(triangles[tri][t]);
				}
			}

			// stripify the triangles
			//vector<vector<unsigned short> > strips;
			int numTriangles = 0;

			//if (make_strips == true) {
			//	strips = stripify(triangles);

			//	for (const QVector<quint16>& strip : strips) {
			//		numTriangles += strip.count() - 2;
			//	}
			//}
			//else {
				numTriangles = triangles.size();
			//}

			// fill in counts
			if (pad) {
				while (bones.size() < maxBonesPerPartition) {
					bones.push_back(0);
				}
			}

			// resort the bone weights in bone order
			vector<vector<boneweight> >::iterator it = weights.begin();
			//QMutableVectorIterator<QList<boneweight> > it(weights);

			while (it != weights.end()) {
				vector<boneweight> & bw = *it;
				//check integrity
				std::set<int> vertices_bw;
				vector<boneweight>::iterator bwit = bw.begin();
				while (bwit != bw.end())
				{
					if (bwit->second == 0.0 || !vertices_bw.insert(bwit->first).second)
					{
						bwit = bw.erase(bwit);
					}
					else {
						bwit++;
					}
				}

				std::sort(bw.begin(), bw.end(), boneweight_equivalence());
				it++;
			}

			iPart.numVertices = vertices.size();
			iPart.numTriangles = numTriangles;
			iPart.numBones = bones.size();
			iPart.numStrips = 0; //strips.count();
			iPart.numWeightsPerVertex = maxBones;

			//nif->set<int>(iPart, "Num Vertices", vertices.count());
			//nif->set<int>(iPart, "Num Triangles", numTriangles);
			//nif->set<int>(iPart, "Num Bones", bones.count());
			//nif->set<int>(iPart, "Num Strips", strips.count());
			//nif->set<int>(iPart, "Num Weights Per Vertex", maxBones);

			// fill in bone map
			vector<unsigned short> bbones;
			bbones.insert(bbones.end(), bones.begin(), bones.end());
			iPart.bones = bbones;
			//if (!iPart.bones.empty())
			//	iPart.bones.clear();
			//for (std::list<int>::iterator bit = bones.begin(); bit != bones.end(); ++bit)
			//	iPart.bones.push_back(*bit);
			//iPart.bones.insert(iPart.bones.end(), bones.begin(), bones.end());
			//QModelIndex iBoneMap = nif->getIndex(iPart, "Bones");
			//nif->updateArray(iBoneMap);
			//nif->setArray<int>(iBoneMap, bones.toVector());

			// fill in vertex map
			iPart.hasVertexMap = true;
			iPart.vertexMap = vertices;
			//nif->set<int>(iPart, "Has Vertex Map", 1);
			//QModelIndex iVertexMap = nif->getIndex(iPart, "Vertex Map");
			//nif->updateArray(iVertexMap);
			//nif->setArray<int>(iVertexMap, vertices);

			// fill in vertex weights
			iPart.hasVertexWeights = true;

			//nif->set<int>(iPart, "Has Vertex Weights", 1);
			//QModelIndex iVWeights = nif->getIndex(iPart, "Vertex Weights");
			//nif->updateArray(iVWeights);
			iPart.vertexWeights.resize(vertices.size());

			for (int v = 0; v < iPart.vertexWeights.size(); v++) {
				//QModelIndex iVertex = iVWeights.child(v, 0);
				//nif->updateArray(iVertex);
				vector<float>& actual = iPart.vertexWeights[v];
				actual.resize(maxBones);
				vector<boneweight> list = weights[vertices[v]];

				for (int b = 0; b < maxBones; b++)
					actual[b] = list.size() > b ? list[b].second : 0.0;
					//nif->set<float>(iVertex.child(b, 0),);
			}

			iPart.hasFaces = true;

			//nif->set<int>(iPart, "Has Faces", 1);

			if (make_strips == true) {
				////Clear out any existing triangle data that might be left over from an existing Skin Partition
				//QModelIndex iTriangles = nif->getIndex(iPart, "Triangles");
				//nif->updateArray(iTriangles);

				//// write the strips
				//QModelIndex iStripLengths = nif->getIndex(iPart, "Strip Lengths");
				//nif->updateArray(iStripLengths);

				//for (int s = 0; s < nif->rowCount(iStripLengths); s++)
				//	nif->set<int>(iStripLengths.child(s, 0), strips.value(s).count());

				//QModelIndex iStrips = nif->getIndex(iPart, "Strips");
				//nif->updateArray(iStrips);

				//for (int s = 0; s < nif->rowCount(iStrips); s++) {
				//	nif->updateArray(iStrips.child(s, 0));
				//	nif->setArray<quint16>(iStrips.child(s, 0), strips.value(s));
				//}
			}
			else {
				//Clear out any existing strip data that might be left over from an existing Skin Partition
				//QModelIndex iStripLengths = nif->getIndex(iPart, "Strip Lengths");
				//nif->updateArray(iStripLengths);
				iPart.stripLengths.clear();
				iPart.strips.clear();

				//QModelIndex iStrips = nif->getIndex(iPart, "Strips");
				//nif->updateArray(iStrips);

				iPart.triangles = triangles;

				//QModelIndex iTriangles = nif->getIndex(iPart, "Triangles");
				//nif->updateArray(iTriangles);
				//nif->setArray<Triangle>(iTriangles, triangles);
			}

			// fill in vertex bones

			//nif->set<int>(iPart, "Has Bone Indices", 1);
			iPart.hasBoneIndices = true;

			//auto& iVBones = iPart.boneIndices; // nif->getIndex(iPart, "Bone Indices");
			//nif->updateArray(iVBones);
			iPart.boneIndices.resize(vertices.size());

			for (int v = 0; v < iPart.boneIndices.size(); v++) {
				//QModelIndex iVertex = iVBones.child(v, 0);
				//nif->updateArray(iVertex);

				vector<Niflib::byte>& actual = iPart.boneIndices[v];
				actual.resize(maxBones);
				vector<boneweight> list = weights[vertices[v]];

				for (int b = 0; b < maxBones; b++)
				{
					actual[b] = list.size() > b ? distance(bones.begin(), find(bones.begin(), bones.end(), list[b].first)) : 0;
				}
			}
		}

		// done

		iSkinPart->SetSkinPartitionBlocks(pparts);

		if (iShapeType == "NiTriStrips")
		{
			//copy partitions out
		}

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