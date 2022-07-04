#include "ProjectTreeModel.h"

using namespace ckcmd::HKX;

QModelIndex ProjectTreeModel::mapToSource(const QModelIndex& proxyIndex) const
{
	if (!proxyIndex.isValid())
		return QModelIndex();

	auto source_index = sourceModel()->createIndex(0, 0, proxyIndex.internalId());
	if (proxyIndex.internalId() == 0)
		source_index = QModelIndex();
	auto mapped_index = sourceModel()->child(proxyIndex.row(), source_index);
	return mapped_index;
}

QModelIndex ProjectTreeModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();
	auto source_parent = sourceModel()->parent(sourceIndex);
	int row = sourceModel()->childIndex(sourceIndex);
	auto index = createIndex(row, 0, source_parent.internalId());
	return index;
}

int ProjectTreeModel::columnCount(const QModelIndex& parent) const
{
	if (nullptr == sourceModel())
		return 0;
	return 1;
}

int ProjectTreeModel::rowCount(const QModelIndex& parent) const
{
	if (nullptr == sourceModel()) 
		return 0;
	int row = parent.row();
	auto count = sourceModel()->childCount(mapToSource(parent));
	return count;
}

QModelIndex ProjectTreeModel::parent(const QModelIndex& child) const {
	if (!child.isValid() || !sourceModel()) return QModelIndex();
	Q_ASSERT(child.model() == this);
	QModelIndex mapped_child = mapToSource(child);
	QModelIndex mapped_parent = sourceModel()->parent(mapped_child);
	QModelIndex unmapped_parent = mapFromSource(mapped_parent);
	return unmapped_parent;
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex& parent) const {
	if (!sourceModel()) return {};
	Q_ASSERT(!parent.isValid() || parent.model() == this);
	QModelIndex mapped_parent = mapToSource(parent);
	QModelIndex mapped_child = sourceModel()->child(row, mapped_parent);
	QModelIndex unmapped_child = mapFromSource(mapped_child);
	return unmapped_child;
}

bool ProjectTreeModel::hasChildren(const QModelIndex& parent) const
{
	return sourceModel()->hasChildren(mapToSource(parent));
}

QVariant ProjectTreeModel::data(const QModelIndex& proxyIndex, int role) const
{
	return sourceModel()->data(mapToSource(proxyIndex), role);
}

void ProjectTreeModel::select(const QModelIndex& index)
{
	if (nullptr != sourceModel())
		sourceModel()->select(mapToSource(index));
}

void ProjectTreeModel::activate(const QModelIndex& index)
{
	if (nullptr != sourceModel())
	{
		auto source_index = mapToSource(index);
		auto type = sourceModel()->nodeType(source_index);
		if (type == NodeType::CharacterNode || type == NodeType::MiscNode)
		{
			if (rowCount(index) <= 0)
			{
				emit beginInsertRows(index, 0, 0);
				sourceModel()->activate(source_index);
				emit endInsertRows();
			}
			else {
				emit beginRemoveRows(index, 0, 0);
				sourceModel()->activate(source_index);
				emit endRemoveRows();
			}
		}
	}
}

//bool ProjectTreeModel::insertRows(int row, int count, const QModelIndex& index)
//{
//	emit beginInsertRows(index, 0, 0);
//	bool result = sourceModel()->insertRows(row, count, mapToSource(index));
//	emit endInsertRows();
//	return result;
//}
//
//bool ProjectTreeModel::removeRows(int row, int count, const QModelIndex& index)
//{
//	emit beginRemoveRows(index, 0, 0);
//	bool result = sourceModel()->removeRows(row, count, mapToSource(index));
//	emit endRemoveRows();
//	return result;
//}

#undef max
#undef min

void ProjectTreeModel::setSourceModel(ProjectModel* newSourceModel)
{ 
	if (newSourceModel == sourceModel()) {
		return;
	}

	beginResetModel();

	disconnect(newSourceModel, nullptr, this, nullptr);

	QAbstractProxyModel::setSourceModel(newSourceModel);

	connect(newSourceModel, &ProjectModel::dataChanged,
		this, &ProjectTreeModel::sourceDataChanged);
	connect(newSourceModel, &ProjectModel::modelAboutToBeReset,
		this, [this]() { beginResetModel(); });
	connect(newSourceModel, &ProjectModel::modelReset,
		this, [this]() { endResetModel(); });

	connect(newSourceModel, &ProjectModel::beginInsertChildren,
		this, &ProjectTreeModel::sourceBeginInsertChildren);
	connect(newSourceModel, &ProjectModel::endInsertChildren,
		this, &ProjectTreeModel::sourceEndInsertChildren);
	connect(newSourceModel, &ProjectModel::beginRemoveChildren,
		this, &ProjectTreeModel::sourceBeginRemoveChildren);
	connect(newSourceModel, &ProjectModel::endRemoveChildren,
		this, &ProjectTreeModel::sourceEndRemoveChildren);

	endResetModel();
}

void ProjectTreeModel::sourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	if (!topLeft.isValid() ||
		!bottomRight.isValid() ||
		(topLeft.parent() != bottomRight.parent()))
	{
		return;
	}

	const auto& _topLeft = mapFromSource(topLeft);
	const auto& _bottomRight = mapFromSource(bottomRight);

	if (_topLeft.isValid() && _bottomRight.isValid())
		emit dataChanged(_topLeft,
			_bottomRight,
			roles);
	else
		__debugbreak();
}

void ProjectTreeModel::sourceBeginInsertChildren(const QModelIndex& sourceParent, int sourceFirst, int sourceLast)
{
	auto parent = mapFromSource(sourceParent);
	emit beginInsertRows(parent, sourceFirst, sourceLast);
}

void ProjectTreeModel::sourceEndInsertChildren()
{
	emit endInsertRows();
}

void ProjectTreeModel::sourceBeginRemoveChildren(const QModelIndex& sourceParent, int sourceFirst, int sourceLast)
{
	auto parent = mapFromSource(sourceParent);
	int rowCount = this->rowCount(parent);
	emit beginRemoveRows(parent, sourceFirst, sourceFirst);
}
void ProjectTreeModel::sourceEndRemoveChildren()
{
	emit endRemoveRows();
}

NodeType ProjectTreeModel::nodeType(const QModelIndex& index)
{
	return sourceModel()->nodeType(mapToSource(index));
}

//bool ProjectTreeModel::hasChildren(const QModelIndex& parent) const
//{
//	if (nullptr != sourceModel())
//	{
//		bool hasChildren = sourceModel()->hasChildren(mapToSource(parent));
//		return hasChildren;
//	}
//	return false;
//}

