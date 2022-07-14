#pragma once

#include <src/hkx/ResourceManager.h>
#include <src/models/ProjectModel.h>

#include <QObject>
#include <QMenu>
#include <QTreeView>

class FBXImport;
class ProjectsWidget;

namespace ckcmd {
	namespace HKX {

		class ActionHandler : public QObject
		{
			Q_OBJECT

			ProjectModel& _model;
			ProjectsWidget* _view = nullptr;

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
			QAction* _removeNode;
			QAction* _addTransition;
			QAction* _removeTransition;
			QAction* _find;
			QAction* _findNext;

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
			void buildRemoveNode();
			void buildAddTransition();
			void buildRemoveTransition();
			void buildCreateProjectAction();
			void buildFindAction();
			void buildFindNextAction();


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
			void remove();
			void addTransition();
			void removeTransition();
			void find();
			void findNext();

		signals:
			void found(const QModelIndex& index);

		public:
			ActionHandler(ProjectModel& model, QObject* parent = nullptr) :
				_save(nullptr),
				_model(model),
				QObject(parent)
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
				buildRemoveNode();
				buildAddTransition();
				buildRemoveTransition();
				buildFindAction();
				buildFindNextAction();
			}

			void setView(ProjectsWidget* view);

			QAction* saveAction(const QVariant& action_data);
			std::vector<QAction*> addActions(const QVariant& action_data);
			QAction* exportFBXAction() { return _exportFBX; }
			QAction* importFBXAction() { return _importFBX; }
			QAction* removeAnimationAction() { return _removeAnimation; }
			QAction* addEventAction() { return _addEvent; }
			QAction* removeEventAction() { return _removeEvent; }
			QAction* addVariableAction() { return _addVariable; }
			QAction* removeVariableAction() { return _removeVariable; }
			QAction* addPropertyAction() { return _addProperty; }
			QAction* removePropertyAction() { return _removeProperty; }
			QAction* createProjectAction() { return _createProject; }
			QAction* addTransitionAction() { return _addTransition; }
			QAction* removeTransitionAction() { return _removeTransition; }
			QAction* removeAction() { return _removeNode; }
			QAction* copyAction() { return _copy; }
			QAction* findAction() { return _find; }
			QAction* findNextAction() { return _findNext; }
		};
	}
}