#pragma once


#include <core/HKXWrangler.h>
#include <src/Log.h>
#include <src/hkx/ITreeBuilderClassHandler.h>
#include <src/hkx/ISpecialFieldsHandler.h>
#include <src/workspace.h>
#include <core/AnimationCache.h>

#include <map>

typedef std::pair< hkVariant, std::vector<hkVariant>> hkx_file_t;

struct Collection;
struct hkVariant;
class hkbCharacterStringData;

namespace Sk {
	class AACTRecord;
	class IDLERecord;
}

namespace ckcmd {
	namespace HKX {

		typedef std::vector<std::pair<int, const hkVariant*>> hk_object_list_t;

		class ResourceManager {

			std::vector<fs::path> _files;
			std::map<size_t, hkx_file_t> _contents;
			std::map<size_t, string> _sanitized_names;
			QStringList _characters;
			QStringList _miscellaneous;
			WorkspaceConfig& _workspace;
			AnimationCache _cache;
			Collection* _esp;

			const std::string& get_sanitized_name(int file_index);
			void scanWorkspace();

		public:

			ResourceManager(WorkspaceConfig& _workspace);

			~ResourceManager();

			WorkspaceConfig& workspace() { return _workspace; }

			hkx_file_t& get(size_t index);
			hkx_file_t& get(const fs::path& file);
			void save(size_t index);

			std::set<Sk::AACTRecord*> actions();
			std::set<Sk::IDLERecord*> idles(size_t index);

			//fs::path ResourceManager::open(const std::string& project);

			size_t index(const fs::path& file) const;
			fs::path path(int file_index) const;

			int findIndex(int file_index, const void* object) const;
			int findIndex(const fs::path& file, const void* object) const;

			bool isHavokProject(const fs::path& file);

			hkVariant* at(const fs::path& file, size_t _index);
			const hkVariant* at(const fs::path& file, size_t _index) const;
			hkVariant* ResourceManager::at(size_t file_index, size_t _index);
			const hkVariant* ResourceManager::at(size_t file_index, size_t _index) const;


			hk_object_list_t findCompatibleNodes(size_t file_index, const hkClassMember* member_class) const;

			bool isCreatureProject(int file_index);
			CacheEntry* findCacheEntry(const std::string& sanitized_name);
			CacheEntry* findOrCreateCacheEntry(size_t file_index, bool character);
			CacheEntry* findCacheEntry(size_t file_index);
			const AnimData::ClipMovementData& getMovement(size_t file_index, string clip);
			vector<AnimationCache::event_info_t> getEventsInfo(size_t file_index, string anim_event);
			void save_cache(int file_index);

			size_t character_project_files() { return _characters.size(); }
			size_t miscellaneous_project_files() { return _miscellaneous.size(); }

			const QString& character_project_file(int index) { return _characters.at(index); }
			const QString& miscellaneous_project_file(int index) { return _miscellaneous.at(index); }

			bool isCharacterFileOpen(int row);
			size_t projectFileIndex(int row);
			size_t characterFileIndex(int row);
			hkVariant* characterFileRoot(int character_index);
			void openCharacterFile(int row);
			void closeCharacterFile(int row);
			size_t hasRigAndRagdoll(int project_file, hkbCharacterStringData* string_data);
			size_t getRigIndex(int project_file, hkbCharacterStringData* string_data);
			hkVariant* getRigRoot(int project_file, int rig_index);
			size_t getRagdollIndex(int project_file, const std::string& path);
			hkVariant* getRagdollRoot(int project_file, int rig_index);

			bool isMiscFileOpen(int row);
			size_t miscFileIndex(int row);
			void openMiscFile(int row);
			void closeMiscFile(int row);

			size_t behaviorFileIndex(int project_file, hkVariant* data);
			hkVariant* behaviorFileRoot(int behavior_file);
		};
	}
}