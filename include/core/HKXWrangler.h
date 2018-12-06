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
#include <nif_math.h>

#include <fbxsdk.h>

#include <cstdio>
#include <sys/stat.h>
#include <map>
#include <filesystem>
namespace fs = std::experimental::filesystem;

// Serialize
#include <Common/Serialize/Util/hkSerializeUtil.h>
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

			map<fs::path, hkRootLevelContainer> out_data;

			void write(hkRootLevelContainer& rootCont, string subfolder = "", string name = "");

			hkRootLevelContainer* read(const fs::path& path, hkArray<hkVariant>& objects);
			hkRootLevelContainer* read(const fs::path& path);
			hkRootLevelContainer* read(const uint8_t* data, const size_t& size, hkArray<hkVariant>& objects);
			hkRootLevelContainer* read(const uint8_t* data, const size_t& size);

			template<typename hkRootType>
			hkRefPtr<hkRootType> load(const fs::path& path, hkRootLevelContainer* root) {
				root = read(path);
				hkRefPtr<hkRootType> project;
				project = (hkRootType*)root->findObjectByType(project->getClassType()->getName());
				return project;
			}

			template<typename hkRootType>
			hkRefPtr<hkRootType> load(const uint8_t* data, const size_t& size, hkRootLevelContainer* root) {
				root = read(data, size);
				hkRefPtr<hkRootType> project;
				project = (hkRootType*)root->findObjectByType(project->getClassType()->getName());
				return project;
			}

			//hkRefPtr<hkbProjectData> load_project(const fs::path& path);
			//hkRefPtr<hkbProjectData> load_project(const uint8_t* data, const size_t& size);

			//void retarget_project(hkRootLevelContainer* root, hkRefPtr<hkbProjectData>, const string& output_project_name, const fs::path& output_dir)
			//{

			//}

			void create_project(const set<string>& havok_sequences_names = {});

			void create_character(const set<string>& havok_sequences_names);

			void create_skeleton();

			void create_behavior(const set<string>& kf_sequences_names, const set<string>& havok_sequences_names);

		public:

			HKXWrapper() {}
			HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix);
			HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix, const set<string>& sequences_names);

			inline string GetPath() { return out_path + "\\" + out_name + "\\" + out_name + ".hkx"; }

			vector<string> read_track_list(const fs::path& path, string& skeleton_name = string(""), string& root_name = string(""), vector<string>& floats = vector<string>());

			//gives back the ordered bone array as written in the skeleton file
			vector<FbxNode*> create_skeleton(const string& name, const set<FbxNode*>& bones, FbxNode* root = NULL);

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

			void write_animations(const string& out_path, const set<string>& havok_sequences_names);
		
			string write_project(const string& out_name, const string& out_path, const string& out_path_abs,
				const string& prefix, const set<string>& kf_sequences_names, const set<string>& havok_sequences_names);
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