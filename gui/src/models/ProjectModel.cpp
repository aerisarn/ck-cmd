#include "ProjectModel.h"

#include <src/models/SelectionProxyModel.h>
#include <src/models/NullableSelectionProxyModel.h>
#include <src/models/StringListModel.h>
#include <QBrush>

#include <src/items/HkxItemPointer.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbStateMachine_4.h>

using namespace ckcmd::HKX;

static size_t runtime_edge_index = 0;

ProjectModel::ProjectModel(CommandManager& commandManager, ResourceManager& resourceManager, QObject* parent) :
	_commandManager(commandManager),
	_resourceManager(resourceManager),
	QAbstractItemModel(parent)
{
	ModelEdge edge;

	edge._parent = QModelIndex();
	edge._parentType = NodeType::Invalid;
	edge._parentItem = nullptr;
	edge._row = -1;
	edge._column = -1;
	edge._child = QModelIndex();
	edge._childType = NodeType::Invalid;
	edge._childItem = nullptr;

	createModelEdgeIndex(edge);
}

qintptr ProjectModel::modelEdgeIndex(const ModelEdge& edge) const
{
	//auto result = std::find_if(
	//	_direct_find.begin(),
	//	_direct_find.end(),
	//	[&edge](const auto& mo) {return mo.second == edge; });

	//if (result != _direct_find.end())
	//	return result->first;
	//__debugbreak();
	//return -1;

	if (_reverse_find.find(const_cast<ModelEdge*>(&edge)) == _reverse_find.end())
		__debugbreak();
	return _reverse_find.at(const_cast<ModelEdge*>(&edge));
}

const ModelEdge& ProjectModel::modelEdge(const QModelIndex& index) const
{
	return _direct_find.at(index.internalId());
}

ModelEdge& ProjectModel::modelEdge(const QModelIndex& index)
{
	if (_direct_find.find(index.internalId()) == _direct_find.end())
		__debugbreak();
	return _direct_find[index.internalId()];
}

bool ProjectModel::hasModelEdgeIndex(const ModelEdge& edge) const
{
	//auto result = std::find_if(
	//	_direct_find.begin(),
	//	_direct_find.end(),
	//	[&edge](const auto& mo) {return mo.second == edge; });

	//return result != _direct_find.end();
	auto result = _reverse_find.find(const_cast<ModelEdge*>(&edge));
	return result != _reverse_find.end();
}

qintptr ProjectModel::createModelEdgeIndex(const ModelEdge& edge)
{
	qintptr result = runtime_edge_index++;
	_direct_find.insert({ result, edge });
	_reverse_find.insert({ &_direct_find[result], result });
	return result;
}

void  ProjectModel::deleteAllModelEdgeIndexesForFile(int project_file)
{
	auto rev_it = _reverse_find.begin();
	while (rev_it != _reverse_find.end())
	{
		if (rev_it->first->_project == project_file)
		{
			rev_it = _reverse_find.erase(rev_it);
		}
		else {
			rev_it++;
		}
	}
	auto dir_it = _direct_find.begin();
	while (dir_it != _direct_find.end())
	{
		if (dir_it->second._project == project_file)
		{
			_resourceManager.close(dir_it->second._file);
			dir_it = _direct_find.erase(dir_it);
		}
		else {
			dir_it++;
		}
	}
}

/*
** AbstractItemModel(required methods)
*/

QVariant ProjectModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	//if (role == Qt::BackgroundRole && index.isValid())
	//{
	//	ProjectNode* item = getNode(index);
	//	return QBrush(item->color());
	//}

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	return modelEdge(index).data(index.row(), index.column(), _resourceManager);
}



