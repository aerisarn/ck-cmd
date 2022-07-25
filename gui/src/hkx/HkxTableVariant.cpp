#include "HkxTableVariant.h"

#include <src\hkx\Getter.h>
#include <src\hkx\Setter.h>
#include <src\hkx\RowCalculator.h>
#include <src\hkx\ColumnCalculator.h>
#include <src\hkx\NameGetter.h>
#include <src\hkx\RowResizer.h>
#include <src\hkx\ArrayFinder.h>
#include <src\hkx\ClassGetter.h>

using namespace ckcmd::HKX;

size_t HkxTableVariant::rows() 
{
	RowCalculator r;
	accept(r);
	return r.rows();
}

size_t HkxTableVariant::columns()
{
	ColumnCalculator c;
	accept(c);
	return c.columns();
}

size_t HkxTableVariant::columns(int row_index)
{
	ColumnCalculator c;
	accept(c);
	return c.column(row_index);
}

QString HkxTableVariant::rowName(int row_index)
{
	NameGetter ng(row_index,"");
	accept(ng);
	return ng.name();
}

TypeInfo HkxTableVariant::rowClass(int row_index)
{
	ClassGetter cg(row_index);
	accept(cg);
	return cg.hkclass();
}

QVariant HkxTableVariant::data(int row, int column)
{
	Getter g(row, column);
	accept(g);
	return g.value();
}

bool HkxTableVariant::setData(int row, int column, const QVariant& value)
{
	Setter s(row, column, value);
	accept(s);
	return s.result();
}

QStringList HkxTableVariant::rowNames()
{
	QStringList names;
	size_t _rows = rows();
	for (size_t r = 0; r < _rows; ++r)
	{
		NameGetter n(r, "");
		accept(n);
		names << n.name();
	}
	return names;
}

std::vector<TypeInfo> HkxTableVariant::rowClasses()
{
	std::vector<TypeInfo> classes;
	size_t _rows = rows();
	for (size_t r = 0; r < _rows; ++r)
	{
		classes.push_back(rowClass(r));
	}
	return classes;
}

std::vector<int>  HkxTableVariant::arrayrows()
{
	ArrayFinder af;
	accept(af);
	return af.arrayrows();
}

bool HkxTableVariant::resizeColumns(int row, int column, int delta)
{
	RowResizer sizer(row, column, delta);
	accept(sizer);
	return sizer.result();
}

std::vector<std::tuple<QString, TypeInfo, bool, size_t>> HkxTableVariant::bindables()
{
	std::vector<std::tuple<QString, TypeInfo, bool, size_t>> out;
	
	RowCalculator r;
	r.setIgnoreNotSerializable(false);
	accept(r);
	int rows = r.rows();

	ArrayFinder af;
	af.setIgnoreNotSerializable(false);
	accept(af);
	auto arrays = af.arrayrows();


	for (int i = 0; i < rows; ++i)
	{
		NameGetter ng(i, "");
		ng.setIgnoreNotSerializable(false);
		accept(ng);
		auto name =  ng.name();

		ClassGetter cg(i);
		cg.setIgnoreNotSerializable(false);
		accept(cg);
		auto hkclass = cg.hkclass();

		bool isArray = std::find(arrays.begin(), arrays.end(), i) != arrays.end();
		int columns = 0;
		if (isArray)
		{
			ColumnCalculator c;
			c.setIgnoreNotSerializable(false);
			accept(c);
			columns = c.column(i);
		}
		out.push_back({ name, hkclass, isArray, columns });
	}
	return out;
}