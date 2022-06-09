#include "ProjectTreeModel.h"
#include <QBrush>

using namespace ckcmd::HKX;



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
	qintptr result = _direct_find.size() + 1;
	_direct_find.insert({ result, edge });
	_reverse_find.insert({ &_direct_find[result], result });
	return result;
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
		ModelEdge edge;/*(
			nullptr,
			-1,
			-1,
			row,
			column,
			_rootNode->child(row)
		);*/


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

	return modelEdge(index).childCount(_resourceManager);
}

int ProjectTreeModel::columnCount(const QModelIndex& index) const
{
	return 1;
}

QVariant ProjectTreeModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{
	//if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	//	return _rootNode->data(section);

	return QVariant();
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;
	//auto node = getNode(index);
	//if (node->type() == ProjectNode::NodeType::event_node)
	//	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);

	return QAbstractItemModel::flags(index);
}

bool ProjectTreeModel::setData(const QModelIndex& index, const QVariant& value,
	int role)
{
	//auto node = getNode(index);
	//if (NULL != node)
	//{
	//	if (role == Qt::BackgroundRole && index.isValid())
	//	{
	//		node->setColor(value.value<QColor>());
	//		return true;
	//	}

	//	if (node->type() == ProjectNode::NodeType::event_node)
	//	{
	//		BehaviorBuilder* builder = (BehaviorBuilder*)_resourceManager.fieldsHandler(node->data(2).value<int>());
	//		return builder->renameEvent(node->data(3).value<int>(), value.value<QString>());
	//	}
	//}
	return false;
}

//QModelIndex ProjectTreeModel::getIndex(ProjectNode* node) const
//{
//	//ProjectNode* parentItem = node->parentItem();
//
//	if (node == _rootNode)
//		return QModelIndex();
//
//	return createIndex(node->row(), 0, node);
//}

//bool ProjectTreeModel::insertRows(int row, int count, const QModelIndex& parent = QModelIndex())
//{
//	beginInsertRows(parent,
//		row,
//		row + count);
//
//	for (int i = row; i < row + count; i++)
//	{
//		getNode(parent)->appendChild(new _resourceManager.);
//	}
//
//	this->endInsertRows();
//
//	return true;
//}

void ProjectTreeModel::select(const QModelIndex& index)
{

}

void ProjectTreeModel::activate(const QModelIndex& index)
{
	auto edge = modelEdge(index);
	if (edge._childType == NodeType::CharacterNode)
	{
		if (_resourceManager.isCharacterFileOpen(index.row()))
		{
			emit beginRemoveRows(index, 0, 2);
			_resourceManager.closeCharacterFile(index.row());
			emit endRemoveRows();
		}
		else {
			emit beginInsertRows(index, 0, 2);
			_resourceManager.openCharacterFile(index.row());
			emit endInsertRows();
		}

		//ProjectNode* node_activated = reinterpret_cast<ProjectNode*>(edge._childItem);
		//if (node_activated->isProjectRoot())
		//{
		//	if (node_activated->childCount() == 0)
		//	{
		//		emit beginInsertRows(index, 0, 2);
		//		_actionsManager.OpenProject(node_activated);
		//		emit endInsertRows();
		//	}
		//	else {
		//		emit beginRemoveRows(index, 0, 2);
		//		_actionsManager.CloseProject(node_activated);
		//		emit endRemoveRows();
		//	}
		//}
	}
//else if (node_clicked->isSkeleton()) {
//	
//	_simulation->addSkeleton(
//		node_clicked->data(1).toString().toUtf8().constData()
//	);
//}
}

NodeType  ProjectTreeModel::nodeType(const QModelIndex& index)
{
	return modelEdge(index).type();
}