//row = field
//column = number of values in the field (1 scalar, >1 vector/matrix)
QModelIndex ProjectModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();


	if (!parent.isValid())
	{
		//root handling has two children, creatures and misc
		ModelEdge edge;

		edge._parent = QModelIndex();
		edge._row = row;
		edge._column = column;
		edge._childType = row == 0 ? NodeType::CharactersNode : NodeType::MiscsNode;

		if (!hasModelEdgeIndex(edge))
		{
			qintptr id = const_cast<ProjectModel*>(this)->createModelEdgeIndex(edge);
			auto& createdChildEdge = const_cast<ProjectModel*>(this)->_direct_find[id];
			createdChildEdge._child = createIndex(row, column, id);
			return createdChildEdge._child;
		}
		
		edge._child = createIndex(0, 0, modelEdgeIndex(edge));

		return edge._child;
	}
	
	const auto& parentEdge = modelEdge(parent);
	if (parentEdge.hasChild(row, column, _resourceManager))
	{
		auto childEdge = parentEdge.childEdge(row, column, _resourceManager);
		if (childEdge._childType != NodeType::Invalid)
		{
			if (!hasModelEdgeIndex(childEdge))
			{
				childEdge._parent = parent;
				qintptr id = const_cast<ProjectModel*>(this)->createModelEdgeIndex(childEdge);
				auto& createdChildEdge = const_cast<ProjectModel*>(this)->_direct_find[id];
				createdChildEdge._child = createIndex(0, 0, id);
				return createdChildEdge._child;
			}
			childEdge._child = createIndex(0, 0, modelEdgeIndex(childEdge));
			return childEdge._child;
		}
	}
	return createIndex(row, column, parent.internalId());
}

QModelIndex ProjectModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	const auto& edge = modelEdge(index);
	return edge._parent;
}

int ProjectModel::rowCount(const QModelIndex& index) const
{
	if (!index.isValid())
		return 2;

	const auto& edge = modelEdge(index);
	return edge.rows(_resourceManager);
}

int ProjectModel::columnCount(const QModelIndex& index) const
{
	if (!index.isValid())
		return 1;

	const auto& edge = modelEdge(index);
	return edge.columns(index.row(), _resourceManager);
}

bool ProjectModel::hasIndex(int row, int column, const QModelIndex& parent) const
{
	if (!parent.isValid())
		return row < 2 && column < 1;

	const auto& edge = modelEdge(parent);
	int rows = edge.rows(_resourceManager);
	int columns = edge.columns(row, _resourceManager);
	bool valid = row < rows&& (column < columns || column == 0);
	return valid;
}

bool ProjectModel::hasChildren(const QModelIndex& index) const
{
	if (!index.isValid())
		return true;
	return childCount(index) > 0;
}

int ProjectModel::childCount(const QModelIndex& index) const
{
	if (!index.isValid())
		return 2;

	const auto& edge = modelEdge(index);
	return edge.childCount(_resourceManager);
}

QModelIndex ProjectModel::child(int child_index, const QModelIndex& parent) const
{
	if (!parent.isValid())
	{
		if (child_index == 0)
			return index(0, 0, QModelIndex());
		else
			return index(1, 0, QModelIndex());
	}

	const auto& edge = modelEdge(parent);
	auto result = edge.child(child_index, _resourceManager);
	if (result == std::pair<int, int>{-1, -1})
	{
		//it happens without reason on blendergenerators. Comes from Qt.
		return QModelIndex();
	}
	return this->index(result.first, result.second, parent);
}

int ProjectModel::childIndex(const QModelIndex& child) const
{
	if (!child.isValid())
	{
		return MODELEDGE_INVALID;
	}

	const auto& edge = modelEdge(child);
	if (edge._parent == QModelIndex())
	{
		return child.internalId() - 1;
	}
	const auto& parent_edge = modelEdge(edge._parent);
	return parent_edge.childIndex(edge.row(), edge.column(), _resourceManager);
}

QVariant ProjectModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{
	return QVariant();
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return QAbstractItemModel::flags(index);
}

