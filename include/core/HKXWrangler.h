#pragma once

#include "stdafx.h"

#include <commands/Project.h>

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/Reflection/Registry/hkDynamicClassNameRegistry.h>

// Scene
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkLoader.h>

#include <obj\NiNode.h>
#include <obj\NiSequence.h>
#include <obj\bhkRigidBody.h>
#include <obj\NiTrishape.h>
#include <obj\bhkShape.h>
#include <nif_math.h>

#include <fbxsdk.h>

#include <cstdio>
#include <sys/stat.h>
#include <map>
#include <filesystem>
namespace fs = std::experimental::filesystem;

// Serialize
#include <Common/Serialize/Util/hkSerializeUtil.h>

#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Animation/Animation/Animation/hkaAnimation.h>
#include <Animation/Animation/Animation/hkaAnimationBinding.h>
#include <Physics/Collide/Shape\hkpShape.h>
#include <Physics\Dynamics\Entity\hkpRigidBody.h>
#include <Common\GeometryUtilities\Inertia\hkInertiaTensorComputer.h>

bool isShapeFbxNode(FbxNode* node);

template<typename PropertyType, typename input>
void set_property(FbxObject* material, const char* name, input value, PropertyType T)
{
	FbxProperty p = material->FindProperty(name);
	if (!p.IsValid())
	{
		p = FbxProperty::Create(material, T, name);
		p.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
	}
	p.Set(value);
};

template<typename Output>
Output get_property(FbxObject* material, const char* name)
{
	FbxProperty p = material->FindProperty(name);
	if (p.IsValid())
	{
		return p.Get<Output>();
	}
	return Output();
};


namespace ckcmd {
	namespace HKX {



#define BEHAVIORS_SUBFOLDER "behaviors"
#define CHARACTERS_SUBFOLDER "characters"
#define ASSETS_SUBFOLDER "assets"
#define ANIMATIONS_SUBFOLDER "animations"

		using namespace std;
		using namespace Niflib;

		class HKXWrapper {
			string out_name;
			string out_path;
			string out_path_abs;
			string prefix;

			hkaSkeleton* skeleton = NULL;

			map<fs::path, hkRootLevelContainer> out_data;
			map<string, int> float_map;

			void write(hkRootLevelContainer& rootCont, string subfolder = "", string name = "");

			hkRootLevelContainer* read(const fs::path& path, hkArray<hkVariant>& objects);
			hkRootLevelContainer* read(const fs::path& path);
			hkRootLevelContainer* read(const uint8_t* data, const size_t& size, hkArray<hkVariant>& objects);
			hkRootLevelContainer* read(const uint8_t* data, const size_t& size);

			//hkRefPtr<hkbProjectData> load_project(const fs::path& path);
			//hkRefPtr<hkbProjectData> load_project(const uint8_t* data, const size_t& size);

			//void retarget_project(hkRootLevelContainer* root, hkRefPtr<hkbProjectData>, const string& output_project_name, const fs::path& output_dir)
			//{

			//}

			void create_project(const set<string>& havok_sequences_names = {});

			void create_character(const set<string>& havok_sequences_names);

			void create_skeleton();

			void create_behavior(const set<string>& kf_sequences_names, const set<string>& havok_sequences_names);

			vector<FbxNode*> add(hkaSkeleton* skeleton, FbxNode* root, vector<FbxProperty>& float_tracks);
			void add(const string& name, hkaAnimation* animation, hkaAnimationBinding* binding, vector<FbxNode*>& ordered_skeleton, vector<FbxProperty>& float_tracks);

		public:

			HKXWrapper() {}
			HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix);
			HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix, const set<string>& sequences_names);

			template<typename hkRootType>
			hkRefPtr<hkRootType> load(const fs::path& path, hkRootLevelContainer* root) {
				root = read(path);
				return root->findObject<hkRootType>();
			}

			template<typename hkRootType>
			hkRefPtr<hkRootType> load(const uint8_t* data, const size_t& size, hkRootLevelContainer*& root) {
				root = read(data, size);
				return root->findObject<hkRootType>();
			}

			template<typename hkRootType>
			hkRefPtr<hkRootType> load(const uint8_t* data, const size_t& size, hkRootLevelContainer*& root, hkArray<hkVariant>& objects) {
				root = read(data, size, objects);
				return root->findObject<hkRootType>();
			}

			inline string GetPath() { return out_path + "\\" + out_name + "\\" + out_name + ".hkx"; }

			vector<string> read_track_list(const fs::path& path, string& skeleton_name = string(""), string& root_name = string(""), vector<string>& floats = vector<string>());

			//gives back the ordered bone array as written in the skeleton file
			vector<FbxNode*> create_skeleton(const string& name, const set<FbxNode*>& bones, FbxNode* root = NULL);

			void create_skeleton(FbxNode* bone);
			void add_bone(FbxNode* bone);

			set<string> create_animations(
				const string& skeleton_name,
				vector<FbxNode*>& skeleton,
				set<FbxAnimStack*>& animations,
				FbxTime::EMode timeMode,
				const vector<uint32_t>& transform_track_to_bone_indices = {},
				set<FbxProperty>& annotations = set<FbxProperty>(),
				vector<FbxProperty>& floats = vector<FbxProperty>(),
				const vector<uint32_t>& transform_track_to_float_indices = {}
			);

			vector<FbxNode*> load_skeleton(const fs::path& path, FbxNode* scene_root, vector<FbxProperty>& float_tracks);
			void load_animation(const fs::path& path, vector<FbxNode*>&, vector<FbxProperty>& float_tracks);

			void write_animations(const string& out_path, const set<string>& havok_sequences_names);
		
			string write_project(const string& out_name, const string& out_path, const string& out_path_abs,
				const string& prefix, const set<string>& kf_sequences_names, const set<string>& havok_sequences_names);

			static hkRefPtr<hkpRigidBody> build_body(FbxNode* body, set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes);
			void build_skeleton_from_ragdoll();
			static hkRefPtr<hkpConstraintInstance> build_constraint(FbxNode* body);
			static hkRefPtr<hkpRigidBody> check_body(bhkRigidBodyRef body, vector<pair<hkTransform, NiTriShapeRef>>& geometry_meshes);
			static hkRefPtr<hkpShape> build_shape(FbxNode* shape_root, set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes, hkpMassProperties& properties, double scale_factor, FbxNode* body, hkpRigidBodyCinfo& hk_body);
			static hkRefPtr<hkpShape> check_shape(bhkShapeRef shape_root, bhkRigidBodyRef bhkBody, vector<pair<hkTransform, NiTriShapeRef>>& geometry_meshes, hkpMassProperties& properties, double scale_factor, hkpRigidBodyCinfo& hk_body);

		};

		typedef map<set<string>, HKXWrapper> wrap_map;

		class HKXWrapperCollection {
			wrap_map wrappers;
		public:
			string wrap(const string& out_name, 
				const string& out_path, 
				const string& out_path_root, 
				const string& prefix, const 
				set<string>& sequences_names);
		};
	}
}