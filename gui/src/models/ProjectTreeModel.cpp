#include "ProjectTreeModel.h"

#include <QBrush>

using namespace ckcmd::HKX;

static size_t runtime_edge_index = 0;

ProjectTreeModel::ProjectTreeModel(CommandManager& commandManager, ResourceManager& resourceManager, QObject* parent) :
	_commandManager(commandManager),
	_resourceManager(resourceManager),
	_actionsManager(_commandManager, _resourceManager),
	QAbstractItemModel(parent)
{
}

qintptr ProjectTreeModel::modelEdgeIndex(const ModelEdge& edge) const
{
	return _reverse_find.at(const_cast<ModelEdge*>(&edge));
}

const ModelEdge& ProjectTreeModel::modelEdge(const QModelIndex& index) const
{
	return _direct_find.at(index.internalId());
}

ModelEdge& ProjectTreeModel::modelEdge(const QModelIndex& index)
{
	return _direct_find[index.internalId()];
}

bool ProjectTreeModel::hasModelEdgeIndex(const ModelEdge& edge) const
{
	return _reverse_find.find(const_cast<ModelEdge*>(&edge)) != _reverse_find.end();
}

qintptr ProjectTreeModel::createModelEdgeIndex(const ModelEdge& edge)
{
	qintptr result = runtime_edge_index++;
	_direct_find.insert({ result, edge });
	_reverse_find.insert({ &_direct_find[result], result });
	return result;
}

void  ProjectTreeModel::deleteAllModelEdgeIndexesForFile(int project_file)
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

QVariant ProjectTreeModel::data(const QModelIndex& index, int role) const
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
QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex& parent) const
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
		edge._childItem = row == 0 ? (void*)(&_charactersNode) : (void*)(&_miscsNode);

		if (!hasModelEdgeIndex(edge))
		{
			qintptr id = const_cast<ProjectTreeModel*>(this)->createModelEdgeIndex(edge);
			edge._child = createIndex(row, column, id);
			return edge._child;
		}
		
		edge._child = createIndex(row, column, _reverse_find.at(&edge));

		return edge._child;
	}
	
	auto parentEdge = modelEdge(parent);
	auto childEdge = parentEdge.childEdge(row, _resourceManager);
	if (!hasModelEdgeIndex(childEdge))
	{
		childEdge._parent = parent;
		qintptr id = const_cast<ProjectTreeModel*>(this)->createModelEdgeIndex(childEdge);
		childEdge._child = createIndex(row, column, id);
		return childEdge._child;
	}
	childEdge._child = createIndex(row, column, _reverse_find.at(&childEdge));
	return childEdge._child;
}

QModelIndex ProjectTreeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	auto edge = modelEdge(index);
	return edge._parent;
}

int ProjectTreeModel::rowCount(const QModelIndex& index) const
{
	int count = 0;
	if (!index.isValid())
		return 2;

	const auto& edge = modelEdge(index);
	return edge.childCount(_resourceManager);
}

int ProjectTreeModel::columnCount(const QModelIndex& index) const
{
	return 1;
}

QVariant ProjectTreeModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{

	return QVariant();
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return QAbstractItemModel::flags(index);
}

bool ProjectTreeModel::setData(const QModelIndex& index, const QVariant& value,
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

	if (index.column() == 0 && role == Qt::EditRole)
	{
		if (modelEdge(index).setData(index.row(), index.column(), value, _resourceManager));
			emit dataChanged(index, index);
	}
	return false;
}


void ProjectTreeModel::select(const QModelIndex& index)
{

}

void ProjectTreeModel::activate(const QModelIndex& index)
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

NodeType ProjectTreeModel::nodeType(const QModelIndex& index)
{
	return modelEdge(index).type();
}

hkVariant* ProjectTreeModel::variant(const QModelIndex& index)
{
	return reinterpret_cast<hkVariant*>(modelEdge(index)._childItem);
}

bool ProjectTreeModel::isVariant(const QModelIndex& index)
{
	return ::isVariant(nodeType(index));
}