bool ProjectModel::setData(const QModelIndex& actual_index, const QVariant& value,
	int role)
{

	//if (NULL != node)
	//{
	//	if (role == Qt::BackgroundRole && index.isValid())
	//	{
	//		node->setColor(value.value<QColor>());
	//		return true;
	//	}

	if (role == Qt::EditRole)
	{
		QModelIndex index = actual_index;
		auto& edge = modelEdge(index);
		auto& parent_edge = modelEdge(edge._parent);
		//check if data is a link
		int child_index = parent_edge.childIndex(edge.row(), edge.column(), _resourceManager);
		if (this->index(edge.row(), edge.column(), edge._parent) == actual_index && 
			value.canConvert<HkxItemPointer>())
		{
			int row = edge.row();
			int column = edge.column();
			auto parent_index = edge._parent;
			//it is, remove link first
			remove(index);
			index = this->index(row, column, parent_index);
			edge = modelEdge(index);
		}
		bool result = edge.setData(index.row(), index.column(), value, _resourceManager);
		if (result)
		{
			emit dataChanged(index, index);
			auto new_index = this->index(index.row(), index.column(), edge._child);
			auto new_child_index = edge.childIndex(index.row(), index.column(), _resourceManager);
			if (MODELEDGE_INVALID != new_child_index && value.canConvert<HkxItemPointer>())
			{
				auto new_ptr = value.value<HkxItemPointer>();
				if (new_ptr.get() != nullptr)
				{
					emit beginInsertChildren(edge._child, new_child_index, new_child_index);
					emit endInsertChildren();
				}
			}
		}
		//else if (actual_index.internalId() != new_index.internalId() && edge.column() != 0)
		//{
		//	emit beginRemoveChildren(index, new_child_index, new_child_index);
		//	emit endRemoveChildren();
		//}
		return result;
	}
	return false;
}

std::pair<int, int> ProjectModel::dataStart(const QModelIndex& index)
{
	auto& edge = modelEdge(index);
	return edge.dataStart();
}

//void ProjectModel::importAsset(const QModelIndex& parent, const fs::path& sourcePath, AssetType type)
//{
//	auto& edge = modelEdge(parent);
//	if (isAssetsNode(parent) || nodeType(parent) == NodeType::CharacterHkxNode)
//	{
//		int row_start = edge.childCount(_resourceManager) - 1;
//		auto result = _resourceManager.importAssets(edge._project, sourcePath, type);
//		if (!result.empty())
//		{
//			emit beginInsertRows(parent, row_start, row_start + result.size());
//			for (int r = 0; r < result.size(); ++r)
//			{
//				edge.setData(r + row_start, 0, QString(result[r].string().c_str()), _resourceManager);
//			}
//			emit endInsertRows();
//		}
//	}
//}

void ProjectModel::refreshAssetList(const QModelIndex& parent, AssetType type)
{
	auto& edge = modelEdge(parent);
	if (isAssetsNode(parent) )
	{
		int current_rows = edge.childCount(_resourceManager);
		//full invalidate
		int current_assets = _resourceManager.assetsCount(edge._project, type);
		emit beginRemoveRows(parent, 0, current_rows);
		_resourceManager.clearAssetList(edge._project, type);
		emit endRemoveRows();
		emit beginInsertRows(parent, 0, current_assets);
		_resourceManager.refreshAssetList(edge._project, type);
		emit endInsertRows();
	}
	else if (nodeType(parent) == NodeType::CharacterHkxNode){

	}
}


void ProjectModel::select(const QModelIndex& index)
{

}

