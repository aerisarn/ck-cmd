#include "ReferenceSetter.h"
#include <src/items/HkxItemPointer.h>
#include <src/items/HkxItemEnum.h>
#include <src/items/HkxItemFlags.h>
#include <stdexcept>

using namespace ckcmd::HKX;


void ReferenceSetter::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
}

void ReferenceSetter::visit(void* object, const hkClassMember& definition)
{
	//array
	const auto& arraytype = definition.getArrayType();
	const auto& arraysubtype = definition.getSubType();
	const auto& arrayclass = definition.getStructClass();
	size_t elements = definition.getSizeInBytes() / definition.getArrayMemberSize();
	char* base = (char*)object;
	if (hkClassMember::TYPE_ARRAY == definition.getType())
	{
		base = (char*)*(uintptr_t*)object; //m_data
		elements = *(int*)((char*)object + sizeof(uintptr_t)) & int(0x3FFFFFFF); // m_size
	}
	for (int i = 0; i < elements; ++i)
	{
		void* element = (void*)((char*)base + i * definition.getArrayMemberSize());
		if (!definition.hasClass()) {
			if (base != NULL)
				recurse(element, arraytype);
		}
		else {
			if (element != NULL)
			{
				if (hkClassMember::TYPE_POINTER != arraysubtype)
				{
					hkVariant v;
					v.m_class = &arrayclass;
					v.m_object = element;
					HkxVariant h(v);
					h.accept(*this);
				}
			}
		}
	}
}

void ReferenceSetter::visit(void* value, const hkClassEnum& enum_type, hkClassMember::Type type)
{
}



void ReferenceSetter::visit(void* object, const hkClass& object_type, const char* member_name)
{
	if (&object_type != &hkReferencedObjectClass)
	{ 
		hkVariant v;
		v.m_class = &object_type;
		v.m_object = object;
		HkxVariant h(v);
		h.accept(*this);
	}
}

void ReferenceSetter::visit(void* value, const hkClassEnum& enum_type, size_t storage)
{
}