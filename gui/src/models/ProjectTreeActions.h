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

            void OpenProject(ProjectNode* projectNode);
        };
    }
}