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
	_parentType = parent._childType;
	_parentItem = parent._childItem;
	_parent = parent._child;
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

TypeInfo ModelEdge::rowClass(int row, ResourceManager& manager) const
{
	return ModelEdgeRegistry::instance().handler(*this)->rowClass(row, *this, manager);
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
	return ModelEdgeRegistry::instance().handler(*this)->removeRows(row_start, count, *this, manager);
}

bool ModelEdge::insertColumns(int row, int column_start, int count, ResourceManager& manager)
{
	return ModelEdgeRegistry::instance().handler(*this)->changeColumns(row, column_start, count, *this, manager);
}

bool ModelEdge::removeColumns(int row, int column_start, int count, ResourceManager& manager)
{
	return ModelEdgeRegistry::instance().handler(*this)->changeColumns(row, column_start, -count, *this, manager);
}

std::pair<int, int> ModelEdge::dataStart()
{
	return ModelEdgeRegistry::instance().handler(*this)->dataStart(*this);
}

bool ModelEdge::canAddRow(const ModelEdge& other, ResourceManager& manager)
{
	return ModelEdgeRegistry::instance().handler(*this)->canAdd(*this, other, manager);
}

std::vector<std::tuple<QString, TypeInfo, bool, size_t>> ModelEdge::bindables(ResourceManager& manager)
{
    return ModelEdgeRegistry::instance().handler(*this)->bindables(*this, manager);
}

bool ModelEdge::isArray(int row, ResourceManager& manager) const
{
	return ModelEdgeRegistry::instance().handler(*this)->isArray(row, *this, manager);
}

size_t ModelEdge::hash_combine(size_t lhs, size_t rhs) const {
    lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}


size_t ModelEdge::hash() const {
    if (
        _childType == NodeType::SkeletonHkxNode ||
        _childType == NodeType::RagdollHkxNode ||
        _childType == NodeType::SkeletonBones ||
        _childType == NodeType::RagdollBones
        )
    {
        return std::hash<int>()(_project) ^
            ((std::hash<int>()((int)_childType) << 1) >> 1);
    }
    if (
        _childType == NodeType::behaviorEventNames ||
        _childType == NodeType::behaviorVariableNames ||
        _childType == NodeType::behaviorCharacterPropertyNames
        )
    {
        return std::hash<int>()(_project) ^
            ((std::hash<int>()((int)_file) << 1) >> 1) ^
            ((std::hash<int>()((int)_childType) << 1));
    }
    if (
        _childType == NodeType::behaviorEventName ||
        _childType == NodeType::behaviorVariable ||
        _childType == NodeType::behaviorCharacterProperty
        )
    {
        return std::hash<int>()(_project) ^
            ((std::hash<int>()((int)_file) << 1) >> 1) ^
            ((std::hash<int>()((int)_row) << 1) >> 1) ^
            ((std::hash<int>()((int)_childType) << 1));
    }
    size_t seed = (size_t)_parentType;
    //seed = hash_combine(seed, (size_t)_parent.row());
    //seed = hash_combine(seed, (size_t)_parent.column());
    //seed = hash_combine(seed, (size_t)_parent.internalPointer());
    seed = hash_combine(seed, (size_t)_project);
    seed = hash_combine(seed, (size_t)_file);
    seed = hash_combine(seed, (size_t)_row);
    seed = hash_combine(seed, (size_t)_column);
    seed = hash_combine(seed, (size_t)_childItem);
    //seed = hash_combine(seed, (size_t)_child.row());
    //seed = hash_combine(seed, (size_t)_child.column());
    //seed = hash_combine(seed, (size_t)_child.internalPointer());
    seed = hash_combine(seed, (size_t)_subindex);
    seed = hash_combine(seed, (size_t)_childType);
    return seed;

    //return std::hash<int>()((int)_parentType) ^
    //    ((std::hash<qintptr>()((qintptr)_parentItem) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_parent.row()) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_parent.column()) << 1) >> 1) ^
    //    ((std::hash<qintptr>()((qintptr)_parent.internalPointer()) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_project) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_file) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_row) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_column) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_subindex) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_childType) << 1) >> 1) ^
    //    ((std::hash<qintptr>()((qintptr)_childItem) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_child.row()) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_child.column()) << 1) >> 1) ^
    //    ((std::hash<qintptr>()((qintptr)_child.internalPointer()) << 1) >> 1) ^
    //    ((std::hash<int>()((int)_childType) << 1));
}

