#include "ValuesProxyModel.h"

using namespace ckcmd::HKX;

ValuesProxyModel::ValuesProxyModel(ProjectModel* sourceModel, const std::vector<size_t>& rows, int firstColumn, const QModelIndex root, QObject* parent) :
	_rows(rows),
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

	return sourceModel()->index(_rows.at(proxyIndex.column()), _firstColumn + proxyIndex.row(), _sourceRoot);
}

QModelIndex ValuesProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();

	if (sourceIndex == _sourceRoot)
		return QModelIndex();

	int row = sourceIndex.column() - _firstColumn;

	size_t column = 0;
	for (column; column < _rows.size(); column++)
	{
		if (_rows.at(column) == sourceIndex.row())
			break;
	}

	return createIndex(row, column, _sourceRoot.internalId());
}

int ValuesProxyModel::columnCount(const QModelIndex& parent) const
{
	return _rows.size();
}

int ValuesProxyModel::rowCount(const QModelIndex& parent) const
{
	auto row_index = sourceModel()->index(_rows.at(0), 0, _sourceRoot);
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
	beginInsertRows(parent, row, row + count - 1);
	bool result = sourceModel()->insertColumns(_rows.at(0), _firstColumn + row, count, mapToSource(parent));
	endInsertRows();
	return result;
}

bool ValuesProxyModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows(parent, row, row + count - 1);
	bool result = sourceModel()->removeColumns(_rows.at(0), _firstColumn + row, count, mapToSource(parent));
	endRemoveRows();
	return result;
}

Qt::ItemFlags ValuesProxyModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QAbstractItemModel* ValuesProxyModel::editModel(const QModelIndex& index, AssetType type, const QString& nullValue, int role)
{
	return sourceModel()->editModel(mapToSource(index), type, nullValue, role);
}

QVariant ValuesProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section < _headerData.size())
			return _headerData[section];
	}
	return QVariant();
}

bool ValuesProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	if (orientation == Qt::Horizontal)
	{
		while (_headerData.size() <= section)
			_headerData.push_back("");
		_headerData[section] = value.toString();
		return true;
	}
	return false;
}

int ValuesProxyModel::getFileIndex(const QModelIndex& index)
{
	return sourceModel()->getFileIndex(mapToSource(index));
}
