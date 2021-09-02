#pragma once

#include <src/ILogger.h>
#include <src/hkx/HkxItemVisitor.h>
#include <src/hkx/HkxTableVariant.h>
#include <src/models/ProjectNode.h>

namespace ckcmd {
	namespace HKX {

		class ResourceManager;

		class ITreeBuilderClassHandler {
		public:
			virtual std::vector<const hkClass*> getHandledClasses() = 0;

			virtual ProjectNode* visit(
				const fs::path& _file,
				int object_index,
				ProjectNode* parent) = 0;
		};
	}
}