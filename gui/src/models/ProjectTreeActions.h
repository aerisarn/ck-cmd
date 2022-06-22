#pragma once

#include <src/models/ProjectModel.h>

namespace ckcmd {
    namespace HKX {

        class ProjectTreeActions
        {
            ProjectModel& _model;

        public:

            ProjectTreeActions(ProjectModel& _model);

            //These actions do not possess undo/redo 
            //you have to explicitly do the opposite to undo them
            void CreateProject();
            void DeleteProject();
        };
    }
}