void ProjectModel::activate(const QModelIndex& index)
{
	auto& edge = modelEdge(index);
	if (edge._childType == NodeType::CharacterNode || edge._childType == NodeType::MiscNode)
	{
		ProjectType project_type = edge._childType == NodeType::CharacterNode ? ProjectType::character : ProjectType::misc;
		if (_resourceManager.isProjectFileOpen(edge.row(), project_type))
		{
			emit beginRemoveRows(index, 0, 0);
			int project_index = _resourceManager.projectFileIndex(edge.row(), project_type);
			edge._project = -1;
			_resourceManager.closeProjectFile(edge.row(), project_type);
			deleteAllModelEdgeIndexesForFile(project_index);
			emit endRemoveRows();
		}
		else {
			emit beginInsertRows(index, 0, 0);
			_resourceManager.openProjectFile(edge.row(), project_type);
			edge._project = _resourceManager.projectFileIndex(edge.row(), project_type);
			emit endInsertRows();
		}
	}
//else if (node_clicked->isSkeleton()) {
//	
//	_simulation->addSkeleton(
//		node_clicked->data(1).toString().toUtf8().constData()
//	);
//}
}

NodeType ProjectModel::nodeType(const QModelIndex& index) const
{
	const auto& type = modelEdge(index).type();
	if (type == NodeType::animationNames)
		int debug = 1;
	return type;
}

hkVariant* ProjectModel::variant(const QModelIndex& index)
{
	return reinterpret_cast<hkVariant*>(modelEdge(index)._childItem);
}

bool ProjectModel::isVariant(const QModelIndex& index)
{
	return ::isVariant(nodeType(index));
}

bool ProjectModel::isAssetsNode(const QModelIndex& index)
{
	return ::isAssetsNode(nodeType(index));
}

int ProjectModel::getProjectIndex(const QModelIndex& index)
{
	return modelEdge(index)._project;
}

int ProjectModel::getRigIndex(const QModelIndex& index)
{
	if (index.isValid())
	{
		auto& edge = modelEdge(index);
		ModelEdge search_edge;
		search_edge._project = edge._project;
		search_edge._file = edge._file;
		search_edge._childType = NodeType::SkeletonHkxNode;
		QModelIndex search_index = createIndex(0, 0, modelEdgeIndex(search_edge));
		if (search_index.isValid())
		{
			auto& child_edge = modelEdge(search_index);
			return child_edge._file;
		}		
	}
	return MODELEDGE_INVALID;
}

int ProjectModel::getFileIndex(const QModelIndex& index)
{
	return modelEdge(index)._file;
}

QModelIndex ProjectModel::getChildAssetProxy(const QModelIndex& index, NodeType proxy_type)
{
	auto& edge = modelEdge(index);
	int children = edge.childCount(_resourceManager);
	for (int i = 0; i < children; ++i)
	{		
		auto child_index = this->child(i, index);;
		auto& child_edge = modelEdge(child_index);
		if (modelEdge(child_index).type() == proxy_type)
		{
			QModelIndex asset_index;
			if (!hasIndex(0, 0, child_index))
				asset_index = createIndex(0, 0, child_index.internalId());
			else
				asset_index = child_index;
			return asset_index;
		}
	}
	return QModelIndex();
}

bool ProjectModel::isAssetNameValid(const QModelIndex& index, const QString& name, AssetType type)
{
	auto& edge = modelEdge(index);
	ModelEdge search_edge;
	search_edge._project = edge._project;
	QModelIndex search_index;
	switch (type) {
	case AssetType::events:
		search_edge._file = edge._file;
		search_edge._childType = NodeType::behaviorEventNames;
		search_index = createIndex(0, 0, modelEdgeIndex(search_edge));
		break;
	case AssetType::variables:
		search_edge._file = edge._file;
		search_edge._childType = NodeType::behaviorVariableNames;
		search_index = createIndex(1, 0, modelEdgeIndex(search_edge));
		break;
	default:
		return false;
	}
	if (search_index.isValid())
	{
		int rows = rowCount(search_index);
		for (int i = 0; i < rows; i++)
		{
			auto eventIndex = this->index(i, 0, search_index);
			if (eventIndex.data() == name)
				return false;
		}
		return true;
	}
	return false;
}

