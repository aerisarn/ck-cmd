#include "ActionHandler.h"

#include <src/models/ProjectTreeModel.h>

using namespace ckcmd::HKX;

void ActionHandler::buildSaveAction()
{
	_saveAction = new QAction(tr("&Save"), this);
	_saveAction->setShortcuts(QKeySequence::Save);
	_saveAction->setStatusTip(tr("Save current selected project"));
	_saveAction->setEnabled(false);
}

void ActionHandler::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	if (selected.size() == 1) {
		auto model = (ProjectTreeModel*)selected[0].model();
		ProjectNode* node = model->getNode(selected[0].topLeft());
		if (node->canSaveOrExport())
		{
			_saveAction->setEnabled(true);
		}
		else {
			_saveAction->setEnabled(false);
		}
	}
}

void ActionHandler::save(ProjectNode* project_node)
{}
void ActionHandler::export_to(ProjectNode* project_node)
{}


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