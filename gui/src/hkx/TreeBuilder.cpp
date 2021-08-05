#include "TreeBuilder.h"

using namespace ckcmd::HKX;

TreeBuilder::TreeBuilder(ProjectNode* parent, ResourceManager& resourceManager, const fs::path& file, std::set<void*>& visited_objects)
	: _resourceManager(resourceManager), _parent(parent), _file(file), _visited_objects(visited_objects),
	HkxConcreteVisitor(*this)
{

}

void TreeBuilder::visit(char* value) {}

void TreeBuilder::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	ProjectNode* member_node = _parent->appendChild(
		ProjectNode::createHkxFieldNode({ QString::fromStdString(_classmember->getName()) }, _parent));
	int object_index = _resourceManager.findIndex(_file, (const void*)*(uintptr_t*)(v));
	if (object_index >= 0)
	{
		ProjectNode* object_node = member_node->appendChild(
			ProjectNode::createHkxNode(
				{ 
					QString::fromStdString(_resourceManager.at(_file, object_index)->m_class->getName()), 
					(unsigned long long)_resourceManager.at(_file, object_index)
				}, 
			member_node));
		HkxTableVariant h(*const_cast<hkVariant*>(_resourceManager.at(_file, object_index)));
		TreeBuilder t(object_node, _resourceManager, _file, _visited_objects);
		h.accept(t);
	}
}
void TreeBuilder::visit(void* object, const hkClassMember& definition) {
	//array
	if (_visited_objects.insert(object).second)
	{
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
						HkxTableVariant h(v);
						TreeBuilder t(_parent, _resourceManager, _file, _visited_objects);
						h.accept(t);
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
}

void TreeBuilder::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) {}

void TreeBuilder::visit(void* object, const hkClass& object_type, const char* member_name)
{
	if (_visited_objects.insert(object).second)
	{
		hkVariant v;
		v.m_class = &object_type;
		v.m_object = object;
		HkxTableVariant h(v);
		TreeBuilder t(_parent, _resourceManager, _file, _visited_objects);
		h.accept(t);
	}
}

void TreeBuilder::visit(void* v, const hkClassEnum& enum_type, size_t storage) {}