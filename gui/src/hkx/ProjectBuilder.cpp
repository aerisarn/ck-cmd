#include "ProjectBuilder.h"
#include <src/hkx/TreeBuilder.h>

#include <hkbProjectStringData_1.h>
#include <hkbCharacterStringData_5.h>
#include <hkbBehaviorGraph_1.h>

using namespace ckcmd::HKX;

ProjectBuilder::ProjectBuilder(
	ProjectNode* parent,
	ResourceManager& resourceManager,
	const std::string& name
) :
	_parent(parent),
	_resourceManager(resourceManager),
	_name(name)
{
	auto project_path = _resourceManager.open(_name);
	auto project_folder = project_path.parent_path();
	auto& project_content = _resourceManager.get(project_path);
	int root_variant_index = -1;
	for (int v = 0; v < project_content.second.getSize(); v++)
	{
		if (project_content.second[v].m_object == project_content.first)
		{
			root_variant_index = v;
		}
	}
	if (root_variant_index == -1)
		throw std::runtime_error("Root variant not found in " + project_path.string());

	ProjectNode* project_node = ProjectNode::createSupport({"Projects", project_path.string().c_str() }, _parent);
	_parent->appendChild(project_node);
	HkxTableVariant h(project_content.second[root_variant_index]);
	std::set<void*> visited;
	TreeBuilder b(project_node, _resourceManager, project_path, visited);
	h.accept(b);
	/*
	//load file
	auto& content = _resourceManager.get(_file);


	
	*/
	visited.clear();
	TreeBuilder builder(project_node, _resourceManager, project_path, visited);
	
	//Load characters
	ProjectNode* characters_node = ProjectNode::createSupport({ "Characters" }, _parent);
	_parent->appendChild(characters_node);
	for (const auto& block : project_content.second) {
		if (block.m_class == &hkbProjectStringDataClass) {
			hkbProjectStringData* string_data = (hkbProjectStringData*)block.m_object;
			for (int c= 0; c < string_data->m_characterFilenames.getSize(); c++ )
			{
				auto character_path = project_folder / string_data->m_characterFilenames[c].cString();
				auto& character_content = _resourceManager.get(character_path);
				int root_variant_index = -1;
				int string_data_index = -1;
				for (int v = 0; v < character_content.second.getSize(); v++)
				{
					if (character_content.second[v].m_object == character_content.first)
					{
						root_variant_index = v;
					}
					if (character_content.second[v].m_class == &hkbCharacterStringDataClass)
					{
						string_data_index = v;
					}
				}
				if (root_variant_index == -1)
					throw std::runtime_error("Root variant not found in " + project_path.string());
				if (string_data_index == -1)
					throw std::runtime_error("hkbCharacterStringData variant not found in " + project_path.string());
				hkbCharacterStringData* character_data = (hkbCharacterStringData*)(character_content.second[string_data_index].m_object);
				ProjectNode* character_node = ProjectNode::createCharacter({ character_data->m_name.cString(), character_path.string().c_str() }, characters_node);
				characters_node->appendChild(character_node);
				HkxTableVariant character_variant(character_content.second[root_variant_index]);
				visited.clear();
				TreeBuilder character_builder(character_node, _resourceManager, character_path, visited);
				character_variant.accept(character_builder);

				auto behavior_path = project_folder / character_data->m_behaviorFilename.cString();
				auto& behavior_content = _resourceManager.get(behavior_path);
				root_variant_index = -1;
				string_data_index = -1;
				for (int v = 0; v < behavior_content.second.getSize(); v++)
				{
					if (behavior_content.second[v].m_object == behavior_content.first)
					{
						root_variant_index = v;
					}
					if (behavior_content.second[v].m_class == &hkbBehaviorGraphClass)
					{
						string_data_index = v;
					}
				}
				hkbBehaviorGraph* graph_node = (hkbBehaviorGraph*)behavior_content.second[string_data_index].m_object;
				ProjectNode* behavior_node = ProjectNode::createBehavior({ graph_node->m_name.cString(), behavior_path.string().c_str() }, character_node);
				character_node->appendChild(behavior_node);
				HkxTableVariant behavior_variant(behavior_content.second[root_variant_index]);
				visited.clear();
				TreeBuilder behavior_builder(behavior_node, _resourceManager, behavior_path, visited);
				behavior_variant.accept(behavior_builder);

			}
		}
	}

}