#include "ProjectTreeActions.h"
#include <src/hkx/ProjectBuilder.h>

using namespace ckcmd::HKX;

ProjectTreeActions::ProjectTreeActions(CommandManager& commandManager, ResourceManager& resourceManager) :
	_commandManager(commandManager),
	_resourceManager(resourceManager)
{
}

void ProjectTreeActions::OpenProject(ProjectNode* projectNode)
{
	ProjectBuilder b(
		projectNode,
		_commandManager,
		_resourceManager,
		projectNode->name()
	);
}

void ProjectTreeActions::CloseProject(ProjectNode* projectNode)
{

}