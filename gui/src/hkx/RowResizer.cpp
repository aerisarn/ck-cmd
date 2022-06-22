#include "RowResizer.h"
#include "HkxItemPointer.h"
#include "HkxItemEnum.h"
#include "HkxItemFlags.h"
#include <stdexcept>

using namespace ckcmd::HKX;


void RowResizer::visit(char* value) {
	_row -= 1;
}

void RowResizer::check(void* value) {
	_row -= 1;
}

void RowResizer::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	_row -= 1;
}

void RowResizer::visit(void* object, const hkClassMember& definition)
{
	//array
	const auto& arraytype = definition.getArrayType();
	const auto& arraysubtype = definition.getSubType();
	const auto& arrayclass = definition.getStructClass();
	size_t array_rows = 1;
	if (definition.hasClass() && hkClassMember::TYPE_POINTER != arraysubtype) {
		hkVariant v;
		v.m_class = definition.getClass();
		v.m_object = object;
		HkxVariant h(v);
		RowCalculator r;
		h.accept(r);
		array_rows = r.rows();
	}
	size_t elements = definition.getSizeInBytes() / definition.getArrayMemberSize();
	char* base = (char*)object;
	if (hkClassMember::TYPE_ARRAY == definition.getType())
	{
		base = (char*)*(uintptr_t*)object; //m_data
		elements = *(int*)((char*)object + sizeof(uintptr_t)) & int(0x3FFFFFFF); // m_size
	}
	if (_column < elements)
	{
		void* element = (void*)((char*)base + _column * definition.getArrayMemberSize());
		if (!definition.hasClass()) {
			if (base != NULL)
				recurse(element, arraytype);
			else
				_row -= 1;
		}
		else {
			int next_rows = _row - array_rows;
			if (element != NULL && _row >= 0 && next_rows < 0)
			{
				if (hkClassMember::TYPE_POINTER != arraysubtype)
				{
					hkVariant v;
					v.m_class = &arrayclass;
					v.m_object = element;
					HkxVariant h(v);
					h.accept(*this);
				}
				else {
					//*(uintptr_t*)element = (uintptr_t)_value.value<HkxItemPointer>().get();
				}
			}
			_row -= array_rows;
		}
	}
	else {
		_row -= array_rows;
	}
}

void RowResizer::visit(void* value, const hkClassEnum& enum_type, hkClassMember::Type type)
{
	_row -= 1;
}



void RowResizer::visit(void* object, const hkClass& object_type, const char* member_name)
{
	size_t array_rows = 1;
	if (&object_type != &hkReferencedObjectClass)
	{ 
		hkVariant v;
		v.m_class = &object_type;
		v.m_object = object;
		HkxVariant h(v);
		RowCalculator r;
		h.accept(r);
		array_rows = r.rows();
		int next_rows = _row - array_rows;
		if (_row >=0 && next_rows < 0)
		{
			h.accept(*this);
		}
	}
	//else {
	//	*(uintptr_t*)object = (uintptr_t)_value.value<HkxItemPointer>().get();
	//}
	_row -= array_rows;
}

void RowResizer::visit(void* value, const hkClassEnum& enum_type, size_t storage)
{
	_row -= 1;
}