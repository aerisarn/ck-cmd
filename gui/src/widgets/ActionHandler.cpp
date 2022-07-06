#include "ActionHandler.h"

#include <src/hkx/Saver.h>
#include <src/widgets/FBXImport.h>
#include <src/utility/Conversion.h>

#include <QMessageBox>
#include <QFileDialog>

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

std::vector<QAction*> ActionHandler::addActions(const QVariant& action_data)
{
	std::vector<QAction*> out;
	QModelIndex index = action_data.value<QModelIndex>();
	if (index.isValid())
	{
		auto types = _model.rowTypes(index);
		for (size_t i = 1; i < types.size(); ++i)
		{
			if (types[i] != nullptr)
			{
				auto row_index = _model.index(i, 0, index);
				if (_model.isArray(row_index))
				{
					QAction* action = new QAction(tr("Add to %1").arg(row_index.data().toString()), this);
					out.push_back(action);
				}
				else {
					QAction* action = new QAction(tr("Set %1").arg(row_index.data().toString()), this);
					out.push_back(action);
				}
			}
		}		
	}
	return out;
}

void ActionHandler::buildImportFBXAction()
{
	_importFBX = new QAction(tr("&Import FBX"), this);
	_importFBX->setShortcuts(QKeySequence::InsertLineSeparator);
	_importFBX->setStatusTip(tr("Import FBX Asset"));
	connect(_importFBX, SIGNAL(triggered()), this, SLOT(importFBX()));
}

void ActionHandler::buildExportFBXAction()
{
	_exportFBX = new QAction(tr("&Export FBX"), this);
	_exportFBX->setShortcuts(QKeySequence::Back);
	_exportFBX->setStatusTip(tr("Export FBX Asset"));
	connect(_exportFBX, SIGNAL(triggered()), this, SLOT(exportFBX()));
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

void ActionHandler::buildCopyAction()
{
	_copy = new QAction(tr("&Copy"), this);
	_copy->setShortcuts(QKeySequence::New);
	_copy->setStatusTip(tr("Copy node"));
	_copy->setEnabled(true);
	connect(_copy, SIGNAL(triggered()), this, SLOT(copy()));
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
		int rig_index = _model.getRigIndex(index);
		if (rig_index != MODELEDGE_INVALID)
		{
			fs::path rig_path = _model.getResourceManager().path(rig_index);
			auto result = Conversion::convertFbxAnimationToHkx(rig_path, selection.first, selection.second);
			if (!result.second.empty())
			{
				fs::path base = _model.getResourceManager().assetFolder(_model.getProjectIndex(index), AssetType::project);
				fs::path result_path = fs::relative(result.second, base);
				int animations = _model.rowCount(index);
				bool insert = _model.insertRow(animations, index);
				QModelIndex new_index = _model.index(animations, 0, index);
				bool set_data = _model.setData(new_index, result_path.string().c_str());
				_model.getResourceManager().setAnimationMovementData(_model.getProjectIndex(index), result_path.string(), result.first);
			}
			else {
				QMessageBox::critical(nullptr, "Import FBX Failed!", QString("Unable to import file %1").arg(selection.first.string().c_str()));
			}
		}
	}
}

fs::path export_fbx_output_dir;

void ActionHandler::exportFBX()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	bool ok;
	fs::path animation_path = _model.getResourceManager().assetFolder(_model.getProjectIndex(index), AssetType::animation);
	if (export_fbx_output_dir.empty())
		export_fbx_output_dir = animation_path;
	QString dir = QFileDialog::getExistingDirectory(nullptr, tr("Select Output directory"),
		export_fbx_output_dir.string().c_str(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (!dir.isEmpty())
	{
		export_fbx_output_dir = dir.toUtf8().constData();
		int rig_index = _model.getRigIndex(index);
		if (rig_index != MODELEDGE_INVALID)
		{
			fs::path rig_path = _model.getResourceManager().path(rig_index);
			std::string project_animation = index.data().toString().toUtf8().constData();
			animation_path = _model.getResourceManager().assetFolder(_model.getProjectIndex(index), AssetType::project) / project_animation;

			auto movements = _model.getResourceManager().getAnimationMovementData(_model.getProjectIndex(index), project_animation);

			bool result = Conversion::convertHkxAnimationToFBX
			(
				rig_path,
				animation_path,
				dir.toUtf8().constData(),
				movements
			);
		}
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

void ActionHandler::copy()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	_model.removeRow(index.row(), index.parent());
}