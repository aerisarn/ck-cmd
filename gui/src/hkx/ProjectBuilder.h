#pragma once

#include <src/models/ProjectNode.h>
#include <src/hkx/ResourceManager.h>
#include <src/hkx/BehaviorBuilder.h>

namespace ckcmd {
	namespace HKX {

		class ProjectBuilder {

			//TODO: consolidate builders into hierarchy
			ProjectNode* _parent;
			ResourceManager& _resourceManager;
			const std::string& _name; // unique id. Maybe
			void ProjectBuilder::buildBranch(hkVariant& root, ProjectNode* root_node, const fs::path& path);

			template<typename T>
			T* loadHkxFile(const fs::path& path, const hkClass& hk_class, hkVariant& root)
			{
				auto& content = _resourceManager.get(path);
				int data_index = -1;
				for (int v = 0; v < content.second.size(); v++)
				{
					if (content.second[v].m_class == &hk_class)
					{
						data_index = v;
					}
				}
				if (data_index == -1)
					throw std::runtime_error("hkbCharacterStringData variant not found in " + path.string());
				root = content.first;
				return(T*)(content.second[data_index].m_object);
			}

		public:
			ProjectBuilder(
				ProjectNode* parent,
				ResourceManager& resourceManager,
				const std::string& _name
			);
		};
	}
}