QModelIndex ProjectModel::variablesIndex(const QModelIndex& index)
{
	auto& edge = modelEdge(index);
	ModelEdge search_edge;
	search_edge._project = edge._project;
	QModelIndex search_index;
	search_edge._file = edge._file;
	search_edge._childType = NodeType::behaviorVariableNames;
	search_index = createIndex(1, 0, modelEdgeIndex(search_edge));
	return search_index;
}

QAbstractItemModel* ProjectModel::editModel(const QModelIndex& index, AssetType type, const QString& nullValue, int role)
{
	auto& edge = modelEdge(index);
	switch (edge.type())
	{
	case NodeType::CharacterHkxNode:
	{
		if (AssetType::bones == type)
		{
			auto skeleton_index = getChildAssetProxy(index, NodeType::SkeletonHkxNode);
			if (skeleton_index.isValid())
			{
				auto skeleton_bones_index = getChildAssetProxy(skeleton_index, NodeType::SkeletonBones);
				if (skeleton_bones_index.isValid())
					return new SelectionProxyModel(this, skeleton_bones_index);
			}
			return nullptr;
		}
		else if (AssetType::ragdoll_bones == type)
		{
			auto ragdoll_index = getChildAssetProxy(index, NodeType::RagdollHkxNode);
			if (ragdoll_index.isValid())
			{
				auto ragdoll_bones_index = getChildAssetProxy(ragdoll_index, NodeType::RagdollBones);
				if (ragdoll_bones_index.isValid())
					return new SelectionProxyModel(this, ragdoll_bones_index);
			}
			return nullptr;
		}
		else if (AssetType::events == type)
		{
			//auto top_behavior_index = getChildAssetProxy(index, NodeType::BehaviorHkxNode);
			//if (top_behavior_index.isValid())
			//{
			//	if (AssetType::events == type)
			//	{
			//		auto events_node = getChildAssetProxy(top_behavior_index, NodeType::behaviorEventNames);
			//		if (events_node.isValid())
			//			return new SelectionProxyModel(this, events_node);
			//	}
			//}
			//return nullptr;
			return new StringListModel(_resourceManager.eventList(edge._project, false));
		}
		else if (AssetType::variable_words == type) {
			return new StringListModel(_resourceManager.wordVariableList(edge._project));
		}
		else if (AssetType::attack_events == type) {
			return new StringListModel(_resourceManager.eventList(edge._project, true));
		}
		else if (AssetType::clips == type) {
			return new StringListModel(_resourceManager.clipList(edge._project));
		}
		else if (AssetType::clip_animations == type) {
			return new StringListModel(_resourceManager.clipAnimationsList(edge._project));
		}
		return new StringListModel(_resourceManager.assetsList(edge._project, type));
	}
	default:
	{
		ModelEdge search_edge;
		search_edge._project = edge._project;
		QModelIndex search_index;
		switch (type) {
		case AssetType::events:
			search_edge._file = edge._file;
			search_edge._childType = NodeType::behaviorEventNames;
			search_index = createIndex(0, 0, modelEdgeIndex(search_edge));
			break;
		case AssetType::variables:
			search_edge._file = edge._file;
			search_edge._childType = NodeType::behaviorVariableNames;
			search_index = createIndex(1, 0, modelEdgeIndex(search_edge));
			break;
		case AssetType::bones:
			search_edge._childType = NodeType::SkeletonHkxNode;
			search_index = createIndex(2, 0, modelEdgeIndex(search_edge));
			if (search_index.isValid())
			{
				search_index = getChildAssetProxy(search_index, NodeType::SkeletonBones);
			}
			break;
		case AssetType::ragdoll_bones:
			search_edge._childType = NodeType::RagdollHkxNode;
			search_index = createIndex(3, 0, modelEdgeIndex(search_edge));
			if (search_index.isValid())
			{
				search_index = getChildAssetProxy(search_index, NodeType::RagdollBones);
			}
			break;
		case AssetType::FSM_states:
		{
			QStringList options;
			if (nullptr != edge.childItem<hkVariant>())
			{
				hkVariant* v = edge.childItem<hkVariant>();
				if (v->m_class == &hkbStateMachineClass)
				{
					options = _resourceManager.getStates((hkbStateMachine*)v->m_object);
				}
				else if (v->m_class == &hkbStateMachineStateInfoClass)
				{
					if (edge.parentItem<hkVariant>() != nullptr)
						options = _resourceManager.getStates((hkbStateMachine*)edge.parentItem<hkVariant>()->m_object);
				}
			}
			if (!nullValue.isEmpty())
				options.insert(0, nullValue);
			return new QStringListModel(options);
		}
		default:
			return nullptr;
		}		
		if (search_index.isValid())
		{
			if (role == Qt::DisplayRole)
				return new SelectionProxyModel(this, search_index);
			else if (role == Qt::EditRole)
				return new NullableSelectionProxyModel(this,search_index, nullValue);
		}
		break;
	}
	}
	return nullptr;
}

