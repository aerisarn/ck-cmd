#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/models/ProjectTreeModel.h>
#include <src/models/ProjectTreeActions.h>

#include <QObject>
#include <QMenu>
#include <QTreeView>

namespace ckcmd {
	namespace HKX {

		class ActionHandler : public QObject
		{
			Q_OBJECT

			ProjectTreeActions& _treeActions;

			QAction* _save;

			QAction* _createProject;

			QAction* _importFBX;

			void buildSaveAction();
			void buildImportFBXAction();
			void buildCreateProjectAction();

		public slots:
			void save();

			void createProject();
			void importFBX();

			void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

		public:
			ActionHandler(ProjectTreeActions& treeActions, QObject* parent = nullptr) :
				QObject(parent),
				_save(nullptr),
				_treeActions(treeActions)
			{
				buildSaveAction();
				buildImportFBXAction();

				//connect(_modelview.selectionModel(), &QItemSelectionModel::selectionChanged,
				//		this, &ActionHandler::selectionChanged);
			}



			QAction* saveAction() {	return _save;	}
			QAction* exportAction();
			QAction* importFBXAction() { return _importFBX; };

			QAction* createProjectAction() { return _createProject; }

		};
	}
}