#pragma once

#include <src/widgets/ActionHandler.h>

#include <QObject>
#include <QMenu>

namespace ckcmd {
	namespace HKX {

		class TreeContextMenuBuilder
		{

			ActionHandler& _actionHandler;

			void buildCharactersNodeMenu(std::vector<QAction*>& actions);
			void buildMiscsNodeMenu(std::vector<QAction*>& actions);
			void buildAnimationsMenu(std::vector<QAction*>& actions);
			void buildAnimationMenu(std::vector<QAction*>& actions);

		public:
			TreeContextMenuBuilder(ActionHandler& actionHandler)
				: _actionHandler(actionHandler)
			{}

			QMenu* build(NodeType type, QVariant action_data);
		};
	}
}