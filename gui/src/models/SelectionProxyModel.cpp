#include "SelectionProxyModel.h"

using namespace ckcmd::HKX;

SelectionProxyModel::SelectionProxyModel(ProjectModel* sourceModel, const QModelIndex root, QObject* parent) :
	QAbstractProxyModel(parent) 
{
	setSourceModel(sourceModel);
	_sourceRoot = root;
}

QModelIndex SelectionProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
	if (!proxyIndex.isValid())
		return _sourceRoot;

	return sourceModel()->index(proxyIndex.row(), 0, _sourceRoot);
}

QModelIndex SelectionProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();

	if (sourceIndex == _sourceRoot)
		return QModelIndex();

	return createIndex(sourceIndex.row(), 0, sourceIndex.internalId());
}

int SelectionProxyModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}
int SelectionProxyModel::rowCount(const QModelIndex& parent) const
{
	return sourceModel()->childCount(mapToSource(parent));
}

QModelIndex SelectionProxyModel::parent(const QModelIndex& child) const {
	if (!child.isValid() || !sourceModel()) return QModelIndex();
	Q_ASSERT(child.model() == this);
	QModelIndex mapped_child = mapToSource(child);
	QModelIndex mapped_parent = sourceModel()->parent(mapped_child);
	QModelIndex unmapped_parent = mapFromSource(mapped_parent);
	return unmapped_parent;
}

QModelIndex SelectionProxyModel::index(int row, int column, const QModelIndex& parent) const {
	if (!sourceModel()) return {};
	Q_ASSERT(!parent.isValid() || parent.model() == this);
	QModelIndex mapped_parent = mapToSource(parent);
	QModelIndex mapped_child = sourceModel()->index(row, column, mapped_parent);
	QModelIndex unmapped_child = mapFromSource(mapped_child);
	return unmapped_child;
}

QVariant SelectionProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
	return sourceModel()->data(mapToSource(proxyIndex), role);
}

