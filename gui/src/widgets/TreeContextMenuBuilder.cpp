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

void TreeContextMenuBuilder::buildProjectNodeMenu(std::vector<QAction*>& actions, const QVariant& action_data)
{
	actions.push_back(_actionHandler.saveAction(action_data));
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

void TreeContextMenuBuilder::buildHavokMenu(std::vector<QAction*>& actions, const QVariant& action_data)
{
	auto addActions = _actionHandler.addActions(action_data);
	for (const auto& action : addActions)
		actions.push_back(action);
	actions.push_back(_actionHandler.copyAction());
}

QMenu* TreeContextMenuBuilder::build(NodeType type, QVariant action_data)
{
	std::vector<QAction*> applicable_actions;
	switch (type) {
		case NodeType::animationNames:
			buildAnimationsMenu(applicable_actions);
			break;
		case NodeType::animationName:
			buildAnimationMenu(applicable_actions);
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
			action->setData(action_data);
			menu->addAction(action);
		}
		return menu;
	}
	return nullptr;
}