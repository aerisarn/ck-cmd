#pragma once

#include <cstdio>
#include <sys/stat.h>
#include <map>
#include <filesystem>

#include "stdafx.h"

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



#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

// Serialize
#include <Common/Serialize/Util/hkSerializeUtil.h>

#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
#include <Animation/Animation/Animation/hkaAnimation.h>
#include <Animation/Animation/Animation/hkaAnimationBinding.h>
#include <Physics/Collide/Shape\hkpShape.h>
#include <Physics\Dynamics\Entity\hkpRigidBody.h>
#include <Common\GeometryUtilities\Inertia\hkInertiaTensorComputer.h>
#include <Physics\Utilities\Serialize\hkpPhysicsData.h>

#include <core/AnimationCache.h>

bool isShapeFbxNode(FbxNode* node);
void to_upper(string& name);

template<typename T> 
void setKeyProperty(FbxObject* material, FbxProperty& p, T value) {}

template<>
void setKeyProperty(FbxObject* material, FbxProperty& p, float value);

template<typename PropertyType, typename input>
FbxProperty set_property(FbxObject* material, const char* name, input value, PropertyType T)
{
	FbxProperty p = material->FindProperty(name);
	if (!p.IsValid())
	{
		p = FbxProperty::Create(material, T, name);
		p.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
	}
	p.Set(value);
	return p;
};

void camel(string& name);
template<typename Output>
Output get_property(FbxObject* material, const char* name, const Output& default_value = Output())
{
	FbxProperty p = material->FindProperty(name);
	if (p.IsValid())
	{
		return p.Get<Output>();
	}
	return default_value;
};


namespace ckcmd {
	namespace HKX {



#define BEHAVIORS_SUBFOLDER "behaviors"
#define CHARACTERS_SUBFOLDER "characters"
#define ASSETS_SUBFOLDER "assets"
#define ANIMATIONS_SUBFOLDER "animations"

		using namespace std;
		using namespace Niflib;

		struct RootMovement
		{
			float duration = 0.;

			vector<tuple<hkReal, hkVector4>> translations;
			vector<tuple<hkReal, ::hkQuaternion>> rotations;
			vector<tuple<hkReal, string>> events;

			vector<float> getData(string data);
			AnimData::StringListBlock getClipEvents() const;
			AnimData::StringListBlock getClipTranslations() const;
			AnimData::StringListBlock getClipRotations() const;

			RootMovement() {}
			RootMovement(
				float duration,
				const std::vector<std::string>& in_translations,
				const std::vector<std::string>& in_rotations,
				const std::vector<std::string>& in_events);

			bool IsValid() const { return !translations.empty() || !rotations.empty() || !events.empty(); }
			bool HasMovements() const { return !translations.empty() || !rotations.empty(); }
		};

		class HKXWrapper {
		public:
			enum DefaultBehaviors {
				autoplay_with_transitions = 0,
				equip_forward_unequip = 1
			};
		private:
			string out_name;
			string out_path;
			string out_path_abs;
			string prefix;

			hkaSkeleton* skeleton = NULL;

			hkaSkeleton* animation_skeleton = NULL;
			hkaSkeleton* ragdoll_skeleton = NULL;
			hkpPhysicsData* physics_data = NULL;
			hkaSkeletonMapper* animation_to_ragdoll_mapper = NULL;

			map<fs::path, hkRootLevelContainer> out_data;
			map<fs::path, RootMovement> out_root_data;
			map<string, int> float_map;

			void write(hkRootLevelContainer& rootCont, string subfolder = "", string name = "");



			//hkRefPtr<hkbProjectData> load_project(const fs::path& path);
			//hkRefPtr<hkbProjectData> load_project(const uint8_t* data, const size_t& size);

			//void retarget_project(hkRootLevelContainer* root, hkRefPtr<hkbProjectData>, const string& output_project_name, const fs::path& output_dir)
			//{

			//}

			void create_project(const set<string>& havok_sequences_names = {});

			void create_character(const set<string>& havok_sequences_names);

			void create_skeleton();

			void create_behavior(const set<string>& kf_sequences_names, const set<string>& havok_sequences_names);
			void create_behavior(const DefaultBehaviors& kf_sequences_names);

			vector<FbxNode*> add(hkaSkeleton* skeleton, FbxNode* root, vector<FbxProperty>& float_tracks);
			void add(const string& name, hkaAnimation* animation, hkaAnimationBinding* binding, vector<FbxNode*>& ordered_skeleton, vector<FbxProperty>& float_tracks, RootMovement& root_movements);

