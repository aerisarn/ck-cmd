#include "ActionHandler.h"

#include <src/hkx/Saver.h>
#include <src/widgets/FBXImport.h>
#include <src/widgets/SelectNode.h>
#include <src/widgets/VariableCreator.h>
#include <src/utility/Conversion.h>
#include <src/widgets/TransitionCreator.h>

#include <src/items/HkxItemPointer.h>
#include <src/items/HkxItemReal.h>

#include <src/widgets/ProjectsWidget.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

#include <hkbVariableInfo_1.h>
#include <hkbVariableBindingSet_2.h>
#include <hkbStateMachineTransitionInfoArray_0.h>

using namespace ckcmd::HKX;

void ActionHandler::buildSaveAction()
{
	_save = new QAction(tr("&Save"), this);
	_save->setShortcuts(QKeySequence::Save);
	_save->setStatusTip(tr("Save current selected project"));
	connect(_save, SIGNAL(triggered()), this, SLOT(save()));
}

void ActionHandler::buildExportLEAction()
{
	_exportLE = new QAction(tr("&Export to LE"), this);
	_exportLE->setShortcuts(QKeySequence::Save);
	_exportLE->setStatusTip(tr("Export this project files to Skyrim LE"));
	connect(_exportLE, SIGNAL(triggered()), this, SLOT(exportLE()));
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

QAction* ActionHandler::exportLEAction(const QVariant& action_data)
{
	QModelIndex index = action_data.value<QModelIndex>();
	if (index.isValid())
	{
		int project_index = _model.getProjectIndex(index);
		bool enable = project_index != -1 && _model.getResourceManager().is_open(project_index);
		_exportLE->setEnabled(enable);
	}
	else {
		_exportLE->setEnabled(false);
	}
	return _exportLE;
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
			if (types[i]._class != nullptr)
			{
				if (types[i]._class == &hkbVariableBindingSetClass)
					continue;
				if (types[i]._class == &hkbStateMachineTransitionInfoArrayClass)
					continue;
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

void ActionHandler::buildRemoveNode()
{
	_removeNode = new QAction(tr("&Remove"), this);
	_removeNode->setShortcuts(QKeySequence::DeleteCompleteLine);
	_removeNode->setStatusTip(tr("Remove"));
	connect(_removeNode, SIGNAL(triggered()), this, SLOT(remove()));
}

void ActionHandler::buildAddTransition()
{
	_addTransition = new QAction(tr("&Add Transition"), this);
	_addTransition->setShortcuts(QKeySequence::DeleteCompleteLine);
	_addTransition->setStatusTip(tr("Add Transition"));
	connect(_addTransition, SIGNAL(triggered()), this, SLOT(addTransition()));
}

void ActionHandler::buildRemoveTransition()
{
	_removeTransition = new QAction(tr("&Remove Transition"), this);
	_removeTransition->setShortcuts(QKeySequence::DeleteCompleteLine);
	_removeTransition->setStatusTip(tr("Remove Transition"));
	connect(_removeTransition, SIGNAL(triggered()), this, SLOT(removeTransition()));
}

void ActionHandler::buildCreateProjectAction()
{
	_createProject = new QAction(tr("&New Project"), this);
	_createProject->setShortcuts(QKeySequence::New);
	_createProject->setStatusTip(tr("Create new project"));
	_createProject->setEnabled(false);
	connect(_createProject, SIGNAL(triggered()), this, SLOT(createProject()));
}

void ActionHandler::buildCreateBehaviorAction()
{
	_createBehavior = new QAction(tr("&New Behavior"), this);
	_createBehavior->setShortcuts(QKeySequence::New);
	_createBehavior->setStatusTip(tr("Create new Behavior File"));
	_createBehavior->setEnabled(false);
	connect(_createBehavior, SIGNAL(triggered()), this, SLOT(createBehavior()));
}

void ActionHandler::buildCopyAction()
{
	_copy = new QAction(tr("&Copy"), this);
	_copy->setShortcuts(QKeySequence::Copy);
	_copy->setStatusTip(tr("Copy node"));
	_copy->setEnabled(true);
	connect(_copy, SIGNAL(triggered()), this, SLOT(copy()));
}

void ActionHandler::buildPasteAction()
{
	_paste = new QAction(tr("&Paste"), this);
	_paste->setShortcuts(QKeySequence::Paste);
	_paste->setStatusTip(tr("Paste copy"));
	_paste->setEnabled(true);
	connect(_paste, SIGNAL(triggered()), this, SLOT(paste()));
}

void ActionHandler::buildFindAction()
{
	_find = new QAction(tr("&Find"), this);
	_find->setShortcuts(QKeySequence::Find);
	_find->setStatusTip(tr("Find Node"));
	_find->setEnabled(true);
	connect(_find, SIGNAL(triggered()), this, SLOT(find()));
}

void ActionHandler::buildFindNextAction()
{
	_findNext = new QAction(tr("&Find Next"), this);
	_findNext->setShortcuts(QKeySequence::FindNext);
	_findNext->setStatusTip(tr("Find Next Node"));
	_findNext->setEnabled(true);
	connect(_findNext, SIGNAL(triggered()), this, SLOT(findNext()));
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

void ActionHandler::createBehavior()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	action->setData(QVariant());
	int project_index = _model.getProjectIndex(index);
	bool ok;
	QString name = QInputDialog::getText(0, "Choose new Behavior's filename",
		"Filename:", QLineEdit::Normal,
		"", &ok);
	if (ok)
	{
		_model.getResourceManager().CreateNewBehaviorFile(project_index, name);
	}
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
	auto type = _model.nodeType(index);
	fs::path assets_path;
	if (NodeType::animationNames == type)
		assets_path = _model.getResourceManager().assetFolder(_model.getProjectIndex(index), AssetType::animation);
	else if (NodeType::SkeletonHkxNode == type)
		assets_path = _model.getResourceManager().assetFolder(_model.getProjectIndex(index), AssetType::character_assets);
	if (export_fbx_output_dir.empty())
		export_fbx_output_dir = assets_path;
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
			if (NodeType::animationName == type)
			{
				std::string project_animation = index.data().toString().toUtf8().constData();
				assets_path = _model.getResourceManager().assetFolder(_model.getProjectIndex(index), AssetType::project) / project_animation;

				auto movements = _model.getResourceManager().getAnimationMovementData(_model.getProjectIndex(index), project_animation);

				bool result = Conversion::convertHkxAnimationToFBX
				(
					rig_path,
					assets_path,
					dir.toUtf8().constData(),
					movements
				);
			}
			else if (NodeType::SkeletonHkxNode == type) {
				bool result = Conversion::convertHkxSkeletonToFbx
				(
					rig_path,
					dir.toUtf8().constData()
				);
			}
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

hkbVariableInfo::VariableType havokVariableType(VariableCreator::VariableType type)
{
	switch (type)
	{
	case VariableCreator::VariableType::type_invalid:
		break;
	case VariableCreator::VariableType::type_bool:
		return hkbVariableInfo::VARIABLE_TYPE_BOOL;
	case VariableCreator::VariableType::type_int8:
		return hkbVariableInfo::VARIABLE_TYPE_INT8;
	case VariableCreator::VariableType::type_int16:
		return hkbVariableInfo::VARIABLE_TYPE_INT16;
	case VariableCreator::VariableType::type_int32:
		return hkbVariableInfo::VARIABLE_TYPE_INT32;
	case VariableCreator::VariableType::type_real:
		return hkbVariableInfo::VARIABLE_TYPE_REAL;
	case VariableCreator::VariableType::type_pointer:
		return hkbVariableInfo::VARIABLE_TYPE_POINTER;
	case VariableCreator::VariableType::type_vector3:
		return hkbVariableInfo::VARIABLE_TYPE_VECTOR3;
	case VariableCreator::VariableType::type_vector4:
		return hkbVariableInfo::VARIABLE_TYPE_VECTOR4;
	case VariableCreator::VariableType::type_quaternion:
		return hkbVariableInfo::VARIABLE_TYPE_QUATERNION;
	default:
		break;
	}
	return hkbVariableInfo::VARIABLE_TYPE_INVALID;
}

void ActionHandler::addVariable()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	action->setData(QVariant());
	bool ok;
	auto new_variable_info = VariableCreator::getNewVariable(nullptr, &ok);
	if (ok && !new_variable_info.first.isEmpty(), new_variable_info.second != VariableCreator::VariableType::type_invalid)
	{
		if (_model.isAssetNameValid(index, new_variable_info.first, AssetType::variables))
		{
			int variables = _model.rowCount(index);
			bool insert = _model.insertRow(variables, index);
			QModelIndex new_index = _model.index(variables, 0, index);
			bool set_data = _model.setData(new_index, new_variable_info.first);
			QModelIndex role_index = _model.index(1, 1, new_index);
			bool set_role = _model.setData(role_index, 0);
			QModelIndex flags_index = _model.index(2, 1, new_index);
			bool set_flags = _model.setData(flags_index, 0);
			QModelIndex type_index = _model.index(3, 1, new_index);
			bool set_type = _model.setData(type_index, havokVariableType(new_variable_info.second));
			QModelIndex value_index = _model.index(4, 1, new_index);
			bool set_value;
			switch (havokVariableType(new_variable_info.second))
			{
			case hkbVariableInfo::VARIABLE_TYPE_BOOL:
			case hkbVariableInfo::VARIABLE_TYPE_INT8:
			case hkbVariableInfo::VARIABLE_TYPE_INT16:
			case hkbVariableInfo::VARIABLE_TYPE_INT32:
				set_value = _model.setData(value_index, 0);
				break;
			case hkbVariableInfo::VARIABLE_TYPE_REAL:
				set_value = _model.setData(value_index, 0.);
				break;
			case hkbVariableInfo::VARIABLE_TYPE_POINTER:
			{
				QVariant value;
				HkxItemPointer p(nullptr);
				value.setValue(p);
				set_value = _model.setData(value_index, value);
				break;
			}
			case hkbVariableInfo::VARIABLE_TYPE_VECTOR3:
			case hkbVariableInfo::VARIABLE_TYPE_VECTOR4:
			case hkbVariableInfo::VARIABLE_TYPE_QUATERNION:
			{
				QVariant value;
				HkxItemReal p({ {0., 0., 0., 0.} });
				value.setValue(p);
				set_value = _model.setData(value_index, value);
				break;
			}
			default:
				break;
			}
		}
	}
}

void ActionHandler::removeVariable()
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
	_model.SetCopyPointer(index);
}

void ActionHandler::paste()
{
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
			to_set_or_add = _model.createObject(index, selection.first.first, name);
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
			QModelIndex new_index = _model.index(index.row(), 1, index);
			_model.setData(new_index, value);
		}
	}
}

