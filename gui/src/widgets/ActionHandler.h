#pragma once

#include <src/models/ProjectNode.h>

#include <src/hkx/ResourceManager.h>
#include <src/models/ProjectTreeModel.h>

#include <QObject>
#include <QMenu>
#include <QTreeView>

namespace ckcmd {
	namespace HKX {

		class ActionHandler : public QObject
		{
			Q_OBJECT

			ResourceManager& _resourceManager;
			QTreeView& _modelview;

			QAction* _saveAction;

			void buildSaveAction();

		public slots:
			void save();
			void export_to(ProjectNode* project_node);

			void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

		public:
			ActionHandler(ResourceManager& resourceManager, QTreeView& modelview, QObject* parent = nullptr) :
				QObject(parent),
				_saveAction(nullptr),
				_resourceManager(resourceManager),
				_modelview(modelview)
			{
				buildSaveAction();
				connect(_modelview.selectionModel(), &QItemSelectionModel::selectionChanged,
						this, &ActionHandler::selectionChanged);
			}



			QAction* saveAction() {	return _saveAction;	}
			QAction* exportAction();
			QAction* saveAction(ProjectNode* node);
			QAction* exportAction(ProjectNode* node);
		};
	}
}