//bool ModelEdge::operator < (const ModelEdge& rhs) const {
//    if (
//        (
//            _childType == NodeType::SkeletonHkxNode ||
//            _childType == NodeType::RagdollHkxNode ||
//            _childType == NodeType::SkeletonBones ||
//            _childType == NodeType::RagdollBones
//            ) || (
//                rhs._childType == NodeType::SkeletonHkxNode ||
//                rhs._childType == NodeType::RagdollHkxNode ||
//                rhs._childType == NodeType::SkeletonBones ||
//                rhs._childType == NodeType::RagdollBones
//                )
//        )
//    {
//        return as_project_index_tuple() < rhs.as_project_index_tuple();
//    }
//    if (
//        (
//            _childType == NodeType::behaviorEventNames ||
//            _childType == NodeType::behaviorVariableNames ||
//            _childType == NodeType::behaviorCharacterPropertyNames
//            ) || (
//                rhs._childType == NodeType::behaviorEventNames ||
//                rhs._childType == NodeType::behaviorVariableNames ||
//                rhs._childType == NodeType::behaviorCharacterPropertyNames
//                )
//        )
//    {
//        return as_file_only_tuple() < rhs.as_file_only_tuple();
//    }
//    if (
//        (
//            _childType == NodeType::behaviorEventName ||
//            _childType == NodeType::behaviorVariable ||
//            _childType == NodeType::behaviorCharacterProperty
//            ) || (
//                rhs._childType == NodeType::behaviorEventName ||
//                rhs._childType == NodeType::behaviorVariable ||
//                rhs._childType == NodeType::behaviorCharacterProperty
//                )
//        )
//    {
//        return as_file_index_tuple() < rhs.as_file_index_tuple();
//    }
//    return as_tuple() < rhs.as_tuple();
//}

bool ModelEdge::project_equal(const ModelEdge& rhs) const
{
    return _project == rhs._project &&
        _childType == rhs._childType;
}

bool ModelEdge::file_equal(const ModelEdge& rhs) const
{
    return _file == rhs._file &&
        _childType == rhs._childType;
}

bool ModelEdge::file_index_equal(const ModelEdge& rhs) const
{
    return _file == rhs._file &&
        _row == rhs._row &&
        _column == rhs._column &&
        _childType == rhs._childType;
}

bool ModelEdge::operator == (const ModelEdge& rhs) const {
    if (
        _childType == NodeType::SkeletonHkxNode ||
        _childType == NodeType::RagdollHkxNode ||
        _childType == NodeType::SkeletonBones ||
        _childType == NodeType::RagdollBones ||
        rhs._childType == NodeType::SkeletonHkxNode ||
        rhs._childType == NodeType::RagdollHkxNode ||
        rhs._childType == NodeType::SkeletonBones ||
        rhs._childType == NodeType::RagdollBones
    )
    {
        return project_equal(rhs);
    }
    if (
        _childType == NodeType::behaviorEventNames ||
        _childType == NodeType::behaviorVariableNames ||
        _childType == NodeType::behaviorCharacterPropertyNames ||
        rhs._childType == NodeType::behaviorEventNames ||
        rhs._childType == NodeType::behaviorVariableNames ||
        rhs._childType == NodeType::behaviorCharacterPropertyNames
    )
    {
        return file_equal(rhs);
    }
    if (
        _childType == NodeType::behaviorEventName ||
        _childType == NodeType::behaviorVariable ||
        _childType == NodeType::behaviorCharacterProperty ||
        rhs._childType == NodeType::behaviorEventName ||
        rhs._childType == NodeType::behaviorVariable ||
        rhs._childType == NodeType::behaviorCharacterProperty
    )
    {
        return file_index_equal(rhs);
    }
    return  _parentType == rhs._parentType &&
        _parentItem == rhs._parentItem &&
        _project == rhs._project &&
        _file == rhs._file &&
        _row == rhs._row &&
        _column == rhs._column &&
        _subindex == rhs._subindex &&
        _childType == rhs._childType &&
        _childItem == rhs._childItem;
}