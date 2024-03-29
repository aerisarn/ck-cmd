#pragma once


#include <core/HKXWrangler.h>
#include <src/Log.h>
#include <src/workspace.h>
#include <core/AnimationCache.h>

#include <map>
#include <list>

typedef std::pair< hkVariant, std::list<hkVariant>> hkx_file_t;

struct Collection;
struct hkVariant;
class hkbCharacterData;
class hkbCharacterStringData;
class hkbProjectStringData;
class hkbStateMachine;

namespace Sk {
	class AACTRecord;
	class IDLERecord;
}

#define HK_INVALID_REF -1

namespace ckcmd {
	namespace HKX {

		typedef std::vector<hkVariant*> hk_object_list_t;

		enum class ProjectType {
			invalid = 0,
			character = 1,
			misc = 2
		};

		enum class ExportType {
			invalid = 0,
			LE = 1,
			SE = 2
		};

		enum class AssetType {
			invalid = 0,
			project,
			skeleton,
			animation,
			behavior,
			bones,
			ragdoll_bones,
			events,
			variables,
			behavior_properties,
			variable_words,
			attack_events,
			clips,
			clip_animations,
			FSM_states,
			character_assets
		};

		class ResourceManager {

			std::map<size_t, fs::path> _files;
			std::map<size_t, hkx_file_t> _contents;
			std::map<size_t, string> _sanitized_names;
			std::vector<fs::path> _characters;
			std::vector<fs::path> _miscellaneous;
			QStringList _charactersNames;
			QStringList _miscellaneousNames;
			WorkspaceConfig& _workspace;
			//TOO CUMBERSOME! Let's create our projects with blackjack and hookers
			AnimationCache _cache;
			//project_file, crc32(name) -> translations, rotations
			std::map<std::pair<size_t, long long>, AnimData::root_movement_t> _animations_root_movements;
			//project_file, crc32(set) -> anim_name inside character data 
			std::multimap<std::pair<size_t, long long>, std::string> _decoded_loaded_sets;
			std::vector<const hkClass*> _concreate_classes;
			
			//IDLES
			Collection* _esp;

			const std::string& get_sanitized_name(int file_index);
			void scanWorkspace();
			const fs::path& projectPath(int row, ProjectType type);
			std::shared_ptr<CacheEntry> findCacheEntry(const std::string& sanitized_name);
			std::shared_ptr<CacheEntry> findCacheEntry(size_t project_index);

			bool isHavokProject(const fs::path& file);
			bool isHavokAnimation(const fs::path& file);
			std::array<std::string, 4>  animationCrc32(const fs::path& path);

			hkbProjectStringData* getProjectRoot(int file_index);
			hkbProjectStringData* getProjectRoot(const fs::path& fs_path);
			hkbCharacterStringData* getCharacterString(int character_index);
			hkbCharacterData* getCharacterData(int character_index);

		public:

			bool isCreatureProject(int project_index);

			ResourceManager(WorkspaceConfig& _workspace);

			~ResourceManager();

			WorkspaceConfig& workspace() { return _workspace; }

			hkx_file_t& get(size_t index);
			hkx_file_t& get(const fs::path& file);

			std::set<Sk::AACTRecord*> actions();
			std::set<Sk::IDLERecord*> idles(size_t index);

			//fs::path ResourceManager::open(const std::string& project);

			int index(const fs::path& file) const;
			fs::path path(int file_index) const;
			bool is_open(const fs::path& file) const;
			bool is_open(int file_index) const;

			hkVariant* findVariant(int file_index, const void* object);

			hk_object_list_t findCompatibleNodes(size_t file_index, const hkClassMember* member_class);
			hk_object_list_t findCompatibleNodes(size_t file_index, const hkClass* hkclass);
			std::vector<const hkClass*> findCompatibleClasses(const hkClass* hkclass) const;
			
			void save(size_t file_index);
			void save_cache(int project_index);

			size_t character_project_files() { return _characters.size(); }
			size_t miscellaneous_project_files() { return _miscellaneous.size(); }

			const QString& character_project_file(int index) { return _charactersNames.at(index); }
			const QString& miscellaneous_project_file(int index) { return _miscellaneousNames.at(index); }

			void close(int file_index);

			fs::path makeExportPath(const fs::path& absolute_file, const fs::path& export_folder);

			bool isProjectFileOpen(int row, ProjectType type);
			void openProjectFile(int row, ProjectType type);
			void closeProjectFile(int row, ProjectType type);
			void saveProject(int row, ProjectType type);
			void saveProject(int project_index);
			void exportProject(int row, ProjectType type, ExportType export_to);
			void exportProject(int project_index, ExportType export_to);
			size_t projectCharacters(int project_index);
			size_t projectFileIndex(int row, ProjectType type);

