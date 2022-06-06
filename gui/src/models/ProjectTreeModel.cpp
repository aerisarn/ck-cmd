#include "ProjectTreeModel.h"
#include <src/hkx/HkxTableVariant.h>


#include <src/hkx/BehaviorBuilder.h>
#include <QBrush>

using namespace ckcmd::HKX;


ProjectTreeModel::ModelEdge::ModelEdge(ProjectNode* parent, int file, int row, int column, ProjectNode* child)
{
	_parentType = NodeType::ProjectNode;
	_parentItem = reinterpret_cast<void*>(parent);
	_file = file;
	_row = row;
	_column = column;
	_childType = NodeType::ProjectNode;;
	_childItem = reinterpret_cast<void*>(child);
}

ProjectTreeModel::ModelEdge::ModelEdge(ProjectNode* parent, int file, int row, int column, hkVariant* child)
{
	_parentType = NodeType::ProjectNode;
	_parentItem = reinterpret_cast<void*>(parent);
	_file = file;
	_row = row;
	_column = column;
	_childType = NodeType::HavokNative;;
	_childItem = reinterpret_cast<void*>(child);
}

ProjectTreeModel::ModelEdge::ModelEdge(hkVariant* parent, int file, int row, int column, hkVariant* child)
{
	_parentType = NodeType::HavokNative;
	_parentItem = reinterpret_cast<void*>(parent);
	_file = file;
	_row = row;
	_column = column;
	_childType = NodeType::HavokNative;;
	_childItem = reinterpret_cast<void*>(child);
}


QVariant ProjectTreeModel::ModelEdge::data(int row, int column)
{
	switch (_childType)
	{
	case NodeType::ProjectNode:
		return reinterpret_cast<ProjectNode*>(_childItem)->data(column);
	case NodeType::HavokNative:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		HkxVariant
	}
	default:
		return QVariant();
	}
	return QVariant();
}

ProjectTreeModel::ProjectTreeModel(CommandManager& commandManager, ResourceManager& resourceManager, QObject* parent) :
	_commandManager(commandManager),
	_resourceManager(resourceManager),
	QAbstractItemModel(parent)
{
}



/*
** AbstractItemModel(required methods)
*/

QVariant ProjectTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::BackgroundRole && index.isValid())
	{
		ProjectNode* item = getNode(index);
		return QBrush(item->color());
	}

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	ProjectNode* item = getNode(index);

	return item->data(index.column());
}



//row = field
//column = number of values in the field (1 scalar, >1 vector/matrix)
QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();


	if (!parent.isValid())
	{


		//root handling
		ModelEdge edge =
		{
			nt_ProjectNode,
			_rootNode,
			row,
			column,
			nt_ProjectNode,


		}
		edge.parentType = nt_ProjectNode;

		parentEdge = new GraphModelIndex();
		parentEdge->parent = QModelIndex();
		parentEdge->parentItem = _rootNode;
		parentEdge->child = createIndex(row, 0, parentEdge);
		parentEdge->childItem = _rootNode->child(row);
		const_cast<ProjectTreeModel*>(this)->holder.insert(parentEdge);
		return parentEdge->child;
	}
	
	parentEdge = static_cast<GraphModelIndex*>(parent.internalPointer());

	ProjectNode* newParentNode = parentEdge->childItem;
	GraphModelIndex* new_edge = new GraphModelIndex();
	new_edge->childItem = newParentNode->child(row);
	new_edge->parent = parent;
	new_edge->parentItem = newParentNode;
	new_edge->child = createIndex(row, 0, new_edge);
	const_cast<ProjectTreeModel*>(this)->holder.insert(new_edge);

	if (new_edge->childItem)
	{
		return new_edge->child;
	}
	return QModelIndex();
}

QModelIndex ProjectTreeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	GraphModelIndex* edge = static_cast<GraphModelIndex*>(index.internalPointer());

	return edge->parent;
}

int ProjectTreeModel::rowCount(const QModelIndex& index) const
{
	ProjectNode* parentItem;

	if (!index.isValid())
		parentItem = _rootNode;
	else
		parentItem = getNode(index);

	return parentItem->childCount();
}

int ProjectTreeModel::columnCount(const QModelIndex& index) const
{
	if (index.isValid())
		return getNode(index)->columnCount();
	return _rootNode->columnCount();
}

QVariant ProjectTreeModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return _rootNode->data(section);

	return QVariant();
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;
	auto node = getNode(index);
	if (node->type() == ProjectNode::NodeType::event_node)
		return Qt::ItemIsEditable | QAbstractItemModel::flags(index);

	return QAbstractItemModel::flags(index);
}

bool ProjectTreeModel::setData(const QModelIndex& index, const QVariant& value,
	int role)
{
	auto node = getNode(index);
	if (NULL != node)
	{
		if (role == Qt::BackgroundRole && index.isValid())
		{
			node->setColor(value.value<QColor>());
			return true;
		}

		if (node->type() == ProjectNode::NodeType::event_node)
		{
			BehaviorBuilder* builder = (BehaviorBuilder*)_resourceManager.fieldsHandler(node->data(2).value<int>());
			return builder->renameEvent(node->data(3).value<int>(), value.value<QString>());
		}
	}
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