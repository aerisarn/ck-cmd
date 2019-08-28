/*
BodySlide and Outfit Studio
Copyright (C) 2018  Caliente & ousnius
See the included LICENSE file
*/

#pragma once

#include <stdafx.h>

#include <core\NifFile.h>
#include <core/HKXWrangler.h>

#include <fbxsdk.h>
#include <unordered_map>

#include <obj\NiNode.h>
#include <nif_math.h>
#include <set>

namespace ckcmd {
namespace FBX {

	struct FBXImportOptions {
		bool InvertU = false;
		bool InvertV = false;
	};


	using namespace std;
	using namespace Niflib;
	using namespace ckcmd::NIF;
	using namespace ckcmd::HKX;

	class FBXWrangler {
	private:
		FbxManager * sdkManager = nullptr;
		FbxScene* scene = nullptr;
		string exporter_name = "";
		string exporter_version = "";
		HKXWrapper hkxWrapper;

		string comName;
		void importShapes(NiNodeRef parent, FbxNode* child, const FBXImportOptions& options);

		map<NiAVObjectRef, NiAVObjectRef> conversion_parent_Map;
		map<FbxNode*, NiObjectRef> conversion_Map;
		map<FbxMesh*, NiTriShapeRef> skins_Map;
		set<FbxMesh*> meshes;

		set<FbxNode*> skinned_bones;
		set<FbxNode*> unskinned_bones;
		set<FbxNode*> animated_nodes;
		set<FbxProperty> annotated;
		set<FbxProperty> float_properties;

		set<FbxAnimStack*> skinned_animations;
		set<FbxAnimStack*> unskinned_animations;

		NiNodeRef conversion_root;
		set<string> sequences_names;
		set<string> havok_sequences;

		set<FbxNode*> physic_entities;

		bool export_skin = false;
		bool export_rig = false;

		string external_skeleton_path = "";

		NiTriShapeRef importShape(FbxNodeAttribute* node, const FBXImportOptions& options);
		
		set<FbxNode*> FBXWrangler::buildBonesList();
		void checkAnimatedNodes();
		void buildKF();
		void buildCollisions();
		bhkShapeRef FBXWrangler::convert_from_hk(const hkpShape* shape, bhkCMSDMaterial& aggregate_layer);
		NiCollisionObjectRef build_physics(FbxNode* rigid_body, set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes);
		double convert(FbxAnimLayer* pAnimLayer, NiControllerSequenceRef sequence, set<NiObjectRef>& targets, NiControllerManagerRef manager, NiMultiTargetTransformControllerRef multiController, string accum_name, double last_start, double last_stop);
		void convertSkins(FbxMesh* m, NiTriShapeRef shape);
		FbxNode* find_animated_parent(FbxNode* rigid_body);
	public:
		FBXWrangler();
		~FBXWrangler();

		string texture_path;

		void setExternalSkeletonPath(const string& external_skeleton_path);
		vector<FbxNode*> importExternalSkeleton(const string& external_skeleton_path, vector<FbxProperty>& float_tracks);
		void importAnimationOnSkeleton(const string& external_skeleton_path, vector<FbxNode*>& skeleton, vector<FbxProperty>& float_tracks);
		bool SaveAnimation(const string& fileName);

		void NewScene();
		void CloseScene();

		//void GetShapeNames(vector<string>& outNames) {
		//	for (auto &s : shapes)
		//		outNames.push_back(s.first);
		//}

		//FBXShape* GetShape(const string& shapeName) {
		//	return &(shapes[shapeName]);
		//}

		//void AddSkeleton(NifFile* nif, bool onlyNonSkeleton = false);

		//// Recursively add bones to the skeleton in a depth-first manner
		//FbxNode* AddLimb(NifFile* nif, NiNode* nifBone);
		//void AddLimbChildren(FbxNode* node, NifFile* nif, NiNode* nifBone);

		void AddNif(NifFile& nif);
		//void AddSkinning(AnimInfo* anim, const std::string& shapeName = "");
		//void AddGeometry(const std::string& shapeName,
		//	const std::vector<Vector3>& verts,
		//	const std::vector<Vector3>& norms,
		//	const std::vector<Triangle>& tris,
		//	const std::vector<TexCoord>& uvs);

		bool ExportScene(const std::string& fileName);
		bool ImportScene(const std::string& fileName, const FBXImportOptions& options = FBXImportOptions());

		bool LoadMeshes(const FBXImportOptions& options);
		bool SaveNif(const string& fileName);
		bool SaveSkin(const string& fileName);
	};	
	
} 
}
