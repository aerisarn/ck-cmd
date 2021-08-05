#pragma once

#include <src/models/ProjectNode.h>
#include <src/hkx/ResourceManager.h>

namespace ckcmd {
	namespace HKX {

		class ProjectBuilder {

			//TODO: consolidate builders into heirarchy
			ProjectNode* _parent;
			ResourceManager& _resourceManager;
			const std::string& _name; // unique id. Maybe

		public:
			ProjectBuilder(
				ProjectNode* parent,
				ResourceManager& resourceManager,
				const std::string& _name
			);
		};
	}
}