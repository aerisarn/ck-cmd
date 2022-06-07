#include "ProjectTreeModel.h"
#include <src/hkx/HkxLinkedTableVariant.h>


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


QVariant ProjectTreeModel::ModelEdge::data(int row, int column) const
{
	switch (_childType)
	{
	case NodeType::ProjectNode:
		return reinterpret_cast<ProjectNode*>(_childItem)->data(column);
	case NodeType::HavokNative:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		if (column == 0)
		{
			return HkxVariant(*variant).name();
		}
		else 
		{
			HkxLinkedTableVariant v(*variant);
			return v.data(row, column - 1);
		}
	}
	default:
		return QVariant();
	}
	return QVariant();
}

ProjectTreeModel::ModelEdge ProjectTreeModel::ModelEdge::childEdge(int index)
{
	switch (_childType)
	{
	case NodeType::ProjectNode:
	{
		ProjectNode* node = reinterpret_cast<ProjectNode*>(_childItem);
		if (node->isVariant())
		{
			auto variant = node->variant();
			auto file = node->file();
			auto& link = HkxLinkedTableVariant(*variant).links().at(index);
			return ModelEdge(node, file, child._row, child._column, (hkBariant*)child._ref);		
		}
		return ModelEdge(node, -1, index, 0, node->child(index));
	}
	case NodeType::HavokNative:
	{
		hkVariant* variant = reinterpret_cast<hkVariant*>(_childItem);
		if (column == 0)
		{
			return HkxVariant(*variant).name();
		}
		else
		{
			HkxLinkedTableVariant v(*variant);
			return v.data(row, column - 1);
		}
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


qintptr ProjectTreeModel::modelEdgeIndex(const ProjectTreeModel::ModelEdge& edge) const
{
	return _reverse_find.at(const_cast<ModelEdge*>(&edge));
}

const ProjectTreeModel::ModelEdge& ProjectTreeModel::modelEdge(const QModelIndex& index) const
{
	return _direct_find.at(index.internalId());
}

ProjectTreeModel::ModelEdge& ProjectTreeModel::modelEdge(const QModelIndex& index)
{
	return _direct_find[index.internalId()];
}

bool ProjectTreeModel::hasModelEdgeIndex(const ProjectTreeModel::ModelEdge& edge) const
{
	return _reverse_find.find(const_cast<ModelEdge*>(&edge)) == _reverse_find.end();
}

qintptr ProjectTreeModel::createModelEdgeIndex(const ProjectTreeModel::ModelEdge& edge)
{
	qintptr result = _direct_find.size();
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

	if (role == Qt::BackgroundRole && index.isValid())
	{
		ProjectNode* item = getNode(index);
		return QBrush(item->color());
	}

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	return modelEdge(index).data(index.row(), index.column());
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
		ModelEdge edge(
			_rootNode,
			-1,
			row,
			column,
			_rootNode->child(row)
		);

		if (!hasModelEdgeIndex(edge))
		{
			qintptr id = const_cast<ProjectTreeModel*>(this)->createModelEdgeIndex(edge);
			return createIndex(row, column, id);
		}
		
		return createIndex(row, column, _reverse_find.at(&edge));
	}
	
	auto parentEdge = modelEdge(parent);
	ModelEdge edge(
		parentEdge._childItem,
		-1,
		row,
		column,
		_rootNode->child(row)
	);

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