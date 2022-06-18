#include "SelectionProxyModel.h"

using namespace ckcmd::HKX;

SelectionProxyModel::SelectionProxyModel(ProjectModel* sourceModel, const QModelIndex root, QObject* parent) :
	_sourceRoot(root), QAbstractProxyModel(parent) 
{
	setSourceModel(sourceModel);
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

	return index(sourceIndex.row(), 0, QModelIndex());
}
