#pragma once

#include <src/widgets/ActionHandler.h>

#include <QAbstractItemModel>
#include <QObject>
#include <QMenu>

namespace ckcmd {
	namespace HKX {

		class TreeContextMenuBuilder
		{

			ActionHandler& _actionHandler;

			void buildCharactersNodeMenu(std::vector<QAction*>& actions);
			void buildMiscsNodeMenu(std::vector<QAction*>& actions);
			void buildProjectNodeMenu(std::vector<QAction*>& actions, const QModelIndex& action_data);
			void buildAnimationsMenu(std::vector<QAction*>& actions);
			void buildAnimationMenu(std::vector<QAction*>& actions);
			void buildEventsMenu(std::vector<QAction*>& actions);
			void buildEventMenu(std::vector<QAction*>& actions);
			void buildVariablesMenu(std::vector<QAction*>& actions);
			void buildVariableMenu(std::vector<QAction*>& actions);
			void buildTransitionsMenu(std::vector<QAction*>& actions);
			void buildTransitionMenu(std::vector<QAction*>& actions);
			void buildSkeletonMenu(std::vector<QAction*>& actions);
			void buildHavokMenu(std::vector<QAction*>& actions, const QModelIndex& action_data);

		public:
			TreeContextMenuBuilder(ActionHandler& actionHandler)
				: _actionHandler(actionHandler)
			{}

			QMenu* build(NodeType type, const QModelIndex& action_data);
		};
	}
}