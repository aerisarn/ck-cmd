/*
BodySlide and Outfit Studio
Copyright (C) 2018  Caliente & ousnius
See the included LICENSE file
*/

#pragma once

#include <stdafx.h>
#include <unordered_map>
#include <set>
#include <algorithm>

#include <VHACD.h>
#include <boundingmesh.h>

#include <commands/Geometry.h>
#include <core/EulerAngles.h>
#include <core/MathHelper.h>
#include <core/HKXWrangler.h>
#include <core/NifFile.h>

//#include <obj\NiNode.h>
//#include <nif_math.h>

#include <fbxsdk.h>

namespace ckcmd {
namespace FBX {

	struct FBXImportOptions {
		bool InvertU = false;
		bool InvertV = true;
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
		map<FbxMesh*, map<size_t, size_t>> vertex_Map;
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

		vector<FbxNode*> physic_entities;

		bool export_skin = false;
		bool export_rig = false;

		string external_skeleton_path = "";

		NiTriShapeRef importShape(FbxNodeAttribute* node, const FBXImportOptions& options);
		
		set<FbxNode*> FBXWrangler::buildBonesList();
		void checkAnimatedNodes();
		void handleInlineTracks(FbxProperty& track, NiNode& parent, const string& ed_name);
		void handleVisibility(FbxProperty& track, NiNode& parent);
		bool hasCurve(FbxProperty& track);
		void buildKF();
		void buildCollisions();
		void buildConstraints();
		bhkSerializableRef convert_from_hk(const hkpConstraintInstance* constraint, const bhkRigidBodyRef entity_a, const bhkRigidBodyRef entity_b); bhkShapeRef convert_from_hk(const hkpShape* shape, bhkCMSDMaterial& aggregate_layer);
		NiCollisionObjectRef build_physics(FbxNode* rigid_body, set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes);
		double convert(FbxAnimLayer* pAnimLayer, NiControllerSequenceRef sequence, set<NiObjectRef>& targets, NiControllerManagerRef manager, NiMultiTargetTransformControllerRef multiController, string accum_name, double last_start, double last_stop);
		void convertSkins(FbxMesh* m, NiTriShapeRef shape, const map<size_t,size_t>& cp);
		FbxNode* find_animated_parent(FbxNode* rigid_body);
	public:
		FBXWrangler();
		~FBXWrangler();

		string texture_path;

		HKXWrapper& hkx_wrapper() { return hkxWrapper; }

		void setExternalSkeletonPath(const string& external_skeleton_path);
		vector<FbxNode*> importExternalSkeleton(const string& external_skeleton_path, vector<FbxProperty>& float_tracks);
		void importAnimationOnSkeleton(const string& external_skeleton_path, vector<FbxNode*>& skeleton, vector<FbxProperty>& float_tracks, RootMovement& root_movements);
		map<fs::path, RootMovement>& SaveAnimation(const string& fileName);


		void NewScene();
		void CloseScene();

		void setExportRig(bool _export_rig = true) { export_rig = _export_rig; }
		void setExportSkin(bool _export_skin = true) { export_skin = _export_skin; }

		void AddNif(NifFile& nif);
		void ApplySkeletonScaling(NifFile& nif);

		bool ExportScene(const std::string& fileName);
		bool ImportScene(const std::string& fileName, const FBXImportOptions& options = FBXImportOptions());

		bool LoadMeshes(const FBXImportOptions& options);
		bool SaveNif(const string& fileName);
		bool SaveSkin(const string& fileName);
	};	
	
} 
}
