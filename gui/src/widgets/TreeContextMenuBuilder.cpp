#include "TreeContextMenuBuilder.h"

using namespace ckcmd::HKX;

QMenu* TreeContextMenuBuilder::build(ProjectNode* node)
{
	//return new QMenu();
	if (node->type() == ProjectNode::NodeType::event_node)
	return nullptr;
}