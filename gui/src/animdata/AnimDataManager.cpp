#include "AnimDataManager.h"

#include <iostream>

using namespace ckcmd::HKX;

void AnimationManager::scanWorkspace()
{
	
	for (auto& p : fs::recursive_directory_iterator(_workspace.getFolder()))
	{
		if (fs::is_regular_file(p.path())
			&& (p.path().extension() == ".hkx" || p.path().extension() == ".xml")
			)
		{
			//LOG << "Analyzing file ... " << p.path().string() << log_endl;
			if (_resource_manager.isHavokProject(p.path())) {
				string sanitized_project_name = p.path().filename().replace_extension("").string();
				//LOG << "Found project " << sanitized_project_name << log_endl;
				CacheEntry* entry = _resource_manager.findCacheEntry(sanitized_project_name);
				if (NULL == entry)
				{
					LOG << " WARNING: " << sanitized_project_name << " was not found into the animation cache. The project won't be loaded by the game" << log_endl;
				}
				else {
					if (entry->hasCache()) {
						//LOG << "Project is a creature" << sanitized_project_name << log_endl;
						_workspace.addCharacterProject(p.path().string().c_str(), sanitized_project_name.c_str());

					}
					else {
						//LOG << "Project is miscellaneous" << sanitized_project_name << log_endl;
						_workspace.addMiscellaneousProject(p.path().string().c_str(), sanitized_project_name.c_str());
					}
				}
			}
		}
	}
}

AnimationManager::AnimationManager(WorkspaceConfig& workspace, ResourceManager& resource_manager) :
	_resource_manager(resource_manager),
	_workspace(workspace)
{
	LOG << "Opening " << _workspace.getFolder() << log_endl;

	if (_workspace.empty())
	{
		LOG << "Workspace has no projects, scanning ... " << _workspace.getFolder() << log_endl;
		scanWorkspace();
	}
}

void AnimationManager::buildProjectTree(ProjectNode* root)
{
	ProjectNode* charactersNode = _resource_manager.createStatic({ "Characters" }, root);
	root->appendChild(charactersNode);
	auto _characters = _workspace.getCharacterProjects();
	for (const auto& character : _characters) {
		charactersNode->appendChild(
			_resource_manager.createStatic({ character }, charactersNode, ProjectNode::NodeType::character_node)
		);
	}
	ProjectNode* miscNode = _resource_manager.createStatic({ "Miscellaneous" }, root);
	root->appendChild(miscNode);
	auto _miscellanous = _workspace.getMiscellaneousProjects();
	for (const auto& misc : _miscellanous) {
		miscNode->appendChild(
			_resource_manager.createStatic({ misc }, miscNode, ProjectNode::NodeType::misc_node)
		);
	}
}