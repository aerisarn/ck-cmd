#include "ProjectModel.h"

#include <src/models/SelectionProxyModel.h>
#include <QStringListModel>
#include <QBrush>

using namespace ckcmd::HKX;

static size_t runtime_edge_index = 0;

ProjectModel::ProjectModel(CommandManager& commandManager, ResourceManager& resourceManager, QObject* parent) :
	_commandManager(commandManager),
	_resourceManager(resourceManager),
	_actionsManager(_commandManager, _resourceManager),
	QAbstractItemModel(parent)
{
}

qintptr ProjectModel::modelEdgeIndex(const ModelEdge& edge) const
{
	return _reverse_find.at(const_cast<ModelEdge*>(&edge));
}

const ModelEdge& ProjectModel::modelEdge(const QModelIndex& index) const
{
	return _direct_find.at(index.internalId());
}

ModelEdge& ProjectModel::modelEdge(const QModelIndex& index)
{
	if (_direct_find.find(index.internalId()) == _direct_find.end())
		int debug = 1;
	return _direct_find[index.internalId()];
}

bool ProjectModel::hasModelEdgeIndex(const ModelEdge& edge) const
{
	return _reverse_find.find(const_cast<ModelEdge*>(&edge)) != _reverse_find.end();
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
		//edge._childItem = row == 0 ? reinterpret_cast<void*>(&_charactersNode) : (void*)(&_miscsNode);

		if (!hasModelEdgeIndex(edge))
		{
			qintptr id = const_cast<ProjectModel*>(this)->createModelEdgeIndex(edge);
			auto& createdChildEdge = const_cast<ProjectModel*>(this)->_direct_find[id];
			createdChildEdge._child = createIndex(row, column, id);
			return createdChildEdge._child;
		}
		
		edge._child = createIndex(row, column, _reverse_find.at(&edge));

		return edge._child;
	}
	
	const auto& parentEdge = modelEdge(parent);
	if (parentEdge.hasChild(row, column, _resourceManager))
	{
		auto childEdge = parentEdge.childEdge(row, column, _resourceManager);
		if (!hasModelEdgeIndex(childEdge))
		{
			childEdge._parent = parent;
			qintptr id = const_cast<ProjectModel*>(this)->createModelEdgeIndex(childEdge);
			auto& createdChildEdge = const_cast<ProjectModel*>(this)->_direct_find[id];
			createdChildEdge._child = createIndex(row, column, id);
			return createdChildEdge._child;
		}
		childEdge._child = createIndex(row, column, _reverse_find.at(&childEdge));
			return childEdge._child;
	}
	return createIndex(row, column, parent.internalId());
}

QModelIndex ProjectModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	auto edge = modelEdge(index);
	return edge._parent;
}

int ProjectModel::rowCount(const QModelIndex& index) const
{
	if (!index.isValid())
		return 2;

	const auto& edge = modelEdge(index);
	return edge.childRows(index.row(), index.column(), _resourceManager);
}

int ProjectModel::rowColumns(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return 2;

	const auto& edge = modelEdge(parent);
	return edge.rowColumns(parent.row(), parent.column(), _resourceManager);
}

int ProjectModel::columnCount(const QModelIndex& index) const
{
	if (!index.isValid())
		return 1;
	const auto& edge = modelEdge(index);
	return edge.childColumns(index.row(), index.column(), _resourceManager);
}

int ProjectModel::childCount(const QModelIndex& index) const
{
	if (!index.isValid())
		return 2;

	const auto& edge = modelEdge(index);
	return edge.childCount(_resourceManager);
}

bool ProjectModel::hasChildren(int row, int column, const QModelIndex& index) const
{
	if (!index.isValid())
		return index.row() < 2;

	const auto& edge = modelEdge(index);
	return edge.hasChild(row, column, _resourceManager);
}

bool ProjectModel::hasChildren(const QModelIndex& parent) const
{
	return QAbstractItemModel::hasChildren(parent);
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

bool ProjectModel::setData(const QModelIndex& index, const QVariant& value,
	int role)
{
	auto& edge = modelEdge(index);
	//if (NULL != node)
	//{
	//	if (role == Qt::BackgroundRole && index.isValid())
	//	{
	//		node->setColor(value.value<QColor>());
	//		return true;
	//	}

	if (role == Qt::EditRole)
	{
		int children = edge.childCount(_resourceManager);
		if (edge.setData(index.row(), index.column(), value, _resourceManager))
			emit dataChanged(index, index);
		int new_children = edge.childCount(_resourceManager);
		if (children != new_children)
		{
			int difference = new_children - children;
			if (difference > 0)
			{
				emit beginInsertRows(index, children - 1, children - 1 + difference);
				emit endInsertRows();
				emit beginInsertChildren(index, children - 1, children - 1 + difference);
				emit endInsertChildren();
			}
			else {
				emit beginRemoveRows(index, children - 1 + difference, children - 1);
				emit endRemoveRows();
				emit beginRemoveChildren(index, children - 1 + difference, children - 1);
				emit endRemoveChildren();
			}
		}
	}
	return false;
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
		if (_resourceManager.isProjectFileOpen(index.row(), project_type))
		{
			emit beginRemoveRows(index, 0, 0);
			int project_index = _resourceManager.projectFileIndex(index.row(), project_type);
			edge._project = -1;
			_resourceManager.closeProjectFile(index.row(), project_type);
			deleteAllModelEdgeIndexesForFile(project_index);
			emit endRemoveRows();
		}
		else {
			emit beginInsertRows(index, 0, 0);
			_resourceManager.openProjectFile(index.row(), project_type);
			edge._project = _resourceManager.projectFileIndex(index.row(), project_type);
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

NodeType ProjectModel::nodeType(const QModelIndex& index)
{
	auto type = modelEdge(index).type();
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
		auto child_index = this->index(i, 0, index);
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

QAbstractItemModel* ProjectModel::editModel(const QModelIndex& index, AssetType type)
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
			auto top_behavior_index = getChildAssetProxy(index, NodeType::BehaviorHkxNode);
			if (top_behavior_index.isValid())
			{
				auto events_node = getChildAssetProxy(top_behavior_index, NodeType::behaviorEventNames);
				if (events_node.isValid())
					return new SelectionProxyModel(this, events_node);
			}
			return nullptr;
		}
		return new QStringListModel(_resourceManager.assetsList(edge._project, type));
	}
	default:
		break;
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
