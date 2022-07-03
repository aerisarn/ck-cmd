#include <src/models/ModelEdge.h>
#include <src/models/ModelEdgeRegistry.h>

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
	return ModelEdgeRegistry::instance().handler(*this)->rows(*this, manager);
}

int ModelEdge::columns(int row, ResourceManager& manager) const
{
	return ModelEdgeRegistry::instance().handler(*this)->columns(row, *this, manager);
}

QVariant ModelEdge::data(int row, int column, ResourceManager& manager) const
{
	return ModelEdgeRegistry::instance().handler(*this)->data(row, column, *this, manager);
}

bool ModelEdge::hasChild(int row, int column, ResourceManager& manager) const 
{ 
	return childIndex(row, column, manager) != MODELEDGE_INVALID; 
}

ModelEdge ModelEdge::childEdge(int row, int column, ResourceManager& manager) const
{
	return ModelEdgeRegistry::instance().handler(*this)->child(row, column, *this, manager);
}

int ModelEdge::childCount(ResourceManager& manager) const
{
	return ModelEdgeRegistry::instance().handler(*this)->childCount(*this, manager);
}

std::pair<int, int> ModelEdge::child(int index, ResourceManager& manager) const
{
	return ModelEdgeRegistry::instance().handler(*this)->child(index, *this, manager);
}

int ModelEdge::childIndex(int row, int column, ResourceManager& manager) const
{
	return ModelEdgeRegistry::instance().handler(*this)->childIndex(row, column, *this, manager);
}

bool ModelEdge::setData(int row, int column, const QVariant& value, ResourceManager& manager)
{
	return ModelEdgeRegistry::instance().handler(*this)->setData(row, column, *this, value, manager);
}

bool ModelEdge::insertRows(int row_start, int count, ResourceManager& manager)
{
	return ModelEdgeRegistry::instance().handler(*this)->addRows(row_start, count, *this, manager);
}

bool ModelEdge::removeRows(int row_start, int count, ResourceManager& manager)
{
	return ModelEdgeRegistry::instance().handler(*this)->addRows(row_start, count, *this, manager);
}

bool ModelEdge::insertColumns(int row, int column_start, int count, ResourceManager& manager)
{
	return ModelEdgeRegistry::instance().handler(*this)->changeColumns(row, column_start, count, *this, manager);
}

bool ModelEdge::removeColumns(int row, int column_start, int count, ResourceManager& manager)
{
	return ModelEdgeRegistry::instance().handler(*this)->changeColumns(row, column_start, -count, *this, manager);
}