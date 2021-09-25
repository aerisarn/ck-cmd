#pragma once

#include <core/HKXWrangler.h>
#include <src/Log.h>
#include <src/models/ProjectNode.h>
#include <src/hkx/ITreeBuilderClassHandler.h>
#include <src/hkx/ISpecialFieldsHandler.h>
#include <src/workspace.h>
#include <core/AnimationCache.h>

#include <map>

typedef std::pair< hkVariant, std::vector<hkVariant>> hkx_file_t;

namespace ckcmd {
	namespace HKX {

		typedef std::vector<std::pair<int, const hkVariant*>> hk_object_list_t;

		class ResourceManager {

			std::vector<fs::path> _files;
			std::map<size_t, hkx_file_t> _contents;
			std::map<size_t, ITreeBuilderClassHandler*> _class_handlers;
			std::map<size_t, ISpecialFieldsHandler*> _field_handlers;
			std::map<size_t, std::vector<ProjectNode*>> _nodes;
			std::map<size_t, string> _sanitized_names;
			WorkspaceConfig& _workspace;
			AnimationCache _cache;

			const std::string& get_sanitized_name(int file_index);

		public:

			ResourceManager(WorkspaceConfig& _workspace);

			~ResourceManager() {
			}

			WorkspaceConfig& workspace() { return _workspace; }

			hkx_file_t& get(size_t index);
			hkx_file_t& get(const fs::path& file);

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


			void setClassHandler(size_t index, ITreeBuilderClassHandler* handler);
			void setFieldHandler(size_t index, ISpecialFieldsHandler* handler);

			ITreeBuilderClassHandler* classHandler(size_t index) const;
			ISpecialFieldsHandler* fieldsHandler(size_t index) const;

			hk_object_list_t findCompatibleNodes(size_t file_index, const hkClassMember* member_class) const;

			ProjectNode* createStatic(const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createStatic(const QVector<QVariant>& data, ProjectNode* parentItem, ProjectNode::NodeType type);
			ProjectNode* createSupport(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createEventsSupport(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createWeaponSetNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem);
			ProjectNode* createVariablesSupport(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createAnimationStylesSupport(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createAnimationStyle(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createProject(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createCharacter(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createBehavior(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createSkeleton(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItemm = nullptr);
			ProjectNode* createAnimation(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createMisc(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createHkxProject(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createHkxCharacter(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createHkxNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createEventNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createVariableNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createPropertyNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);
			ProjectNode* createClipEventNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem = nullptr);

			ProjectNode* findNode(int file, const hkVariant* variant) const;
			QModelIndex getIndex(ProjectNode* node) const;

			bool isCreatureProject(int file_index);
			CacheEntry* findCacheEntry(const std::string& sanitized_name);
			CacheEntry* findOrCreateCacheEntry(size_t file_index, bool character);
			CacheEntry* findCacheEntry(size_t file_index);
			const AnimData::ClipMovementData& getMovement(size_t file_index, string clip);
			vector<AnimationCache::event_info_t> getEventsInfo(size_t file_index, string anim_event);
			void save_cache(int file_index);



		};
	}
}