#include "HkxItemTableModel.h"

#include <src\hkx\NameGetter.h>
#include <src\hkx\Getter.h>
#include <src\hkx\Setter.h>
#include <src\hkx\RowCalculator.h>
#include <src\hkx\ColumnCalculator.h>
#include <src\hkx\HkxItemPointer.h>
#include <src\hkx\HkxTableVariant.h>

using namespace ckcmd::HKX;

HkxItemTableModel::HkxItemTableModel(hkVariant* variant, int file, hkVariant* variant_parent, QObject* parent) :
	_variant(variant), _file(file), _parent(variant_parent),
	QAbstractTableModel(parent)
{
}

hkVariant* HkxItemTableModel::getObject(const QModelIndex& index) const {
	return (hkVariant*)index.internalId();
}

bool HkxItemTableModel::indexValid(const QModelIndex& index) const
{
	HkxTableVariant h(*_variant);
	RowCalculator r;
	ColumnCalculator c;
	h.accept(r);
	int rows = r.rows();
	h.accept(c);
	int columns = c.column(index.row());
	return (index.row() < rows && index.column() < columns);
}

QVariant HkxItemTableModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || 
		role == Qt::EditRole)
	{
		if (indexValid(index))
		{
			HkxTableVariant h(*_variant);
			Getter g(index.row(), index.column(), _file, _handlers);
			g.setParentVariant(_parent);
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
	if (role == Qt::EditRole)
	{
		if (indexValid(index))
		{
			HkxTableVariant h(*_variant);
			Setter s(index.row(), index.column(), _file, _handlers);
			s.setParentVariant(_parent);
			h.accept(s);
			return true;
		}
	}
	return false;
}

Qt::ItemFlags HkxItemTableModel::flags(const QModelIndex& index) const
{
	return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}