		public:

			HKXWrapper() {}
			HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix);
			HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix, const set<string>& sequences_names);
			HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix, const DefaultBehaviors& sequences_names);


			hkRootLevelContainer* read(const fs::path& path, hkArray<hkVariant>& objects);
			hkRootLevelContainer* read(const fs::path& path);
			hkRootLevelContainer* read(const uint8_t* data, const size_t& size, hkArray<hkVariant>& objects);
			hkRootLevelContainer* read(const uint8_t* data, const size_t& size);

			template<typename hkRootType>
			hkRefPtr<hkRootType> load(const fs::path& path, hkRootLevelContainer*& root) {
				root = read(path);
				return root->findObject<hkRootType>();
			}

			template<typename hkRootType>
			hkRefPtr<hkRootType> load(const fs::path& path, hkRootLevelContainer*& root, hkArray<hkVariant>& objects) {
				root = read(path, objects);
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

			inline string GetGenericPath() { return out_path + "\\" + out_name + ".hkx"; }

			vector<string> read_track_list(const fs::path& path, string& skeleton_name = string(""), string& root_name = string(""), vector<string>& floats = vector<string>());

			//gives back the ordered bone array as written in the skeleton file
			vector<FbxNode*> create_skeleton(const string& name, const set<FbxNode*>& bones, FbxNode* root = NULL);

			void create_skeleton(FbxNode* bone);
			void add_bone(FbxNode* bone);
			int setExternalSkeletonPose(FbxNode* body);

			set<string> create_animations(
				const string& skeleton_name,
				vector<FbxNode*>& skeleton,
				set<FbxAnimStack*>& animations,
				FbxTime::EMode timeMode,
				const vector<uint32_t>& transform_track_to_bone_indices = {},
				set<FbxProperty>& annotations = set<FbxProperty>(),
				vector<FbxProperty>& floats = vector<FbxProperty>(),
				const vector<uint32_t>& transform_track_to_float_indices = {},
				bool extract_motion = true,
				RootMovement& root_info = RootMovement()
			);

			vector<FbxNode*> load_skeleton(const fs::path& path, FbxNode* scene_root, vector<FbxProperty>& float_tracks);
			void load_animation(const fs::path& path, vector<FbxNode*>&, vector<FbxProperty>& float_tracks, RootMovement& root_movements);

			map<fs::path, RootMovement>& write_animations(const string& out_path, const set<string>& havok_sequences_names);
		
			string write_project(const string& out_name, const string& out_path, const string& out_path_abs,
				const string& prefix, const set<string>& kf_sequences_names, const set<string>& havok_sequences_names);

			static hkRefPtr<hkpRigidBody> build_body(FbxNode* body, set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes);
			std::string build_skeleton_from_ragdoll();
			static const set<tuple<FbxNode*, FbxNode*, hkpConstraintInstance*>>&  get_constraints_table();
			static hkRefPtr<hkpConstraintInstance> build_constraint(FbxNode* body);
			static hkRefPtr<hkpRigidBody> check_body(bhkRigidBodyRef body, vector<pair<hkTransform, NiTriShapeRef>>& geometry_meshes);
			static hkRefPtr<hkpShape> build_shape(FbxNode* shape_root, set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes, hkpMassProperties& properties, double scale_factor, FbxNode* body, hkpRigidBodyCinfo& hk_body);
			static hkRefPtr<hkpShape> check_shape(bhkShapeRef shape_root, bhkRigidBodyRef bhkBody, vector<pair<hkTransform, NiTriShapeRef>>& geometry_meshes, hkpMassProperties& properties, double scale_factor, hkpRigidBodyCinfo& hk_body);

			void GetStaticClipsMovements(
				vector<fs::path> animation_files,
				StaticCacheEntry& entry,
				const fs::path& behaviorFolder,
				std::map< fs::path, RootMovement>& map
			);

			void GetClipsMovements(
				vector<fs::path> animation_files,
				CacheEntry& entry,
				const fs::path& behaviorFolder,
				std::map< fs::path, RootMovement>& map
			); 
				
			void PutClipMovement(
				const fs::path& animation_file,
				CacheEntry& entry,
				const fs::path& behaviorFolder,
				const RootMovement& root_info
			);

			void write(hkRootLevelContainer* rootCont, const fs::path& out);
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


			string wrap(const string& out_name,
				const string& out_path,
				const string& out_path_root,
				const string& prefix, const
				HKXWrapper::DefaultBehaviors& behavior_type);
		};
	}
}