#pragma once

#include <src/config.h>
#include <src/log.h>
#include <src/models/ProjectNode.h>

#include <set>

namespace ckcmd {
	namespace HKX {

		class AnimationManager {

			const fs::path& _workspace_folder;
			const fs::path	_data_folder;
			const fs::path	_sets_folder;

			std::set<std::string> _characters;
			std::set<std::string> _miscellanous;

		public:
			AnimationManager(const fs::path& workspace_folder);

			void buildProjectTree(ProjectNode* root);
		};
	}
}