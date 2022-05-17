#pragma once

#include <src/hkx/TreeBuilder.h>

namespace ckcmd {
	namespace HKX {
		class IDLEBuilder {

			ResourceManager& _manager;
			size_t _file_index;
			ProjectNode* _root;

		public:
			IDLEBuilder(size_t file_index, ProjectNode* root, ResourceManager& manager);
		};
	}
}