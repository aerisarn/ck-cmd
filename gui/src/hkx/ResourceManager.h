#pragma once


#include <core/HKXWrangler.h>
#include <src/Log.h>
#include <src/workspace.h>
#include <core/AnimationCache.h>

#include <map>

typedef std::pair< hkVariant, std::vector<hkVariant>> hkx_file_t;

struct Collection;
struct hkVariant;
class hkbCharacterStringData;
class hkbProjectStringData;

namespace Sk {
	class AACTRecord;
	class IDLERecord;
}

namespace ckcmd {
	namespace HKX {

		typedef std::vector<std::pair<int, const hkVariant*>> hk_object_list_t;

		enum class ProjectType {
			invalid = 0,
			character = 1,
			misc = 2
		};

		class ResourceManager {

			std::map<size_t, fs::path> _files;
			std::map<size_t, hkx_file_t> _contents;
			std::map<size_t, string> _sanitized_names;
			QStringList _characters;
			QStringList _miscellaneous;
			WorkspaceConfig& _workspace;
			//TOO CUMBERSOME! Let's create our projects with blackjack and hookers
			AnimationCache _cache;
			//project_file, crc32(name) -> translations, rotations
			std::map<std::pair<size_t, long long>, AnimData::root_movement_t> _animations_root_movements;
			//project_file, crc32(set) -> crc32(anim_name) 
			std::multimap<std::pair<size_t, long long>, long long> _decoded_loaded_sets;
			
			//IDLES
			Collection* _esp;

			const std::string& get_sanitized_name(int file_index);
			void scanWorkspace();
			const QString& projectPath(int row, ProjectType type);
			CacheEntry* findCacheEntry(const std::string& sanitized_name);
			CacheEntry* findCacheEntry(size_t project_index);

			bool isHavokProject(const fs::path& file);
			bool isHavokAnimation(const fs::path& file);
			std::array<std::string, 4>  animationCrc32(const fs::path& path);
			bool isCreatureProject(int file_index);
			hkbProjectStringData* getProjectRoot(int file_index);
			hkbProjectStringData* getProjectRoot(const fs::path& fs_path);
			hkbCharacterStringData* getCharacterString(int character_index);

		public:

			ResourceManager(WorkspaceConfig& _workspace);

			~ResourceManager();

			WorkspaceConfig& workspace() { return _workspace; }

			hkx_file_t& get(size_t index);
			hkx_file_t& get(const fs::path& file);

			std::set<Sk::AACTRecord*> actions();
			std::set<Sk::IDLERecord*> idles(size_t index);

			//fs::path ResourceManager::open(const std::string& project);

			size_t index(const fs::path& file) const;
			fs::path path(int file_index) const;
			bool is_open(const fs::path& file) const;

			int findIndex(int file_index, const void* object) const;
			int findIndex(const fs::path& file, const void* object) const;



			hkVariant* at(const fs::path& file, size_t _index);
			const hkVariant* at(const fs::path& file, size_t _index) const;
			hkVariant* ResourceManager::at(size_t file_index, size_t _index);
			const hkVariant* ResourceManager::at(size_t file_index, size_t _index) const;

			hk_object_list_t findCompatibleNodes(size_t file_index, const hkClassMember* member_class) const;
			
			void save(size_t file_index);
			void save_cache(int project_index);

			size_t character_project_files() { return _characters.size(); }
			size_t miscellaneous_project_files() { return _miscellaneous.size(); }

			const QString& character_project_file(int index) { return _characters.at(index); }
			const QString& miscellaneous_project_file(int index) { return _miscellaneous.at(index); }

			void close(int file_index);

			bool isProjectFileOpen(int row, ProjectType type);
			void openProjectFile(int row, ProjectType type);
			void closeProjectFile(int row, ProjectType type);
			void saveProject(int row, ProjectType type);
			size_t projectCharacters(int project_index);
			size_t projectFileIndex(int row, ProjectType type);


			size_t characterFileIndex(int row, int project_file, ProjectType type);
			hkVariant* characterFileRoot(int character_index);

			size_t hasRigAndRagdoll(int project_file, hkbCharacterStringData* string_data);
			size_t getRigIndex(int project_file, hkbCharacterStringData* string_data);
			hkVariant* getRigRoot(int project_file, int rig_index);
			size_t getRagdollIndex(int project_file, const std::string& path);
			hkVariant* getRagdollRoot(int project_file, int rig_index);

			size_t behaviorFileIndex(int project_file, hkVariant* data);
			hkVariant* behaviorFileRoot(int behavior_file);
		};
	}
}