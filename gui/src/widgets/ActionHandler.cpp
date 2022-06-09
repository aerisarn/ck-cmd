#include "ActionHandler.h"

#include <src/hkx/Saver.h>

#include <src/models/ProjectTreeModel.h>

using namespace ckcmd::HKX;

void ActionHandler::buildSaveAction()
{
	_save = new QAction(tr("&Save"), this);
	_save->setShortcuts(QKeySequence::Save);
	_save->setStatusTip(tr("Save current selected project"));
	_save->setEnabled(false);
	connect(_save, SIGNAL(triggered()), this, SLOT(save()));
}

void ActionHandler::buildCreateProjectAction()
{
	_createProject = new QAction(tr("&New"), this);
	_createProject->setShortcuts(QKeySequence::New);
	_createProject->setStatusTip(tr("Create new project"));
	_createProject->setEnabled(false);
	connect(_createProject, SIGNAL(triggered()), this, SLOT(createProject()));
}


void ActionHandler::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	//if (selected.size() == 1) {
	//	auto model = (ProjectTreeModel*)selected[0].model();
	//	ProjectNode* node = model->getNode(selected[0].topLeft());
	//	if (node->canSaveOrExport())
	//	{
	//		_saveAction->setEnabled(true);
	//	}
	//	else {
	//		_saveAction->setEnabled(false);
	//	}
	//}
}


void ActionHandler::save()
{
	//if (_modelview.selectionModel()->selection().size() == 1) {
	//	auto selected = _modelview.selectionModel()->selection()[0];
	//	auto model = (ProjectTreeModel*)_modelview.selectionModel()->selection()[0].model();
	//	ProjectNode* node = model->getNode(selected.topLeft());
	//	Saver(_resourceManager, node, model);
	//}
}

void ActionHandler::export_to(ProjectNode* project_node)
{}

void ActionHandler::createProject()
{
	//if (_modelview.selectionModel()->selection().size() == 1) {
	//	auto selected = _modelview.selectionModel()->selection()[0];
	//	auto model = (ProjectTreeModel*)_modelview.selectionModel()->selection()[0].model();
	//	ProjectNode* node = model->getNode(selected.topLeft());
	//	Saver(_resourceManager, node, model);
	//}
}



QAction* ActionHandler::exportAction()
{
	return nullptr;
}

QAction* ActionHandler::saveAction(ProjectNode* node)
{
	return nullptr;
}

QAction* ActionHandler::exportAction(ProjectNode* node)
{
	return nullptr;
}