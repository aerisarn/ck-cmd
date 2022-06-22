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
			QAction* _removeAnimation;

			void buildSaveAction();
			void buildImportFBXAction();
			void buildRemoveAnimation();
			void buildCreateProjectAction();

		public slots:
			void save();

			void createProject();
			void importFBX();
			void removeAnimation();

		public:
			ActionHandler(ProjectModel& model, QObject* parent = nullptr) :
				QObject(parent),
				_save(nullptr),
				_model(model)
			{
				buildSaveAction();
				buildImportFBXAction();
				buildRemoveAnimation();
			}



			QAction* saveAction() {	return _save;	}
			QAction* exportAction();
			QAction* importFBXAction() { return _importFBX; };
			QAction* removeAnimationAction() { return _removeAnimation; }
			QAction* createProjectAction() { return _createProject; }

		};
	}
}