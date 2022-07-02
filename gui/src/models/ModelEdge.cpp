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

ModelEdge::ModelEdge(hkVariant* parent, int project, int file, int row, int column, int subindex, hkVariant* child, NodeType childType)
{
	_parentType = NodeType::HavokNative;
	_parentItem = reinterpret_cast<void*>(parent);
	_project = project;
	_file = file;
	_row = row;
	_column = column;
	_subindex = subindex;
	_childType = childType;
	_childItem = reinterpret_cast<void*>(child);
}

ModelEdge::ModelEdge(const ModelEdge& parent, int project, int file, int row, int column, int subindex, hkVariant* child, NodeType childType)
{
	_parentType = parent._parentType;
	_parentItem = parent._parentItem;
	_parent = parent._parent;
	_project = project;
	_file = file;
	_row = row;
	_column = column;
	_subindex = subindex;
	_childType = childType;
	_childItem = reinterpret_cast<void*>(child);
}

NodeType ModelEdge::type() const
{
	return _childType;
}

int ModelEdge::rows(ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return ProjectTreeFileHandler::getChildCount(_project, _childType, manager);
	default:
		return ProjectTreeHkHandler::rows(*this, manager);
	}
	return 0;
}

int ModelEdge::columns(int row, ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return 1;
	default:
		return ProjectTreeHkHandler::columns(row, *this, manager);
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
		return ProjectTreeFileHandler::data(_row, _column, _project, _childType, manager);
	default:
		return ProjectTreeHkHandler::data(row, column, *this, manager);
	}
	return QVariant();
}

bool ModelEdge::hasChild(int row, int column, ResourceManager& manager) const 
{ 
	return childIndex(row, column, manager) != MODELEDGE_INVALID; 
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
		return ProjectTreeHkHandler::child(row, column, *this, manager);
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
		return ProjectTreeHkHandler::childCount(*this, manager);
	}
	}
	return 0;
}

std::pair<int, int> ModelEdge::child(int index, ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return { index, 0 };
	default:
	{
		return ProjectTreeHkHandler::child(index, *this, manager);
	}
	}
	return { -1,-1 };
}

int ModelEdge::childIndex(int row, int column, ResourceManager& manager) const
{
	switch (_childType)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		return ProjectTreeFileHandler::childIndex(row, column, *this, manager);
	default:
	{
		return ProjectTreeHkHandler::childIndex(row, column, *this, manager);
	}
	}
	return MODELEDGE_INVALID;
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
		return ProjectTreeHkHandler::setData(row, column, *this, value, manager);
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
		return ProjectTreeHkHandler::addRows(row_start, count, *this, manager);
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
		return ProjectTreeHkHandler::removeRows(row_start, count, *this, manager);
	}
	}
	return false;
}

bool ModelEdge::insertColumns(int row, int column_start, int count, ResourceManager& manager)
{
	hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
	return ProjectTreeHkHandler::changeColumns(row, column_start, count, *this, manager);
}

bool ModelEdge::removeColumns(int row, int column_start, int count, ResourceManager& manager)
{
	hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
	return ProjectTreeHkHandler::changeColumns(row, column_start, -count, *this, manager);
}