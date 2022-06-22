#pragma once

#include <src/widgets/ActionHandler.h>

#include <QObject>
#include <QMenu>

namespace ckcmd {
	namespace HKX {

		class TreeContextMenuBuilder
		{

			ActionHandler& _actionHandler;

			QMenu* buildCharactersNodeMenu();
			QMenu* buildMiscsNodeMenu();
			QMenu* buildAnimationsMenu();


		public:
			TreeContextMenuBuilder(ActionHandler& actionHandler)
				: _actionHandler(actionHandler)
			{}

			QMenu* build(NodeType type);
		};
	}
}