			/*MODEL FILES*/

			size_t characterFileIndex(int row, int project_file, ProjectType type);
			hkVariant* characterFileRoot(int character_index);

			size_t hasBehavior(int project_file, hkbCharacterStringData* string_data);
			size_t hasRigAndRagdoll(int project_file, hkbCharacterStringData* string_data);
			size_t getRigIndex(int project_file, hkbCharacterStringData* string_data);
			hkVariant* getRigRoot(int project_file, int rig_index);
			size_t getRagdollIndex(int project_file, const std::string& path);
			hkVariant* getRagdollRoot(int project_file, int rig_index);

			int behaviorFileIndex(int project_file, hkVariant* data);
			hkVariant* behaviorFileRoot(int behavior_file);

			const AnimData::root_movement_t& getAnimationMovementData(int project_file, const std::string& animation_name);
			void setAnimationMovementData(int project_file, const std::string& animation_name, const AnimData::root_movement_t& movement);

			/* NODES */
			template <typename T>
			T* createObject(int file, const hkClass* hkclass) {
				return static_cast<T*>(createObject(file, hkclass, ""));
			}

			void* createObject(int file, const hkClass* hkclass, const std::string& name);

			/* ASSETS */

			std::vector<fs::path> importAssets(int project_file, const fs::path& sourcePath, AssetType type);
			size_t assetsCount(int project_file, AssetType type);
			fs::path assetFolder(int project_file, AssetType type);
			void clearAssetList(int project_file, AssetType type);
			void refreshAssetList(int project_file, AssetType type);
			bool removeAsset(int file_index, AssetType type, int asset_index);

			QStringList assetsList(int project_index, AssetType type);
			QStringList clipList(int project_index);
			QStringList clipAnimationsList(int project_index);
			QStringList eventList(int project_index, bool attacksOnly);
			QStringList wordVariableList(int project_index);

			/* CACHE SETS */

			size_t getAnimationSetsFiles(int project_file);
			QString getAnimationSetsFile(int project_file, int index);
			void createAnimationSet(int project_file, const QString& name); //add .txt
			void deleteAnimationSet(int project_file, int index);

			size_t getAnimationSetEvents(int project_file, int set_index);
			QString getAnimationSetEvent(int project_file, int set_index, int event_index);
			void addAnimationSetEvent(int project_file, int set_index, const QString& event_name);
			void deleteAnimationSetEvent(int project_file, int set_index, int event_index);

			size_t getAnimationSetAttacks(int project_file, int set_index);
			QString getAnimationSetAttackEvent(int project_file, int set_index, int attack_index);
			void setAnimationSetAttackEvent(int project_file, int set_index, int attack_index, const QString& attack_event);
			void addAnimationSetAttack(int project_file, int set_index, const QString& attack_event);
			void deleteAnimationSetAttack(int project_file, int set_index, int attack_index);
			size_t getAnimationSetAttackClips(int project_file, int set_index, int attack_index);
			QString getAnimationSetAttackClip(int project_file, int set_index, int attack_index, int clip_index);
			void addAnimationSetAttackClip(int project_file, int set_index, int attack_index, const QString& clip_generator_name);
			void deleteAnimationSetAttackClip(int project_file, int set_index, int attack_index, int clip_index);

			size_t getAnimationSetVariables(int project_file, int set_index);
			QString getAnimationSetVariable(int project_file, int set_index, int variable_index);
			int getAnimationSetVariableMin(int project_file, int set_index, int variable_index);
			int getAnimationSetVariableMax(int project_file, int set_index, int variable_index);
			void setAnimationSetVariableMin(int project_file, int set_index, int variable_index, int min_value);
			void setAnimationSetVariableMax(int project_file, int set_index, int variable_index, int max_value);
			void addAnimationSetVariable(int project_file, int set_index, const QString& variable_name, int min_value, int max_value);
			void deleteAnimationSetVariable(int project_file, int set_index, int variable_index);

			QStringList getAnimationSetAnimation(int project_file, int set_index);
			void addAnimationSetAnimation(int project_file, int set_index, const QString& animation_path);
			void deleteAnimationSetAnimation(int project_file, int set_index, const QString& animation_path);

			QStringList getStates(hkbStateMachine* fsm);

			/*FILES*/
			void CreateNewBehaviorFile(int project_file, const QString& behavior_name);
			void CreateNewProject(const QString& project_name, const QString& sub_folder, ProjectType type);

		};
	}
}