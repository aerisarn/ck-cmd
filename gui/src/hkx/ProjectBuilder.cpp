#include "ProjectBuilder.h"
#include <src/hkx/TreeBuilder.h>

#include <hkbProjectStringData_1.h>
#include <hkbCharacterStringData_5.h>
#include <hkbBehaviorGraph_1.h>

using namespace ckcmd::HKX;

void ProjectBuilder::buildBranch(hkVariant& root, ProjectNode* root_node, const fs::path& path) {
	HkxTableVariant h(root);
	std::set<void*> visited;
	TreeBuilder b(root_node, _resourceManager, path, visited);
	h.accept(b);
}

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
	hkVariant project_root;
	hkbProjectStringData* project_data = loadHkxFile<hkbProjectStringData>(project_path, hkbProjectStringDataClass, project_root);
	ProjectNode* project_node = ProjectNode::createSupport({"Projects", project_path.string().c_str() }, _parent);
	_parent->appendChild(project_node);
	buildBranch(project_root, project_node, project_path);
	
	if (project_data)
	{
		ProjectNode* characters_node = ProjectNode::createSupport({ "Characters" }, _parent);
		_parent->appendChild(characters_node);
		//Load characters
		for (int c = 0; c < project_data->m_characterFilenames.getSize(); c++)
		{
			//character data
			auto character_path = project_folder / project_data->m_characterFilenames[c].cString();
			hkVariant character_root;
			hkbCharacterStringData* character_data = loadHkxFile<hkbCharacterStringData>(character_path, hkbCharacterStringDataClass, character_root);
			if (character_data == NULL)
				throw std::runtime_error("hkbCharacterStringData variant not found in " + character_path.string());
			ProjectNode* character_node = ProjectNode::createCharacter({ character_data->m_name.cString(), character_path.string().c_str() }, characters_node);
			characters_node->appendChild(character_node);
			buildBranch(character_root, character_node, character_path);

			//behavior
			auto behavior_path = project_folder / character_data->m_behaviorFilename.cString();
			hkVariant behavior_root;
			hkbBehaviorGraph* behavior_data = loadHkxFile<hkbBehaviorGraph>(behavior_path, hkbBehaviorGraphClass, behavior_root);
			if (character_data == NULL)
				throw std::runtime_error("hkbBehaviorGraph variant not found in " + behavior_path.string());
			ProjectNode* behavior_node = ProjectNode::createBehavior({ behavior_data->m_name.cString(), behavior_path.string().c_str() }, character_node);
			character_node->appendChild(behavior_node);
			buildBranch(behavior_root, behavior_node, behavior_path);

			//skeleton
			if (!std::string(character_data->m_rigName).empty())
			{
				auto rig_path = project_folder / character_data->m_rigName.cString();
				auto& rig_contents = _resourceManager.get(rig_path);
				ProjectNode* rig_node = ProjectNode::createSkeleton({ "Skeleton", rig_path.string().c_str() }, character_node);
				character_node->appendChild(rig_node);
				buildBranch(rig_contents.first, rig_node, rig_path);
			}

			//animations
			if (character_data->m_animationNames.getSize() > 0)
			{
				ProjectNode* animations_node = ProjectNode::createSupport({ "Animations" }, character_node);
				character_node->appendChild(animations_node);
				for (int a = 0; a < character_data->m_animationNames.getSize(); a++)
				{
					auto animation_path = project_folder / character_data->m_animationNames[a].cString();
					auto& animation_contents = _resourceManager.get(animation_path);
					ProjectNode* animation_node = ProjectNode::createAnimation({ character_data->m_animationNames[a].cString(), animation_path.string().c_str() }, animations_node);
					animations_node->appendChild(animation_node);
					buildBranch(animation_contents.first, animation_node, animation_path);
				}
			}
		}
	}
}