#include "ActionHandler.h"

#include <src/hkx/Saver.h>
#include <src/widgets/FBXImport.h>
#include <src/widgets/SelectNode.h>
#include <src/utility/Conversion.h>

#include <src/items/HkxItemPointer.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

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
					action->setData(row_index);
					connect(action, SIGNAL(triggered()), this, SLOT(addOrSet()));
					out.push_back(action);
				}
				else {
					QAction* action = new QAction(tr("Set %1").arg(row_index.data().toString()), this);
					action->setData(row_index);
					connect(action, SIGNAL(triggered()), this, SLOT(addOrSet()));
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

void ActionHandler::buildAddEvent()
{
	_addEvent = new QAction(tr("&Create new Event"), this);
	_addEvent->setShortcuts(QKeySequence::FindPrevious);
	_addEvent->setStatusTip(tr("add Event"));
	connect(_addEvent, SIGNAL(triggered()), this, SLOT(addEvent()));
}

void ActionHandler::buildRemoveEvent()
{
	_removeEvent = new QAction(tr("&Remove Event"), this);
	_removeEvent->setShortcuts(QKeySequence::DeleteCompleteLine);
	_removeEvent->setStatusTip(tr("Remove Event"));
	connect(_removeEvent, SIGNAL(triggered()), this, SLOT(removeEvent()));
}

void ActionHandler::buildAddVariable()
{
	_addVariable = new QAction(tr("&Create new Variable"), this);
	_addVariable->setShortcuts(QKeySequence::FindPrevious);
	_addVariable->setStatusTip(tr("add Variable"));
	connect(_addVariable, SIGNAL(triggered()), this, SLOT(addVariable()));
}

void ActionHandler::buildRemoveVariable()
{
	_removeVariable = new QAction(tr("&Remove Variable"), this);
	_removeVariable->setShortcuts(QKeySequence::DeleteCompleteLine);
	_removeVariable->setStatusTip(tr("Remove Variable"));
	connect(_removeVariable, SIGNAL(triggered()), this, SLOT(removeVariable()));
}

void ActionHandler::buildAddProperty()
{
	_addProperty = new QAction(tr("&Create new Property"), this);
	_addProperty->setShortcuts(QKeySequence::FindPrevious);
	_addProperty->setStatusTip(tr("add Property"));
	connect(_addProperty, SIGNAL(triggered()), this, SLOT(addProperty()));
}

void ActionHandler::buildRemoveProperty()
{
	_removeProperty = new QAction(tr("&Remove Property"), this);
	_removeProperty->setShortcuts(QKeySequence::DeleteCompleteLine);
	_removeProperty->setStatusTip(tr("Remove Property"));
	connect(_removeProperty, SIGNAL(triggered()), this, SLOT(removeProperty()));
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
	action->setData(QVariant());
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
	action->setData(QVariant());
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
	action->setData(QVariant());
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
	action->setData(QVariant());
	_model.remove(index);
	action->setData(QVariant());
}

void ActionHandler::addEvent()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	action->setData(QVariant());
	bool ok;
	// Ask for birth date as a string.
	QString name = QInputDialog::getText(0, "Choose new Event's name",
		"Name:", QLineEdit::Normal,
		"", &ok);
	if (ok)
	{
		if (_model.isAssetNameValid(index, name, AssetType::events))
		{
			int events = _model.rowCount(index);
			bool insert = _model.insertRow(events, index);
			QModelIndex new_index = _model.index(events, 0, index);
			bool set_data = _model.setData(new_index, name);
		}
	}
}

void ActionHandler::removeEvent()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	action->setData(QVariant());
	_model.remove(index);
	action->setData(QVariant());
}

void ActionHandler::addVariable()
{

}

void ActionHandler::removeVariable()
{
}

void ActionHandler::addProperty()
{

}

void ActionHandler::removeProperty()
{
}

void ActionHandler::copy()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	action->setData(QVariant());
}

void ActionHandler::addOrSet()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	action->setData(QVariant());
	//row index to be set or container to be modified;
	bool ok;
	auto selection = SelectNode::getNode(_model, index, nullptr, &ok);
	if (ok)
	{
		void* to_set_or_add = selection.second == nullptr ? nullptr : selection.second->m_object;
		if (to_set_or_add == nullptr)
		{
			std::string name = selection.first.second.toUtf8().constData();
			to_set_or_add = _model.getResourceManager().createObject(_model.getFileIndex(index), selection.first.first, name);
		}
		HkxItemPointer p(to_set_or_add);
		QVariant value; value.setValue(p);
		bool isArray = _model.isArray(index);
		if (isArray)
		{
			int columns = _model.columnCount(index);
			bool result = _model.insertColumns(index.row(), columns, 1, index);
			QModelIndex new_index = _model.index(index.row(), columns, index);
			_model.setData(new_index, value);
		}
		else {
			_model.setData(index, value);
		}
	}
}