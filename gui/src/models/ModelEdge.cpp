#include <src/models/ModelEdge.h>
#include <src/models/ProjectTreeHkHandler.h>
#include <src/models/ProjectTreeFileHandler.h>

using namespace ckcmd::HKX;

ModelEdge::ModelEdge(hkVariant* parent, int project, int file, int row, int column, hkVariant* child)
{
	_parentType = NodeType::HavokNative;
	_parentItem = reinterpret_cast<void*>(parent);
	_project = project;
	_file = file;
	_row = row;
	_column = column;
	_childType = NodeType::HavokNative;
	_childItem = reinterpret_cast<void*>(child);
}

ModelEdge::ModelEdge(hkVariant* parent, int project, int file, int row, int column, hkVariant* child, NodeType childType)
{
	_parentType = NodeType::HavokNative;
	_parentItem = reinterpret_cast<void*>(parent);
	_project = project;
	_file = file;
	_row = row;
	_column = column;
	_childType = childType;
	_childItem = reinterpret_cast<void*>(child);
}

NodeType ModelEdge::type()
{
	//switch (_childType)
	//{
	//case NodeType::ProjectNode:
	//	switch (reinterpret_cast<ProjectNode*>(_childItem)->type())
	//	{
	//	case ProjectNode::NodeType::characters_node:
	//		return NodeType::CharactersNode;
	//	case ProjectNode::NodeType::miscs_node:
	//		return NodeType::MiscsNode;
	//	default:
	//		return NodeType::HavokNative;
	//	}
	//default:
	//	break;
	//}
	return _childType;
}

QVariant ModelEdge::data(int row, int column, ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return ProjectTreeFileHandler::data(row, column, _file, _childType, manager);// *reinterpret_cast<const QString*>(_childItem);
	default:
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::data(row, column, variant, _childType);
	}
	return QVariant();
}

ModelEdge ModelEdge::childEdge(int index, ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return ProjectTreeFileHandler::getChild(index, _project, _file, _childType, manager);
	//case NodeType::ProjectNode:
	//{
	//	ProjectNode* node = reinterpret_cast<ProjectNode*>(_childItem);
	//	if (node->isVariant())
	//	{
	//		auto variant = node->variant();
	//		auto file = node->file();
	//		auto project = node->project();

	//		int havok_children = ProjectTreeHkHandler::getChildCount(variant, _childType);
	//		if (index < havok_children)
	//		{
	//			return ProjectTreeHkHandler::getChild(node, index, project, file, variant, manager, _childType);
	//		}
	//		else {
	//			return ModelEdge(node, project, -1, index, 0, node->child(index - havok_children));
	//		}
	//	}
	//	return ModelEdge(node, _project, _file, index, 0, node->child(index));
	//}
	default:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::getChild(variant, index, _project, _file, variant, manager, _childType);
	}
	}
	return ModelEdge();
}

int ModelEdge::childCount(ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return ProjectTreeFileHandler::getChildCount(_file, _childType, manager);
	//case NodeType::ProjectNode:
	//{
	//	ProjectNode* node = reinterpret_cast<ProjectNode*>(_childItem);
	//	size_t havok_links = 0;
	//	if (node->isVariant())
	//	{
	//		auto variant = node->variant();
	//		havok_links = ProjectTreeHkHandler::getChildCount(variant, _childType);
	//	}
	//	return node->childCount() + havok_links;
	//}
	default:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::getChildCount(variant, _childType);
	}
	}
	return 0;
}