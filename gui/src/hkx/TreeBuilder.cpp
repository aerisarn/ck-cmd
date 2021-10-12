#include "TreeBuilder.h"

using namespace ckcmd::HKX;

TreeBuilder::TreeBuilder(ProjectNode* parent, ResourceManager& resourceManager, const fs::path& file, std::set<void*>& visited_objects)
	: _resourceManager(resourceManager), _parent(parent), _file(file), _file_index(resourceManager.index(file)), _visited_objects(visited_objects),
	HkxConcreteVisitor(*this)
{
	auto handler = resourceManager.classHandler(_file_index);
	if (NULL != handler)
		registerClassHandler(handler);
}

TreeBuilder::TreeBuilder(
	ProjectNode* parent,
	ResourceManager& resourceManager,
	const fs::path& file,
	std::set<void*>& visited_objects,
	const std::map<const hkClass*, ITreeBuilderClassHandler*>& handlers
) : TreeBuilder(parent, resourceManager, file, visited_objects)
{
	_handlers = handlers;
}

TreeBuilder::TreeBuilder(
	ProjectNode* parent,
	ResourceManager& resourceManager,
	const fs::path& file,
	size_t file_index,
	std::set<void*>& visited_objects,
	const std::map<const hkClass*, ITreeBuilderClassHandler*>& handlers
) : TreeBuilder(parent, resourceManager, file, visited_objects)
{
	_file_index = file_index;
	_handlers = handlers;
}

void TreeBuilder::visit(char* value) {}

void TreeBuilder::visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags)
{
	int object_index = _resourceManager.findIndex(_file_index, (const void*)*(uintptr_t*)(v));
	if (object_index >= 0)
	{
		auto* variant = _resourceManager.at(_file_index, object_index);
		auto temp_member = variant->m_class->getMemberByName("name");
		//if (HK_NULL != temp_member)
		//{
		//	auto member_ptr = ((char*)variant->m_object) + temp_member->getOffset();
		//	std::string c_str_ptr = (char*)*(uintptr_t*)(member_ptr);
		//	if (c_str_ptr == "1HM_Solo_Behavior")
		//		__debugbreak();
		//}
		if (_visited_objects.insert(variant->m_object).second)
		{		
			ProjectNode* object_node = NULL;
			if (_handlers.find(variant->m_class) != _handlers.end())
			{
				object_node = _handlers[variant->m_class]->visit(
					_file,
					object_index,
					_parent
				);
			}
			else {

				QString display_name = variant->m_class->getName();
				//check if the object has a name we can display
				auto member = variant->m_class->getMemberByName("name");
				if (HK_NULL != member)
				{
					auto member_ptr = ((char*)variant->m_object) + member->getOffset();
					auto c_str_ptr = (char*)*(uintptr_t*)(member_ptr);
					display_name = QString("%1 \"%2\"").arg(display_name).arg(c_str_ptr);
				}

				QString name = QString("[%1] %2").arg(object_index).arg(display_name);
				object_node = _parent->appendChild(
					_resourceManager.createHkxNode(
						_file_index,
						{
							name,
							(unsigned long long)variant,
							(unsigned long long)_parent->isVariant() ? _parent->data(1) : 0,
							_file_index
						},
						_parent));
			}
			HkxTableVariant h(*variant);
			h.accept(TreeBuilder(object_node, _resourceManager, _file, _file_index, _visited_objects, _handlers));
		}
		else {
			auto node = _resourceManager.findNode(_file_index, variant);
			node->addParent(_parent);
			_parent->appendChild(node);
		}
	}
}
void TreeBuilder::visit(void* object, const hkClassMember& definition) {
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
					h.accept(*this);
				}
				else {
					size_t ref_index = _resourceManager.findIndex(_file_index, (const void*)*(uintptr_t*)(element));
					hkVariant* v = _resourceManager.at(_file_index, ref_index);
					visit(element, *v->m_class, hkClassMember::Flags());
				}
			}
			else if (hkClassMember::TYPE_POINTER == arraytype) {
				recurse(element, arraytype);
			}
		}
	}
}

void TreeBuilder::visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) {}

void TreeBuilder::visit(void* object, const hkClass& object_type, const char* member_name)
{
	hkVariant v;
	v.m_class = &object_type;
	v.m_object = object;
	HkxTableVariant h(v);
	h.accept(*this);
}

void TreeBuilder::visit(void* v, const hkClassEnum& enum_type, size_t storage) {}