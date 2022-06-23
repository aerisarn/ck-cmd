#include "ActionHandler.h"

#include <src/hkx/Saver.h>
#include <src/widgets/FBXImport.h>
#include <src/utility/Conversion.h>

//#include <src/models/ProjectTreeModel.h>

using namespace ckcmd::HKX;

void ActionHandler::buildSaveAction()
{
	_save = new QAction(tr("&Save"), this);
	_save->setShortcuts(QKeySequence::Save);
	_save->setStatusTip(tr("Save current selected project"));
	connect(_save, SIGNAL(triggered()), this, SLOT(save()));
}

QAction* ActionHandler::saveAction(const QVariant& action_data)
{
	QModelIndex index = action_data.value<QModelIndex>();
	if (index.isValid())
	{
		int project_index = _model.getProjectIndex(index);
		bool enable = project_index != -1 && _model.getResourceManager().is_open(project_index);
		_save->setEnabled(enable);
	}
	else {
		_save->setEnabled(false);
	}
	return _save;
}

void ActionHandler::buildImportFBXAction()
{
	_importFBX = new QAction(tr("&Import FBX"), this);
	_importFBX->setShortcuts(QKeySequence::InsertLineSeparator);
	_importFBX->setStatusTip(tr("Import FBX Asset"));
	connect(_importFBX, SIGNAL(triggered()), this, SLOT(importFBX()));
}

void ActionHandler::buildRemoveAnimation()
{
	_removeAnimation = new QAction(tr("&Remove Animation"), this);
	_removeAnimation->setShortcuts(QKeySequence::DeleteCompleteLine);
	_removeAnimation->setStatusTip(tr("Remove Animation"));
	connect(_removeAnimation, SIGNAL(triggered()), this, SLOT(removeAnimation()));
}
void ActionHandler::buildCreateProjectAction()
{
	_createProject = new QAction(tr("&New"), this);
	_createProject->setShortcuts(QKeySequence::New);
	_createProject->setStatusTip(tr("Create new project"));
	_createProject->setEnabled(false);
	connect(_createProject, SIGNAL(triggered()), this, SLOT(createProject()));
}

void ActionHandler::save()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	int project_index = _model.getProjectIndex(index);
	_model.getResourceManager().saveProject(project_index);
}

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

void ActionHandler::importFBX()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	bool ok;
	auto selection = FBXImport::getPath(
		_model.getResourceManager().assetFolder(_model.getProjectIndex(index), AssetType::animation),
		nullptr,
		&ok);

	if (ok && !selection.first.empty() && !selection.second.empty())
	{
		fs::path out = Conversion::convertFbxAnimationToHkx(selection.first, selection.second);
		fs::path base = _model.getResourceManager().assetFolder(_model.getProjectIndex(index), AssetType::project);
		fs::path result_path = fs::relative(out, base);
		int animations = _model.rowCount(index);
		_model.insertRow(animations, index);
		QModelIndex new_index = _model.index(animations, 0, index);
		_model.setData(new_index, result_path.string().c_str());
	}
}

void ActionHandler::removeAnimation()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	_model.removeRow(index.row(), index.parent());
}