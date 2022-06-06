#include <src/hkx/LinkGetter.h>

using namespace ckcmd::HKX;

LinkGetter::LinkGetter(size_t file, ResourceManager& resourceManager)
	: _resourceManager(resourceManager), _file(file),
	HkxConcreteVisitor(*this)
{
}

void LinkGetter::visit(char* value) {}

void LinkGetter::visit(void* object, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	hkVariant v;
	v.m_class = &pointer_type;
	v.m_object = object;
	_links.push_back(v);
}

void LinkGetter::visit(void* object, const hkClassMember& definition) {

	const auto& arraytype = definition.getArrayType();
	const auto& arrayclass = definition.getStructClass();
	const auto& arraysubtype = definition.getSubType();
	int elements = definition.getSizeInBytes() / definition.getArrayMemberSize();
	char* base = (char*)object;
	if (hkClassMember::TYPE_ARRAY == definition.getType())
	{
		base = (char*)*(uintptr_t*)object; //m_data
		elements = *(int*)((char*)object + sizeof(uintptr_t)); // m_size
	}
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
				}
				else {
					size_t ref_index = _resourceManager.findIndex(_file, (const void*)*(uintptr_t*)(element));
					hkVariant* v = _resourceManager.at(_file, ref_index);
					visit(element, *v->m_class, hkClassMember::Flags());
				}
			}
			else if (hkClassMember::TYPE_POINTER == arraytype) {
				recurse(element, arraytype);
			}
		}
	}
}

void LinkGetter::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) {}

void LinkGetter::visit(void* object, const hkClass& object_type, const char* member_name)
{
	hkVariant v;
	v.m_class = &object_type;
	v.m_object = object;
	visit(v.m_object, *v.m_class, hkClassMember::Flags());
}

void LinkGetter::visit(void* v, const hkClassEnum& enum_type, size_t storage) {}