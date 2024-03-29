#include "TreeContextMenuBuilder.h"

using namespace ckcmd::HKX;

void TreeContextMenuBuilder::buildCharactersNodeMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.createProjectAction());
}

void TreeContextMenuBuilder::buildMiscsNodeMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.createProjectAction());
}

void TreeContextMenuBuilder::buildProjectNodeMenu(std::vector<QAction*>& actions, const QModelIndex& action_data)
{
	actions.push_back(_actionHandler.saveAction(action_data));
	actions.push_back(_actionHandler.exportLEAction(action_data));
	actions.push_back(_actionHandler.exportSEAction(action_data));
}

void TreeContextMenuBuilder::buildAnimationsMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.importFBXAction());
}

void TreeContextMenuBuilder::buildAnimationMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.exportFBXAction());
	actions.push_back(_actionHandler.removeAnimationAction());
}

void TreeContextMenuBuilder::buildEventsMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.addEventAction());
}

void TreeContextMenuBuilder::buildEventMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.removeEventAction());
}

void TreeContextMenuBuilder::buildVariablesMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.addVariableAction());
}

void TreeContextMenuBuilder::buildVariableMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.removeVariableAction());
}

void TreeContextMenuBuilder::buildTransitionsMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.addTransitionAction());
}

void TreeContextMenuBuilder::buildTransitionMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.removeTransitionAction());
}

void TreeContextMenuBuilder::buildSkeletonMenu(std::vector<QAction*>& actions)
{
	actions.push_back(_actionHandler.exportFBXAction());
}

void TreeContextMenuBuilder::buildHavokMenu(std::vector<QAction*>& actions, const QModelIndex& action_data)
{
	auto addActions = _actionHandler.addActions(action_data);
	for (const auto& action : addActions)
		actions.push_back(action);
	actions.push_back(nullptr);
	actions.push_back(_actionHandler.copyAction());
	auto remove = _actionHandler.removeAction();
	remove->setData(QVariant());
	actions.push_back(remove);
}

QMenu* TreeContextMenuBuilder::build(NodeType type, const QModelIndex& action_data)
{
	std::vector<QAction*> applicable_actions;
	switch (type) {
		case NodeType::animationNames:
			buildAnimationsMenu(applicable_actions);
			break;
		case NodeType::animationName:
			buildAnimationMenu(applicable_actions);
			break;
		case NodeType::behaviorEventNames:
			buildEventsMenu(applicable_actions);
			break;
		case NodeType::behaviorEventName:
			buildEventMenu(applicable_actions);
			break;
		case NodeType::behaviorVariableNames:
			buildVariablesMenu(applicable_actions);
			break;
		case NodeType::behaviorVariable:
			buildVariableMenu(applicable_actions);
			break;
		case NodeType::SkeletonHkxNode:
			buildSkeletonMenu(applicable_actions);
			break;
		case NodeType::FSMStateTransitions:
		case NodeType::FSMWildcardTransitions:
			buildTransitionsMenu(applicable_actions);
			break;
		case NodeType::FSMStateTransition:
		case NodeType::FSMWildcardTransition:
			buildTransitionMenu(applicable_actions);
			break;
		case NodeType::CharacterNode:
		case NodeType::MiscNode:
			buildProjectNodeMenu(applicable_actions, action_data);
			break;
//here we add more specialized actions for node types. no break!
//finally build generic havok menu
		case NodeType::HavokNative:
			buildHavokMenu(applicable_actions, action_data);
			break;
		default:
			break;
	}
	if (!applicable_actions.empty())
	{
		QMenu* menu = new QMenu();
		for (auto& action : applicable_actions)
		{
			if (action == nullptr)
			{
				menu->addSeparator();
				continue;
			}
			if (action->data().isNull())
				action->setData(action_data);
			menu->addAction(action);
		}
		return menu;
	}
	return nullptr;
}