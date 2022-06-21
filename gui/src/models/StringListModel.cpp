#include "StringListModel.h"

StringListModel::StringListModel(const QStringList& strings, QObject* parent) :
	QStringListModel(strings, parent)
{
}

QVariant StringListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section < _headerData.size())
			return _headerData[section];
	}
	return QVariant();
}

bool StringListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
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