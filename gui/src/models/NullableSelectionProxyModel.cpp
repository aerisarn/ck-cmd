#include "NullableSelectionProxyModel.h"

using namespace ckcmd::HKX;

NullableSelectionProxyModel::NullableSelectionProxyModel(ProjectModel* sourceModel, const QModelIndex root, QObject* parent) :
	QAbstractProxyModel(parent) 
{
	setSourceModel(sourceModel);
	_sourceRoot = root;
}

QModelIndex NullableSelectionProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
	if (!proxyIndex.isValid())
		return _sourceRoot;

	if (proxyIndex.row() == 0)
		return _sourceRoot;

	return sourceModel()->index(proxyIndex.row() - 1, 0, _sourceRoot);
}

QModelIndex NullableSelectionProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();

	if (sourceIndex == _sourceRoot)
		return QModelIndex();

	int row = sourceModel()->childIndex(sourceIndex);

	return createIndex(row + 1, 0, sourceModel()->parent(sourceIndex).internalId());
}

int NullableSelectionProxyModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}
int NullableSelectionProxyModel::rowCount(const QModelIndex& parent) const
{
	if (parent.row() == 0)
		return 0;
	return 1 + sourceModel()->childCount(mapToSource(parent));
}

QModelIndex NullableSelectionProxyModel::parent(const QModelIndex& child) const {
	if (!child.isValid() || !sourceModel()) return QModelIndex();
	Q_ASSERT(child.model() == this);
	if (child.row() == 0)
		return QModelIndex();
	QModelIndex mapped_child = mapToSource(child);
	QModelIndex mapped_parent = sourceModel()->parent(mapped_child);
	QModelIndex unmapped_parent = mapFromSource(mapped_parent);
	return unmapped_parent;
}

QModelIndex NullableSelectionProxyModel::index(int row, int column, const QModelIndex& parent) const {
	if (!sourceModel()) return {};
	Q_ASSERT(!parent.isValid() || parent.model() == this);
	if (row == 0)
		return createIndex(0, 0, _sourceRoot.internalPointer());
	QModelIndex mapped_parent = mapToSource(parent);
	QModelIndex mapped_child = sourceModel()->index(row - 1, column, mapped_parent);
	QModelIndex unmapped_child = mapFromSource(mapped_child);
	return unmapped_child;
}

QVariant NullableSelectionProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
	if (proxyIndex.row() == 0 && (role == Qt::DisplayRole|| role == Qt::EditRole))
		return "<Not Set>";
	return sourceModel()->data(mapToSource(proxyIndex), role);
}

bool NullableSelectionProxyModel::hasChildren(const QModelIndex& parent) const
{
	return rowCount(parent) > 0 && columnCount(parent) > 0;
}

QModelIndex NullableSelectionProxyModel::buddy(const QModelIndex& index) const
{
	if (index.row() == 0)
		return createIndex(0, 0);
	return mapFromSource(sourceModel()->buddy(mapToSource(index)));
}

Qt::ItemFlags NullableSelectionProxyModel::flags(const QModelIndex& index) const
{
	if (index.row() == 0)
		return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	return sourceModel()->flags(mapToSource(index));
}