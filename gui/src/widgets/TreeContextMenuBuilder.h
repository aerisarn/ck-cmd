#pragma once

#include <src/models/ProjectNode.h>

#include <QMenu>

namespace ckcmd {
	namespace HKX {

		class TreeContextMenuBuilder
		{
		public:
			TreeContextMenuBuilder() {}


			QMenu* build(ProjectNode* node);
		};
	}
}