bool ProjectModel::insertRows(int row, int count, const QModelIndex& index)
{
	auto& edge = modelEdge(index);
	emit beginInsertRows(index, row, row + count);
	emit beginInsertChildren(index, row, row + count);
	bool result = edge.insertRows(row, count, _resourceManager);
	emit endInsertChildren();
	emit endInsertRows();
	return result;
}

bool ProjectModel::removeRows(int row, int count, const QModelIndex& index)
{
	auto& edge = modelEdge(index);
	int rows = rowCount(index);
	emit beginRemoveRows(index, row , row + count - 1);
	emit beginRemoveChildren(index, row, row + count - 1);
	bool result = edge.removeRows(row, count, _resourceManager);
	emit endRemoveChildren();
	emit endRemoveRows();
	return result;
}

bool ProjectModel::remove(const QModelIndex& index)
{
	auto& edge = modelEdge(index);
	if (edge._parentType == NodeType::Invalid)
		__debugbreak();
	auto& parent_edge = modelEdge(edge._parent);
	int row = edge.row();
	bool result = false;
	int child_index = parent_edge.childIndex(edge.row(), edge.column(), _resourceManager);
	if (parent_edge.isArray(row, _resourceManager))
	{
		if (edge.column() == 0)
		{
			emit beginRemoveRows(edge._parent, row, row);
			emit beginRemoveChildren(edge._parent, child_index, child_index);
			result = parent_edge.removeRows(row, 1, _resourceManager);
			if (edge._childType == NodeType::behaviorEventName ||
				edge._childType == NodeType::behaviorVariable)
			{
				AssetType type = edge._childType == NodeType::behaviorEventName ?
					AssetType::events :
					AssetType::variables;
				result = result && _resourceManager.removeAsset(edge._file, type, row);
			}
			emit endRemoveChildren();
			emit endRemoveRows();
		}
		else {
			auto rowIndex = this->index(row, 0, edge._parent);
			emit beginRemoveColumns(rowIndex, edge.column(), edge.column());
			emit beginRemoveChildren(edge._parent, child_index, child_index);
			result = parent_edge.removeColumns(row, edge.column(), 1, _resourceManager);
			emit endRemoveChildren();
			emit endRemoveRows();
		}
	}
	else {
		emit beginRemoveChildren(edge._parent, child_index, child_index);
		HkxItemPointer ptr(nullptr); QVariant value; value.setValue(ptr);
		result = parent_edge.setData(edge.row(), edge.column(), value, _resourceManager);
		emit endRemoveChildren();
	}
	auto id = modelEdgeIndex(edge);
	_reverse_find.erase(&edge);
	_direct_find.erase(id);
	return result;
}


bool ProjectModel::insertColumns(int row, int column, int count, const QModelIndex& index)
{
	auto& edge = modelEdge(index);
	QModelIndex columnIndex = this->index(row, 0, index);
	int columns = columnCount(columnIndex);
	emit beginInsertColumns(columnIndex, column, column + count - 1);
	bool result = edge.insertColumns(row, column, count, _resourceManager);
	emit endInsertColumns();
	return result;
}

