#include "Setter.h"
#include "HkxItemPointer.h"
#include "HkxItemEnum.h"
#include "HkxItemFlags.h"
#include <stdexcept>

using namespace ckcmd::HKX;


void Setter::visit(char* value) {
	if (_row == 0)
		throw std::runtime_error("Cannot set a constant string!");
	_row -= 1;
}

void Setter::check(void* value) {
	_row -= 1;
}

void Setter::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	if (_row == 0)
		*(uintptr_t*)v = (uintptr_t)_value.value<HkxItemPointer>().get();
	_row -= 1;
}

void Setter::visit(void* object, const hkClassMember& definition)
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
		HkxTableVariant h(v);
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
					HkxTableVariant h(v);
					h.accept(*this);
				}
				else {
					*(uintptr_t*)element = (uintptr_t)_value.value<HkxItemPointer>().get();
				}
			}
			_row -= array_rows;
		}
	}
	else {
		_row -= array_rows;
	}
}

void Setter::visit(void* value, const hkClassEnum& enum_type, hkClassMember::Type type)
{
	if (_row == 0)
	{
		int enum_value = _value.value<HkxItemEnum>().value();

		switch (type) {
		case hkClassMember::TYPE_CHAR:
			*(char*)value = (char)enum_value;
			break;
		case hkClassMember::TYPE_INT8:
			*(unsigned char*)value = (unsigned char)enum_value;
			break;
		case hkClassMember::TYPE_UINT8:
			*(char*)value = (char)enum_value;
			break;
		case hkClassMember::TYPE_INT16:
			*(short*)value = _byteswap_ushort((short)enum_value);
			break;
		case hkClassMember::TYPE_UINT16:
			*(unsigned short*)value = _byteswap_ushort((unsigned short)enum_value);
			break;
		case hkClassMember::TYPE_INT32:
			*(int*)value = _byteswap_ulong(enum_value);
			break;
		case hkClassMember::TYPE_UINT32:
			*(unsigned int*)value = _byteswap_ulong((unsigned int)enum_value);
			break;
		default:
			throw std::runtime_error(std::string("Unknown storage type for enum: ") + std::to_string(type));
		}
	}
	_row -= 1;
}



void Setter::visit(void* object, const hkClass& object_type, const char* member_name)
{
	size_t array_rows = 1;
	if (&object_type != &hkReferencedObjectClass)
	{ 
		hkVariant v;
		v.m_class = &object_type;
		v.m_object = object;
		HkxTableVariant h(v);
		RowCalculator r;
		h.accept(r);
		array_rows = r.rows();
		int next_rows = _row - array_rows;
		if (_row >=0 && next_rows < 0)
		{
			h.accept(*this);
		}
	}
	else {
		*(uintptr_t*)object = (uintptr_t)_value.value<HkxItemPointer>().get();
	}
	_row -= array_rows;
}

void Setter::visit(void* value, const hkClassEnum& enum_type, size_t storage)
{
	if (_row == 0)
	{
		//char* name = new char(256);
		int enum_value = _value.value<HkxItemFlags>().value();
		switch (storage)
		{
		case 1:
			*(unsigned char*)value = (unsigned char)enum_value;
			break;
		case 2:
			*(unsigned short*)value = _byteswap_ushort((unsigned short)enum_value);
			break;
		default:
			*(unsigned int*)value = _byteswap_ulong((unsigned int)enum_value);
			break;
		}
	}
	_row -= 1;
}