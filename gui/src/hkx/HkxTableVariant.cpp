#include "HkxTableVariant.h"

using namespace ckcmd::HKX;

#include <src\hkx\Getter.h>
#include <src\hkx\Setter.h>
#include <src\hkx\RowCalculator.h>
#include <src\hkx\ColumnCalculator.h>

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

size_t HkxTableVariant::references()
{

}

size_t HkxTableVariant::data(int row, int column)
{

}