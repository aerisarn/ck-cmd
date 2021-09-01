#include "Setter.h"
#include "HkxItemPointer.h"
#include "HkxItemEnum.h"
#include "HkxItemFlags.h"
#include <stdexcept>

using namespace ckcmd::HKX;


void Setter::visit(char* value) {
	if (_row == 0)
		_value = QString::fromStdString(value);
	_row -= 1;
}

void Setter::check(void* value) {
	_row -= 1;
}

void Setter::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	if (_row == 0)
		_value.setValue(HkxItemPointer(_file_index, (void*)*(uintptr_t*)v));
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
		elements = *(int*)((char*)object + sizeof(uintptr_t)); // m_size
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
					//Getter g(_row, _column, _file_index, _handlers);
					h.accept(*this);
					//_value = g.value();
				}
				else {
					_value.setValue(HkxItemPointer(_file_index, (void*)*(uintptr_t*)element));
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
		//char* name = new char(256);
		//enum_type.getNameOfValue(*(int*)value, (const char**)&name);
		//_value = QString::fromStdString(name);

		int enum_value = 0;

		switch (type) {
		case hkClassMember::TYPE_CHAR:
			enum_value = *(char*)value;
			break;
		case hkClassMember::TYPE_INT8:
			enum_value = *(unsigned char*)value;
			break;
		case hkClassMember::TYPE_UINT8:
			enum_value = *(char*)value;
			break;
		case hkClassMember::TYPE_INT16:
			enum_value = _byteswap_ushort(*(short*)value);
			break;
		case hkClassMember::TYPE_UINT16:
			enum_value = _byteswap_ushort(*(unsigned short*)value);
			break;
		case hkClassMember::TYPE_INT32:
			enum_value = _byteswap_ulong(*(int*)value);
			break;
		case hkClassMember::TYPE_UINT32:
			enum_value = _byteswap_ulong(*(unsigned int*)value);
			break;
		default:
			throw std::runtime_error(std::string("Unknown storage type for enum: ") + std::to_string(type));
		}
		_value.setValue(HkxItemEnum(enum_value, &enum_type));
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
			//Getter g(_row, _column, _file_index, _handlers);
			h.accept(*this);
			//_value = g.value();
		}
	}
	else {
		_value.setValue(HkxItemPointer(_file_index, (void*)*(uintptr_t*)object));
	}
	_row -= array_rows;
}

void Setter::visit(void* value, const hkClassEnum& enum_type, size_t storage)
{
	if (_row == 0)
	{
		//char* name = new char(256);
		int enum_value = 0;
		switch (storage)
		{
		case 1:
			enum_value = *(unsigned char*)value;
			//enum_type.getNameOfValue(*(unsigned char*)value, (const char**)&name);
			break;
		case 2:
			enum_value = _byteswap_ushort(*(unsigned short*)value);
			//enum_type.getNameOfValue(_byteswap_ushort(*(unsigned short*)value), (const char**)&name);
			break;
		default:
			enum_value = _byteswap_ulong(*(unsigned int*)value);
			//enum_type.getNameOfValue(_byteswap_ulong(*(unsigned int*)value), (const char**)&name);
			break;
		}
		_value.setValue(HkxItemFlags(enum_value, &enum_type, storage));
	}
	_row -= 1;
}