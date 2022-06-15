#include "TreeContextMenuBuilder.h"

using namespace ckcmd::HKX;

QMenu* TreeContextMenuBuilder::buildCharactersNodeMenu()
{
	QMenu* context_menu = new QMenu();
	context_menu->addAction(_actionHandler.createProjectAction());
	return context_menu;
}

QMenu* TreeContextMenuBuilder::buildMiscsNodeMenu()
{
	QMenu* context_menu = new QMenu();
	context_menu->addAction(_actionHandler.createProjectAction());
	return context_menu;
}

QMenu* TreeContextMenuBuilder::build(NodeType type)
{
	QMenu* context_menu = new QMenu();
	context_menu->addAction(_actionHandler.importFBXAction());
	return context_menu;

	//if (node->type() == ProjectNode::NodeType::characters_node)
	//	return buildCharactersNodeMenu();
	//if (node->type() == ProjectNode::NodeType::miscs_node)
	//	return buildMiscsNodeMenu();
	//return new QMenu();
	//if (node->type() == ProjectNode::NodeType::event_node)
	//{

	//}
	//else if (node->type() == ProjectNode::NodeType::character_node ||
	//	node->type() == ProjectNode::NodeType::misc_node)
	//{
	//	auto save_action = new QAction(tr("&Save"), context_menu);
	//	save_action->setShortcuts(QKeySequence::Save);
	//	save_action->setStatusTip(tr("Create a new file"));
	//}
	return nullptr;
}