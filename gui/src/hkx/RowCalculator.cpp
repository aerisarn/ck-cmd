#include "RowCalculator.h"

#include "HkxTableVariant.h"

using namespace ckcmd::HKX;

void RowCalculator::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	_rows += 1;
}

void RowCalculator::visit(void* object, const hkClassMember& definition)
{
	const auto& arraytype = definition.getArrayType();
	const auto& arraysubtype = definition.getSubType();
	if (definition.hasClass() && hkClassMember::TYPE_POINTER != arraysubtype)
	{
		hkVariant v;
		v.m_class = definition.getClass();
		v.m_object = object;
		HkxTableVariant h(v);
		RowCalculator r;
		h.accept(r);
		_rows += r.rows();
	}
	else {
		_rows += 1;
	}
}

void  RowCalculator::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type)
{
	_rows += 1;
}

void  RowCalculator::visit(void* object, const hkClass& object_type, const char* member_name)
{
	if (&object_type != &hkReferencedObjectClass)
	{
		hkVariant v;
		v.m_class = &object_type;
		v.m_object = object;
		HkxTableVariant h(v);
		RowCalculator r;
		h.accept(r);
		_rows += r.rows();
	}
	else {
		_rows += 1;
	}
}

void  RowCalculator::visit(void* v, const hkClassEnum& enum_type, size_t storage)
{
	_rows += 1;
}