#include "ColumnCalculator.h"

#include "HkxTableVariant.h"

using namespace ckcmd::HKX;

void ColumnCalculator::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	fit(1);
}

void ColumnCalculator::visit(void* object, const hkClassMember& definition)
{
	const auto& arrayclass = definition.getStructClass();
	const auto& arraytype = definition.getArrayType();
	const auto& arraysubtype = definition.getSubType();

	size_t num_elements = definition.getSizeInBytes() / definition.getArrayMemberSize();
	if (hkClassMember::TYPE_ARRAY == definition.getType())
	{
		num_elements = (*(int*)((char*)object + sizeof(uintptr_t))) & int(0x3FFFFFFF); // m_size
	}
	if (definition.hasClass() && hkClassMember::TYPE_POINTER != arraysubtype)
	{
		hkVariant v;
		v.m_class = definition.getClass();
		v.m_object = object;
		HkxTableVariant h(v);
		ColumnCalculator r;
		h.accept(r);
		for (int i = 0; i < r.rows(); i++)
			fit(r.column(i) > num_elements ? r.column(i) : num_elements);
	}
	else {
		fit(num_elements);
	}
}

void ColumnCalculator::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type)
{
	fit(1);
}

void ColumnCalculator::visit(void* object, const hkClass& object_type, const char* member_name)
{
	size_t num_elements = 0;
	hkVariant v;
	v.m_class = &object_type;
	v.m_object = object;
	HkxTableVariant h(v);
	ColumnCalculator r;
	h.accept(r);
	for (int i = 0; i < r.rows(); i++)
		fit(r.column(i));
}

void ColumnCalculator::visit(void* v, const hkClassEnum& enum_type, size_t storage)
{
	fit(1);
}