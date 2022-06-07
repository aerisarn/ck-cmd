#include "HkxTableVariant.h"

#include <src\hkx\Getter.h>
#include <src\hkx\Setter.h>
#include <src\hkx\RowCalculator.h>
#include <src\hkx\ColumnCalculator.h>

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

QVariant HkxTableVariant::data(int row, int column)
{
	Getter g(row, column);
	accept(g);
	return g.value();
}