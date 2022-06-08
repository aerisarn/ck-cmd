#pragma once

#include <src/models/ProjectNode.h>
#include <src/hkx/CommandManager.h>
#include <src/hkx/ResourceManager.h>
#include <src/hkx/BehaviorBuilder.h>

class hkbProjectStringData;
class hkbCharacterData;
class hkbBehaviorGraphData;
class hkbBehaviorGraphStringData;
class hkaSkeleton;

namespace ckcmd {
	namespace HKX {

		class ProjectBuilder {

			ProjectNode* _parent;
			CommandManager& _commandManager;
			ResourceManager& _resourceManager;
			const std::string& _name; // unique id. Maybe

			template<typename T>
			T* loadHkxFile(const fs::path& path, const hkClass& hk_class, hkVariant*& root)
			{
				auto& content = _resourceManager.get(path);
				int data_index = -1;
				int root_index = -1;
				for (size_t v = 0; v < content.second.size(); v++)
				{
					if (content.second[v].m_class == &hk_class)
					{
						data_index = v;
						break;
					}
				}
				if (data_index == -1)
					throw std::runtime_error("hkbCharacterStringData variant not found in " + path.string());
				root = &content.second[data_index];
				return(T*)(content.second[data_index].m_object);
			}

			std::pair<hkbProjectStringData*, size_t>  buildProjectFileModel();
			std::tuple<hkbCharacterData*, size_t, ProjectNode*> buildCharacter(const fs::path& project_folder, ProjectNode* characters_node);
			std::tuple<hkaSkeleton*, hkaSkeleton*, size_t, ProjectNode*> buildSkeleton(const fs::path& rig_path, ProjectNode* character_node);
			std::vector<std::tuple<hkbBehaviorGraphData*, hkbBehaviorGraphStringData*, size_t, ProjectNode*>> buildBehaviors(const fs::path& behaviors_path, ProjectNode* behaviors_node);

		public:
			ProjectBuilder(
				ProjectNode* parent,
				CommandManager& commandManager,
				ResourceManager& resourceManager,
				const std::string& _name
			);
		};
	}
}