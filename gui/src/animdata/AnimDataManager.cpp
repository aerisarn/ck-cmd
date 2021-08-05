#include "AnimDataManager.h"

#include <iostream>

using namespace ckcmd::HKX;

AnimationManager::AnimationManager(const fs::path& workspace_folder) : 
	_workspace_folder(workspace_folder),
	_data_folder(_workspace_folder / "animationdata"),
	_sets_folder(_workspace_folder / "animationsetdata")
{
	LOG << "Opening " << _workspace_folder.string() << log_endl;

	std::set<std::string> projects;

	if (fs::exists(_data_folder))
	{
		for (auto& p : fs::directory_iterator(_data_folder))
		{
			if (fs::is_regular_file(p.path())
				&& p.path().extension() == ".txt"
				&& p.path().string().find("dirlist") == std::string::npos)
			{
				std::string project_name = p.path().filename().replace_extension("").string();
				if (fs::exists(_data_folder / "boundanims") &&
					fs::is_regular_file(_data_folder / "boundanims" / (std::string("anims_") + project_name + ".txt")))
				{
					_characters.insert(project_name);
				}
				else {
					_miscellanous.insert(project_name);
				}
			}
		}
	}
}

void AnimationManager::buildProjectTree(ProjectNode* root)
{
	ProjectNode* charactersNode = ProjectNode::createSupport({ "Characters" }, root);
	root->appendChild(charactersNode);
	for (const auto& character : _characters) {
		charactersNode->appendChild(
			ProjectNode::createCharacter({ QString::fromStdString(character) }, charactersNode)
		);
	}
	ProjectNode* miscNode = ProjectNode::createSupport({ "Miscellaneous" }, root);
	root->appendChild(miscNode);
	for (const auto& misc : _miscellanous) {
		miscNode->appendChild(
			ProjectNode::createMisc({ QString::fromStdString(misc) }, miscNode)
		);
	}
}