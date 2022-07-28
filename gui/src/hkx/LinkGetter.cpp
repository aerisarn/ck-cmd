#include <src/hkx/LinkGetter.h>

#include <hkbGenerator_0.h>
#include <hkbModifier_0.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbBlenderGeneratorChild_2.h>

#include <typeinfo>

using namespace ckcmd::HKX;

LinkGetter::LinkGetter() :
	HkxConcreteVisitor(*this)
{
}

const std::vector<Link>& LinkGetter::links() const
{
	return _links;
}

bool isValidLinkClass(void* ptr)
{
	auto obj = reinterpret_cast<hkReferencedObject*>(ptr);
	hkbGenerator* gen = nullptr;
	hkbModifier* mod = nullptr;
	try {
		gen = dynamic_cast<hkbGenerator*>(obj);
	}
	catch (std::__non_rtti_object) {
	}
	try {
		mod = dynamic_cast<hkbModifier*>(obj);
	}
	catch (std::__non_rtti_object) {
	}

	if (nullptr != gen || nullptr != mod)
		return true;

	return false;
}

void LinkGetter::visit(char*& value) {}

void LinkGetter::visit(void* object, const hkClass& pointer_type, hkClassMember::Flags flags)
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
		auto ptr = (void*)*(uintptr_t*)object;
		if (ptr != nullptr)
		{
			if (ptr != nullptr && isValidLinkClass(ptr))
			{
				_links.push_back({ _row, 0, ptr });
			}
		}
		_row += 1;
	}
}

void LinkGetter::visit(void* object, const hkClassMember& definition) {

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
			if (elements > 10000)
				__debugbreak();
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
					else {
						auto ptr = (void*)*(uintptr_t*)element;
						if (ptr != nullptr && (&arrayclass == &hkbStateMachineStateInfoClass || &arrayclass == &hkbBlenderGeneratorChildClass || isValidLinkClass(ptr)))
						{
							_links.push_back({ _row, i, ptr });	
						}
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

void LinkGetter::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) 
{
	_row += 1;
}

void LinkGetter::visit(void* object, const hkClass& object_type, const char* member_name)
{
	hkVariant v;
	v.m_class = &object_type;
	v.m_object = object;
	visit(v.m_object, *v.m_class, hkClassMember::Flags());
}

void LinkGetter::visit(void* v, const hkClassEnum& enum_type, size_t storage) 
{
	_row += 1;
}