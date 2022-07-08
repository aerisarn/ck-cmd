#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/models/ProjectModel.h>

#include <QObject>
#include <QMenu>
#include <QTreeView>

class FBXImport;

namespace ckcmd {
	namespace HKX {

		class ActionHandler : public QObject
		{
			Q_OBJECT

			ProjectModel& _model;

			QAction* _save;

			QAction* _createProject;

			QAction* _importFBX;
			QAction* _exportFBX;
			QAction* _removeAnimation;
			QAction* _addEvent;
			QAction* _removeEvent;
			QAction* _addVariable;
			QAction* _removeVariable;
			QAction* _addProperty;
			QAction* _removeProperty;

			QAction* _copy;

			void buildSaveAction();
			void buildImportFBXAction();
			void buildExportFBXAction();
			void buildRemoveAnimation();
			void buildAddEvent();
			void buildRemoveEvent();
			void buildAddVariable();
			void buildRemoveVariable();
			void buildAddProperty();
			void buildRemoveProperty();
			void buildCreateProjectAction();

			void buildCopyAction();

		public slots:
			void save();

			void createProject();
			void importFBX();
			void exportFBX();
			void removeAnimation();
			void addEvent();
			void removeEvent();
			void addVariable();
			void removeVariable();
			void addProperty();
			void removeProperty();
			void copy();
			void addOrSet();

		public:
			ActionHandler(ProjectModel& model, QObject* parent = nullptr) :
				QObject(parent),
				_save(nullptr),
				_model(model)
			{
				buildSaveAction();
				buildImportFBXAction();
				buildExportFBXAction();
				buildRemoveAnimation();
				buildCopyAction();
				buildAddEvent();
				buildRemoveEvent();
				buildAddVariable();
				buildRemoveVariable();
				buildAddProperty();
				buildRemoveProperty();
			}



			QAction* saveAction(const QVariant& action_data);
			std::vector<QAction*> addActions(const QVariant& action_data);
			QAction* exportFBXAction() { return _exportFBX; }
			QAction* importFBXAction() { return _importFBX; }
			QAction* removeAnimationAction() { return _removeAnimation; }
			QAction* addEventAction() { return _addEvent; }
			QAction* removeEventAction() { return _removeEvent; }
			QAction* addVariableAction() { return _addEvent; }
			QAction* removeVariableAction() { return _removeEvent; }
			QAction* addPropertyAction() { return _addEvent; }
			QAction* removePropertyAction() { return _removeEvent; }
			QAction* createProjectAction() { return _createProject; }
			QAction* copyAction() { return _copy; }

		};
	}
}