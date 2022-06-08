#include <src/models/ModelEdge.h>
#include <src/models/ProjectTreeHkHandler.h>
#include <src/hkx/HkxLinkedTableVariant.h>
#include <hkbBehaviorGraph_1.h>

using namespace ckcmd::HKX;

ModelEdge::ModelEdge(ProjectNode* parent, int project, int file, int row, int column, ProjectNode* child)
{
	_parentType = NodeType::ProjectNode;
	_parentItem = reinterpret_cast<void*>(parent);
	_project = project;
	_file = file;
	_row = row;
	_column = column;
	_childType = NodeType::ProjectNode;
	_childItem = reinterpret_cast<void*>(child);
}

ModelEdge::ModelEdge(ProjectNode* parent, int project, int file, int row, int column, hkVariant* child)
{
	_parentType = NodeType::ProjectNode;
	_parentItem = reinterpret_cast<void*>(parent);
	_project = project;
	_file = file;
	_row = row;
	_column = column;
	_childType = NodeType::HavokNative;
	_childItem = reinterpret_cast<void*>(child);
}

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

QVariant ModelEdge::data(int row, int column) const
{
	switch (_childType)
	{
	case NodeType::ProjectNode:
		return reinterpret_cast<ProjectNode*>(_childItem)->data(column);
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
	case NodeType::ProjectNode:
	{
		ProjectNode* node = reinterpret_cast<ProjectNode*>(_childItem);
		if (node->isVariant())
		{
			auto variant = node->variant();
			auto file = node->file();
			auto project = node->project();

			int havok_children = ProjectTreeHkHandler::getChildCount(variant, _childType);
			if (index < havok_children)
			{
				return ProjectTreeHkHandler::getChild(node, index, project, file, variant, manager, _childType);
			}
			else {
				return ModelEdge(node, project, -1, index, 0, node->child(index - havok_children));
			}
		}
		return ModelEdge(node, _project, _file, index, 0, node->child(index));
	}
	default:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::getChild(variant, index, _project, _file, variant, manager, _childType);
	}
	}
	return ModelEdge();
}

int ModelEdge::childCount() const
{
	switch (_childType)
	{
	case NodeType::ProjectNode:
	{
		ProjectNode* node = reinterpret_cast<ProjectNode*>(_childItem);
		size_t havok_links = 0;
		if (node->isVariant())
		{
			auto variant = node->variant();
			havok_links = ProjectTreeHkHandler::getChildCount(variant, _childType);
		}
		return node->childCount() + havok_links;
	}
	default:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::getChildCount(variant, _childType);
	}
	}
	return 0;
}