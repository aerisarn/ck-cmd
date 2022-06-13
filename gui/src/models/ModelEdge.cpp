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
		return ProjectTreeFileHandler::getChildCount(_project, _childType, manager);
	default:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::getChildCount(_project, variant, _childType, manager);
	}
	}
	return 0;
}