bool ProjectModel::removeColumns(int row, int column, int count, const QModelIndex& index)
{
	auto& edge = modelEdge(index);
	QModelIndex columnIndex = this->index(row, 0, index);
	int columns = columnCount(columnIndex);
	emit beginRemoveColumns(columnIndex, column, column + count - 1);
	bool result = edge.removeColumns(row, column, count, _resourceManager);
	emit endRemoveColumns();
	return result;
}

//bool ProjectModel::insertColumns(int column, int count, const QModelIndex& index)
//{
//	auto& edge = modelEdge(index);
//	auto& parent_edge = modelEdge(edge._parent);
//	emit beginInsertColumns(edge._parent, column, column + count - 1);
//	bool result = edge.insertColumns(edge.row(), column, count, _resourceManager);
//	emit endInsertColumns();
//	return result;
//}
//
//bool ProjectModel::removeColumns(int column, int count, const QModelIndex& index)
//{
//	auto& edge = modelEdge(index);
//	emit beginRemoveColumns(edge._parent, column, column + count - 1);
//	bool result = edge.removeColumns(index.row(), column, count, _resourceManager);
//	emit endRemoveColumns();
//	return result;
//}

void ProjectModel::SetCopyPointer(const QModelIndex& index)
{
	if (index.isValid())
		_copyPointer = index;
	else 
		_copyPointer = QModelIndex();
}

bool ProjectModel::PasteEnabled(const QModelIndex& index)
{
	if (index.isValid() && _copyPointer.isValid())
	{
		auto& to_paste = modelEdge(index);
		auto& paste_into = modelEdge(_copyPointer);
		return paste_into.canAddRow(to_paste, _resourceManager);
	}
	return false;
}

TypeInfo ProjectModel::rowType(const QModelIndex& index)
{
	if (index.isValid())
	{
		auto& edge = modelEdge(index);
		return edge.rowClass(index.row(), _resourceManager);
	}
	return {hkClassMember::Type::TYPE_VOID, nullptr };
}

std::vector<TypeInfo>  ProjectModel::rowTypes(const QModelIndex& index)
{
	std::vector<TypeInfo> out;
	if (index.isValid())
	{
		auto rows = rowCount(index);
		out.resize(rows);
		auto& edge = modelEdge(index);
		for (int i = 0; i < rows; ++i)
		{
			out[i] = edge.rowClass(i, _resourceManager);
		}
	}
	return out;
}

bool ProjectModel::isArray(const QModelIndex& index)
{
	if (index.isValid())
	{
		auto& edge = modelEdge(index);
		return edge.isArray(index.row(), _resourceManager);
	}
	return false;
} 

QStringList ProjectModel::rowNames(const QModelIndex& index, const QString& prefix_filter)
{
	QStringList out;
	if (index.isValid())
	{
		auto rows = rowCount(index);
		auto& edge = modelEdge(index);
		for (int i = 1; i < rows; ++i)
		{
			auto row_name = this->index(i, 0, index).data().toString();
			if (!row_name.startsWith(prefix_filter))
				out << row_name;
		}
	}
	return out;
}

QModelIndex ProjectModel::next(const QModelIndex& start) const
{
	QModelIndex current = start;
	bool end = false;
	int next_row = start.row();
	int next_column = start.column();
	auto& edge = modelEdge(current);
	if (edge.hasChild(next_row, next_column, _resourceManager))
	{
		auto child_index = index(next_row, next_column, current);
		if (child_index.isValid())
			return child_index;
	}
	next_column += 1;
	while (!end)
	{
		auto& edge = modelEdge(current);
		int columns = edge.columns(next_row, _resourceManager);
		if (next_column < columns)
		{
			auto child_index = index(next_row, next_column, current);
			if (child_index.isValid())
				return child_index;
		}
		int rows = edge.rows(_resourceManager);
		if (next_row + 1 < rows)
			return index(next_row + 1, 0, current);
		current = edge._parent;
		next_row = edge.row();
		next_column = edge.column() + 1;
		if (edge.childType() == NodeType::Invalid)
			end = true;
	}
	return QModelIndex();
}

