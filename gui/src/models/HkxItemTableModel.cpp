#include "HkxItemTableModel.h"

#include <src\hkx\NameGetter.h>
#include <src\hkx\Getter.h>
#include <src\hkx\RowCalculator.h>
#include <src\hkx\ColumnCalculator.h>
#include <src\hkx\HkxItemPointer.h>
#include <src\hkx\HkxTableVariant.h>

using namespace ckcmd::HKX;

HkxItemTableModel::HkxItemTableModel(hkVariant* variant, int file, QObject* parent) :
	_variant(variant), _file(file),
	QAbstractTableModel(parent)
{
}

hkVariant* HkxItemTableModel::getObject(const QModelIndex& index) const {
	return (hkVariant*)index.internalId();
}

QVariant HkxItemTableModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		HkxTableVariant h(*_variant);
		RowCalculator r;
		ColumnCalculator c;
		h.accept(r);
		size_t rows = r.rows();
		h.accept(c);
		size_t columns = c.column(index.row());
		if (index.row() < rows && index.column() < columns)
		{
			Getter g(index.row(), index.column(), _file);
			h.accept(g);
			return g.value();
		}
		return QVariant();
	}
	return QVariant();
}

int HkxItemTableModel::rowCount(const QModelIndex& parent) const
{
	RowCalculator r;
	HkxTableVariant h(*_variant);
	h.accept(r);
	return r.rows();
}

int HkxItemTableModel::columnCount(const QModelIndex& parent) const
{
	ColumnCalculator c;
	HkxTableVariant h(*_variant);
	h.accept(c);
	return c.columns();
}

QVariant HkxItemTableModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Orientation::Horizontal)
		{
			if (section == 0)
				return tr("value");
			else
				return QString("value[%1]").arg(section);
		}
		else if (orientation == Qt::Orientation::Vertical) {
			NameGetter n(section, "");
			HkxTableVariant h(*_variant);
			h.accept(n);
			return n.name();
		}
	}
	return QVariant();
}

bool HkxItemTableModel::setData(const QModelIndex& index, const QVariant& value,
	int role)
{
	return false;
}