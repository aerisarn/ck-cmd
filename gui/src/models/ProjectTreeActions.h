#pragma once

#include "ProjectNode.h"
#include <src/hkx/CommandManager.h>
#include <src/hkx/ResourceManager.h>

namespace ckcmd {
    namespace HKX {

        class ProjectTreeActions
        {
            CommandManager& _commandManager;
            ResourceManager& _resourceManager;

        public:

            ProjectTreeActions(CommandManager& commandManager, ResourceManager& resourceManager);

            //These actions do not possess undo/redo 
            //you have to explicitly do the opposite to undo them

            void OpenProject(ProjectNode* projectNode);
            void CloseProject(ProjectNode* projectNode);
            void CreateProject();
            void DeleteProject();

        };
    }
}