void ActionHandler::remove()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	_model.remove(index);
	action->setData(QVariant());
}

void ActionHandler::addTransition()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message

	bool ok = false;
	auto new_transition = TransitionCreator::getTransition(_model, index, nullptr, &ok);
	if (ok && new_transition.eventIndex != -1 && new_transition.toStateId != -1)
	{
		int rows = _model.rowCount(index);
		if (new_transition.fromStateId != -1)
		{
			_model.insertRow(new_transition.fromStateId, index);
			QVariant data; data.setValue(QVector<int>({ new_transition.fromStateId, new_transition.toStateId, new_transition.eventIndex }));
			_model.setData(index, data);
		}
		else {
			_model.insertRow(rows, index);
			auto new_index = _model.index(rows, 0, index);
			auto event_index = _model.index(11, 1, new_index);
			_model.setData(event_index, new_transition.eventIndex);
			auto toState_index = _model.index(12, 1, new_index);
			_model.setData(toState_index, new_transition.toStateId);
		}
	}

	action->setData(QVariant());
}

void ActionHandler::removeTransition()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message

	_model.remove(index);

	action->setData(QVariant());
}

QString _last_search = "";
QModelIndex _last_result = QModelIndex();

void ActionHandler::find()
{
	bool ok;
	QString search_string = QInputDialog::getText(0, "Enter search string",
		"String:", QLineEdit::Normal,
		"", &ok);
	if (ok && !search_string.isEmpty())
	{
		QModelIndex start = QModelIndex();
		if (nullptr != _view)
			start = _view->currentIndex();
		if (!start.isValid())
			start = _model.index(0, 0);

		_last_search = search_string;
		auto result = _model.match(start, Qt::DisplayRole, _last_search, 1, Qt::MatchContains | Qt::MatchRecursive);
		if (!result.isEmpty() && result.begin()->isValid())
		{
			_last_result = *result.begin();
			emit found(_last_result);
			return;
		}
	}
	_last_result = _model.index(0, 0);
}

void ActionHandler::findNext()
{
	if (!_last_search.isEmpty())
	{
		if (_last_result == QModelIndex())
			_last_result = _model.index(0, 0);
		else
			_last_result = _model.next(_last_result);
		auto result = _model.match(_last_result, Qt::DisplayRole, _last_search, 1, Qt::MatchContains | Qt::MatchRecursive);
		if (!result.isEmpty() && result.begin()->isValid())
		{
			_last_result = *result.begin();
			emit found(_last_result);
		}
	}
}

void ActionHandler::setView(ProjectsWidget* view)
{
	_view = view;
}

void ActionHandler::exportLE()
{
	QAction* action = static_cast<QAction*>(sender());
	if (action == nullptr)
		return; //todo error message
	QModelIndex index = action->data().value<QModelIndex>();
	if (!index.isValid())
		return; //todo error message
	action->setData(QVariant());
	int project_index = _model.getProjectIndex(index);
	_model.getResourceManager().exportProject(project_index);
}