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
			void buildProjectNodeMenu(std::vector<QAction*>& actions, const QVariant& action_data);
			void buildAnimationsMenu(std::vector<QAction*>& actions);
			void buildAnimationMenu(std::vector<QAction*>& actions);
			void buildHavokMenu(std::vector<QAction*>& actions);

		public:
			TreeContextMenuBuilder(ActionHandler& actionHandler)
				: _actionHandler(actionHandler)
			{}

			QMenu* build(NodeType type, QVariant action_data);
		};
	}
}