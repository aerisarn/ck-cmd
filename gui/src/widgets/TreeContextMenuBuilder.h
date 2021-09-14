#pragma once

#include <src/models/ProjectNode.h>

#include <src/widgets/ActionHandler.h>

#include <QObject>
#include <QMenu>

namespace ckcmd {
	namespace HKX {

		class TreeContextMenuBuilder
		{

			ActionHandler& _actionHandler;

		public:
			TreeContextMenuBuilder(ActionHandler& actionHandler)
				: _actionHandler(actionHandler)
			{}
			QMenu* build(ProjectNode* node);
		};
	}
}