QModelIndexList ProjectModel::match(const QModelIndex& start, int role,
	const QVariant& value, int hits,
	Qt::MatchFlags flags) const
{
	std::set<QModelIndex> visited;
	std::vector<QString> visited_data;
	QModelIndexList result;
	uint matchType = flags & 0x0F;
	Qt::CaseSensitivity cs = flags & Qt::MatchCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
	bool recurse = flags & Qt::MatchRecursive;
	bool wrap = flags & Qt::MatchWrap;
	bool allHits = (hits == -1);
	QString text; // only convert to a string if it is needed
	QModelIndex idx = start;
	while (idx.isValid() && (allHits || result.count() < hits))
	{
		QVariant v = data(idx, role);
		visited_data.push_back(v.toString());
		for (int i = 0; (wrap && i < 2) || (!wrap && i < 1); ++i) {


			// QVariant based matching
			if (matchType == Qt::MatchExactly) {
				if (value == v)
					result.append(idx);
			}
			else { // QString based matching
				if (text.isEmpty()) // lazy conversion
					text = value.toString();
				QString t = v.toString();
				switch (matchType) {
				case Qt::MatchRegExp:
					if (QRegExp(text, cs).exactMatch(t))
						result.append(idx);
					break;
				case Qt::MatchWildcard:
					if (QRegExp(text, cs, QRegExp::Wildcard).exactMatch(t))
						result.append(idx);
					break;
				case Qt::MatchStartsWith:
					if (t.startsWith(text, cs))
						result.append(idx);
					break;
				case Qt::MatchEndsWith:
					if (t.endsWith(text, cs))
						result.append(idx);
					break;
				case Qt::MatchFixedString:
					if (t.compare(text, cs) == 0)
						result.append(idx);
					break;
				case Qt::MatchContains:
				default:
					if (t.contains(text, cs))
						result.append(idx);
				}
			}
		}

		visited.insert(idx);

		QModelIndex next_idx = next(idx);

		if (visited.find(next_idx) != visited.end())
			return { next_idx };

		if (next_idx == idx)
			return { next_idx };
		if (!recurse)
		{
			while (next_idx.internalPointer() > idx.internalPointer())
				next_idx = next(idx);
			if (!next_idx.isValid() || next_idx.internalPointer() < idx.internalPointer())
				break;
		}
		idx = next_idx;
	}
	return result;
}

void* ProjectModel::createObject(const QModelIndex& index, const hkClass* hkclass, const std::string& name)
{
	auto& edge = modelEdge(index);
	int file_index = edge._file;
	void* out = _resourceManager.createObject(file_index, hkclass, name);
	//Special case
	if (hkclass == &hkbStateMachineStateInfoClass && edge._childItem!= NULL)
	{
		hkbStateMachineStateInfo* state = (hkbStateMachineStateInfo*)out;
		auto fsm_variant = edge.childItem<hkVariant>();
		auto fsm = (hkbStateMachine*)fsm_variant->m_object;
		int highest_stateid = -1;
		if (state != NULL && fsm != NULL)
		{
			for (int i = 0; i < fsm->m_states.getSize(); i++)
			{
				if (fsm->m_states[i]->m_stateId > highest_stateid)
					highest_stateid = fsm->m_states[i]->m_stateId;
			}
			if (highest_stateid >= 0)
			{
				state->m_stateId = highest_stateid + 1;
			}
		}
	}
	return out;
}

std::vector<std::tuple<QString, TypeInfo, bool, size_t>> ProjectModel::bindables(const QModelIndex& index)
{
	auto& edge = modelEdge(index);
	return edge.bindables(_resourceManager);
}
