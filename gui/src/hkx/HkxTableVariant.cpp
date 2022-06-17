#include "HkxTableVariant.h"

#include <src\hkx\Getter.h>
#include <src\hkx\Setter.h>
#include <src\hkx\RowCalculator.h>
#include <src\hkx\ColumnCalculator.h>
#include <src\hkx\NameGetter.h>

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