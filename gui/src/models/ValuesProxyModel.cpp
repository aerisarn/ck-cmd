#include "ValuesProxyModel.h"

using namespace ckcmd::HKX;

ValuesProxyModel::ValuesProxyModel(ProjectModel* sourceModel, int row, int firstColumn, const QModelIndex root, QObject* parent) :
	_row(row),
	_firstColumn(firstColumn),
	QAbstractProxyModel(parent)
{
	setSourceModel(sourceModel);
	_sourceRoot = root;
}

QModelIndex ValuesProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
	if (!proxyIndex.isValid())
		return _sourceRoot;

	return sourceModel()->index(_row, _firstColumn + proxyIndex.row(), _sourceRoot);
}

QModelIndex ValuesProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();

	if (sourceIndex == _sourceRoot)
		return QModelIndex();

	int row = sourceIndex.column() - _firstColumn;

	return createIndex(row, 0, _sourceRoot.internalId());
}

int ValuesProxyModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

int ValuesProxyModel::rowCount(const QModelIndex& parent) const
{
	auto row_index = sourceModel()->index(_row, 0, _sourceRoot);
	int count = sourceModel()->columnCount(row_index) - _firstColumn;
	return count;
}

QModelIndex ValuesProxyModel::parent(const QModelIndex& child) const {
	if (!child.isValid() || !sourceModel()) return QModelIndex();
	Q_ASSERT(child.model() == this);
	return QModelIndex();
}

QModelIndex ValuesProxyModel::index(int row, int column, const QModelIndex& parent) const {
	if (!sourceModel()) return {};
	Q_ASSERT(!parent.isValid() || parent.model() == this);
	return createIndex(row, column, parent.internalId());
}

QVariant ValuesProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
	return sourceModel()->data(mapToSource(proxyIndex), role);
}

bool ValuesProxyModel::setData(const QModelIndex& proxyIndex, const QVariant& value, int role)
{
	return sourceModel()->setData(mapToSource(proxyIndex), value, role);
}

bool ValuesProxyModel::insertRows(int row, int count, const QModelIndex& parent)
{
	return sourceModel()->insertRows(row, count, mapToSource(parent));
}

Qt::ItemFlags ValuesProxyModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QAbstractItemModel* ValuesProxyModel::editModel(const QModelIndex& index, AssetType type, int role)
{
	return sourceModel()->editModel(mapToSource(index), type, role);
}

