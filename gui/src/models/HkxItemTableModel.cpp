//#include "HkxItemTableModel.h"
//
//#include <src\hkx\NameGetter.h>
//#include <src\hkx\Getter.h>
//#include <src\hkx\Setter.h>
//#include <src\hkx\RowCalculator.h>
//#include <src\hkx\ColumnCalculator.h>
//#include <src\hkx\HkxItemPointer.h>
//#include <src\hkx\HkxVariant.h>
//
//using namespace ckcmd::HKX;
//
//HkxItemTableModel::HkxItemTableModel(
//	CommandManager& command_manager,
//	hkVariant* variant, 
//	int file,
//	QObject* parent
//) :
//	_command_manager(command_manager),
//	_variant(variant), 
//	_file(file),
//	QAbstractTableModel(parent)
//{
//}
//
//hkVariant* HkxItemTableModel::getObject(const QModelIndex& index) const {
//	return (hkVariant*)index.internalId();
//}
//
//bool HkxItemTableModel::indexValid(const QModelIndex& index) const
//{
//	HkxVariant h(*_variant);
//	RowCalculator r;
//	ColumnCalculator c;
//	h.accept(r);
//	int rows = r.rows();
//	h.accept(c);
//	int columns = c.column(index.row());
//	return (index.row() < rows && index.column() < columns);
//}
//
//QVariant HkxItemTableModel::data(const QModelIndex& index, int role) const
//{
//	if (role == Qt::DisplayRole || 
//		role == Qt::EditRole)
//	{
//		if (indexValid(index))
//		{
//			HkxVariant h(*_variant);
//			Getter g(index.row(), index.column());
//			h.accept(g);
//			return g.value();
//		}
//		return QVariant();
//	}
//	return QVariant();
//}
//
//int HkxItemTableModel::rowCount(const QModelIndex& parent) const
//{
//	if (_variant == nullptr)
//		return 0;
//	RowCalculator r;
//	HkxVariant h(*_variant);
//	h.accept(r);
//	return r.rows();
//}
//
//int HkxItemTableModel::columnCount(const QModelIndex& parent) const
//{
//	if (_variant == nullptr)
//		return 0;
//	ColumnCalculator c;
//	HkxVariant h(*_variant);
//	h.accept(c);
//	return c.columns();
//}
//
//QVariant HkxItemTableModel::headerData(int section, Qt::Orientation orientation,
//	int role) const
//{
//	if (role == Qt::DisplayRole)
//	{
//		if (orientation == Qt::Orientation::Horizontal)
//		{
//			if (section == 0)
//				return tr("value");
//			else
//				return QString("value[%1]").arg(section);
//		}
//		else if (orientation == Qt::Orientation::Vertical) {
//			NameGetter n(section, "");
//			HkxVariant h(*_variant);
//			h.accept(n);
//			return n.name();
//		}
//	}
//	return QVariant();
//}
//
//QVariant HkxItemTableModel::internalSetData(const QModelIndex& index, const QVariant& value,
//	int role)
//{
//	if (role == Qt::EditRole)
//	{
//		if (indexValid(index))
//		{
//			QVariant old_value;
//			HkxVariant h(*_variant);
//
//			Getter g(index.row(), index.column());
//			h.accept(g);
//			old_value = g.value();
//
//
//			Setter s(index.row(), index.column(), value);
//			//if (value.canConvert<HkxItemPointer>())
//			//{
//			//	HkxItemPointer new_value(nullptr, nullptr);
//			//	HkxItemPointer old_value_ptr = g.value().value<HkxItemPointer>();
//			//	new_value = value.value<HkxItemPointer>();
//			//	emit HkxItemPointerChanged(old_value_ptr, new_value, _file, _variant);
//			//}
//			h.accept(s);
//			return old_value;
//		}
//	}
//	return QVariant();
//}
//
//bool HkxItemTableModel::setData(const QModelIndex& index, const QVariant& value,
//	int role)
//{
//	if (role == Qt::EditRole)
//	{
//		if (indexValid(index))
//		{
//			_command_manager.pushCommand(
//				new ChangeValue(*this, index, value)
//			);
//		}
//	}
//	return true;
//}
//
//Qt::ItemFlags HkxItemTableModel::flags(const QModelIndex& index) const
//{
//	return Qt::ItemIsEditable | Qt::ItemIsUserCheckable | QAbstractTableModel::flags(index);
//}