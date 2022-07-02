#include <src/hkx/ArrayFinder.h>

#include <hkbGenerator_0.h>
#include <hkbModifier_0.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbBlenderGeneratorChild_2.h>

#include <typeinfo>

using namespace ckcmd::HKX;

ArrayFinder::ArrayFinder() :
	HkxConcreteVisitor(*this)
{
}

const std::vector<int>& ArrayFinder::arrayrows() const
{
	return _arrayrows;
}

void ArrayFinder::visit(char* value) {}

void ArrayFinder::visit(void* object, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	if (!hkReferencedObjectClass.isSuperClass(pointer_type))
	{
		hkVariant v;
		v.m_class = &pointer_type;
		v.m_object = object;
		HkxVariant h(v);
		h.accept(*this);
	}
	else {
		_row += 1;
	}
}

void ArrayFinder::visit(void* object, const hkClassMember& definition) {

	const auto& arraytype = definition.getArrayType();
	const auto& arrayclass = definition.getStructClass();
	const auto& arraysubtype = definition.getSubType();
	int elements = definition.getSizeInBytes() / definition.getArrayMemberSize();
	char* base = (char*)object;
	if (base != NULL)
	{
		if (hkClassMember::TYPE_ARRAY == definition.getType())
		{
			base = (char*)*(uintptr_t*)object; //m_data
			elements = *(int*)((char*)object + sizeof(uintptr_t)); // m_size
			_arrayrows.push_back(_row);
		}
		bool recursion = false;
		if (base != NULL && elements > 0)
		{
			for (int i = 0; i < elements; i++) {
				void* element = base + i * definition.getArrayMemberSize();
				if (definition.hasClass()) {
					if (hkClassMember::TYPE_POINTER != arraysubtype)
					{
						hkVariant v;
						v.m_class = definition.getClass();
						v.m_object = element;
						HkxVariant h(v);
						h.accept(*this);
						recursion = true;
					}
				}
				else if (hkClassMember::TYPE_POINTER == arraytype) {
					recurse(element, arraytype);
					recursion = true;
				}
			}
		}
		if (!recursion)
			_row += 1;
	}
}

void ArrayFinder::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) 
{
	_row += 1;
}

void ArrayFinder::visit(void* object, const hkClass& object_type, const char* member_name)
{
	hkVariant v;
	v.m_class = &object_type;
	v.m_object = object;
	visit(v.m_object, *v.m_class, hkClassMember::Flags());
}

void ArrayFinder::visit(void* v, const hkClassEnum& enum_type, size_t storage) 
{
	_row += 1;
}