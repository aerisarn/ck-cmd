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

			QAction* _copy;

			void buildSaveAction();
			void buildImportFBXAction();
			void buildExportFBXAction();
			void buildRemoveAnimation();
			void buildCreateProjectAction();
			void buildCopyAction();

		public slots:
			void save();

			void createProject();
			void importFBX();
			void exportFBX();
			void removeAnimation();
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
			}



			QAction* saveAction(const QVariant& action_data);
			std::vector<QAction*> addActions(const QVariant& action_data);
			QAction* exportFBXAction() { return _exportFBX; }
			QAction* importFBXAction() { return _importFBX; }
			QAction* removeAnimationAction() { return _removeAnimation; }
			QAction* createProjectAction() { return _createProject; }
			QAction* copyAction() { return _copy; }

		};
	}
}