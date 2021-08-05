#include "HkxItemModel.h"

#include <src\hkx\NameGetter.h>
#include <src\hkx\Getter.h>
#include <src\hkx\RowCalculator.h>
#include <src\hkx\ColumnCalculator.h>
#include <src\hkx\HkxItemPointer.h>
#include <src\hkx\HkxTableVariant.h>

using namespace ckcmd::HKX;

//std::vector<std::array<size_t,2>> debug;
//QVector<QPair<QString,QVariant>> values;

	//for (int i = 0; i < _objects.getSize(); i++)
	//{
	//	RowCalculator r;
	//	ColumnCalculator c;

	//	HkxTableVariant h(_objects[i]);
	//	h.accept(r);
	//	h.accept(c);
	//	size_t rows = r.rows();
	//	size_t columns = c.columns();
	//	for (int r = 0; r < rows; r++) {
	//		NameGetter n(r);
	//		h.accept(n);
	//		for (int c = 0; c < columns; c++)
	//		{
	//			Getter g(r, c);
	//			h.accept(g);
	//			//std::string temp_name = n.name().toStdString();
	//			values.push_back({ n.name(),g.value() });
	//		}
	//	}
	//	debug.push_back({ rows,columns });
	//}

HkxItemModel::HkxItemModel(const fs::path& file, QObject* parent) :
    QAbstractItemModel(parent)
{
    HKXWrapper temp;
	temp.read(file, _objects);
	for (int i = 0; i < _objects.getSize(); i++) {
		if (_objects[i].m_class == &hkRootLevelContainerClass)
		{
			_root = &_objects[i];
			break;
		}
	}
}

const hkVariant* HkxItemModel::find(const void* object) const 
{
	for (int i = 0; i < _objects.getSize(); i++) {
		if (_objects[i].m_object == object)
			return &_objects[i];
	}
	return NULL;
}

hkVariant* HkxItemModel::find(void* object) 
{
	return const_cast<hkVariant*>(const_cast<const HkxItemModel*>(this)->find(object));
}

hkVariant* HkxItemModel::getObject(const QModelIndex& index) const {
	return (hkVariant*)index.internalId();
}

/*
** AbstractItemModel(required methods)
*/

QVariant HkxItemModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		hkVariant* object = getObject(index);
		if (object == NULL)
			object = _root;
		HkxTableVariant h(*object);
		if (index.column() == 0) {
			NameGetter n(index.row(), "");
			h.accept(n);
			return n.name();
		}
		else {
			Getter g(index.row(), index.column() - 1);
			h.accept(g);
			return g.value();
		}
	}
	return QVariant();
}

//row = field
//column = number of values in the field (1 scalar, >1 vector/matrix)
QModelIndex HkxItemModel::index(int row, int column, const QModelIndex& parent) const 
{
	hkVariant* object = getObject(parent);
	if (object == NULL)
		return createIndex(row, column, _root);
	HkxTableVariant h(*object);
	size_t g_column = column;
	if (g_column == 0) g_column = 1;
	Getter g(row, g_column - 1);
	h.accept(g);
	if (g.value().canConvert<HkxItemPointer>()) {
		const void* ptr = g.value().value<HkxItemPointer>().get();
		const hkVariant* v = find(ptr);
		return createIndex(row, column, const_cast<hkVariant*>(v));
	}
	return createIndex(row, column, object);
}

QModelIndex HkxItemModel::parent(const QModelIndex& index) const 
{
	hkVariant* object = getObject(index);
	if (object == NULL || object == _root)
		return QModelIndex();
	//found it. Now we have to look again to see where this came from, and here is practically undetermined
	for (int i = 0; i < _objects.getSize(); i++)
	{
		RowCalculator r;
		ColumnCalculator c;
		hkVariant* parent = const_cast<hkVariant*>(&_objects[i]);
		HkxTableVariant h(*parent);
		h.accept(r);
		h.accept(c);
		size_t rows = r.rows();
		size_t columns = c.columns();
		for (int r = 0; r < rows; r++) {
			NameGetter n(r, "");
			h.accept(n);
			for (int c = 0; c < columns; c++)
			{
				Getter g(r, c);
				h.accept(g);
				if (g.value().canConvert<HkxItemPointer>() &&
					object == const_cast<hkVariant*>(find(g.value().value<HkxItemPointer>().get()))) {
					return createIndex(r, c + 1, parent);
				}
			}
		}
	}
	return QModelIndex();
}

int HkxItemModel::rowCount(const QModelIndex& parent) const 
{
	hkVariant* object = getObject(parent);
	if (object == NULL)
		object = _root;
	RowCalculator r;
	HkxTableVariant h(*object);
	h.accept(r);
	return r.rows();
}

int HkxItemModel::columnCount(const QModelIndex& parent) const 
{
	hkVariant* object = getObject(parent);
	if (object == NULL)
		object = _root;
	ColumnCalculator c;
	HkxTableVariant h(*object);
	h.accept(c);
	// column 0 is file name, 1..N values
	return c.columns() + 1;
}

QVariant HkxItemModel::headerData(int section, Qt::Orientation orientation,
    int role) const 
{
	return QVariant();
}

Qt::ItemFlags HkxItemModel::flags(const QModelIndex& index) const
{
	return 0;
}

bool HkxItemModel::setData(const QModelIndex& index, const QVariant& value,
    int role) 
{
	return false;
}