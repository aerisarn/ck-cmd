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

int ModelEdge::childRows(int row, int column, ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return ProjectTreeFileHandler::getChildCount(_project, _childType, manager);
	default:
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::childRows(_project, _file, row, column, variant, _childType, manager);
	}
	return 0;
}

int ModelEdge::rowColumns(int row, int column, ResourceManager& _resourceManager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return 1;
	default:
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::childRowColumns(_project, _file, row, column, variant, _childType, _resourceManager);
	}
	return 0;
}

int ModelEdge::childColumns(int row, int column, ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return 1;
	default:
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::childColumns(_project, _file, row, column, variant, _childType, manager);
	}
	return 0;
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
		return ProjectTreeHkHandler::data(_file, row, column, variant, _childType, manager);
	}
	return QVariant();
}

bool ModelEdge::hasChild(int row, int column, ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return ProjectTreeFileHandler::hasChild(row, _project, _file, _childType, manager);
	default:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::hasChild(variant, row, column, _project, _file, variant, manager, _childType);
	}
	}
	return false;
}

ModelEdge ModelEdge::childEdge(int row, int column, ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return ProjectTreeFileHandler::getChild(row, _project, _file, _childType, manager);
	default:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::getChild(variant, row, column, _project, _file, variant, manager, _childType);
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

bool ModelEdge::setData(int row, int column, const QVariant& value, ResourceManager& manager)
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return false;
	default:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::setData(row, column, _project, _file, variant, _childType, value, manager);
	}
	}
	return false;
}

bool ModelEdge::insertRows(int row_start, int count, ResourceManager& manager)
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return false;
	default:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::addRows(row_start, count, _project, _file, variant, _childType, manager);
	}
	}
	return false;
}

bool ModelEdge::removeRows(int row_start, int count, ResourceManager& manager)
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return false;
	default:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		return ProjectTreeHkHandler::removeRows(row_start, count, _project, _file, variant, _childType, manager);
	}
	}
	return false;
}

bool ModelEdge::insertColumns(int row, int column_start, int count, ResourceManager& manager)
{
	hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
	return ProjectTreeHkHandler::changeColumns(row, column_start, count, _project, _file, variant, _childType, manager);
}

bool ModelEdge::removeColumns(int row, int column_start, int count, ResourceManager& manager)
{
	hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
	return ProjectTreeHkHandler::changeColumns(row, column_start, -count, _project, _file, variant, _childType, manager);
}