#include "ProjectTreeModel.h"

#include <src/hkx/BehaviorBuilder.h>

using namespace ckcmd::HKX;

ProjectTreeModel::ProjectTreeModel(CommandManager& commandManager, ResourceManager& resourceManager, QObject* parent) :
	_commandManager(commandManager),
	_resourceManager(resourceManager),
	QAbstractItemModel(parent)
{
}

ProjectNode* ProjectTreeModel::getNode(const QModelIndex& index) const 
{ 
	return static_cast<ProjectNode*>(index.internalPointer()); 
}

/*
** AbstractItemModel(required methods)
*/

QVariant ProjectTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	ProjectNode* item = static_cast<ProjectNode*>(index.internalPointer());

	return item->data(index.column());
}

//row = field
//column = number of values in the field (1 scalar, >1 vector/matrix)
QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	ProjectNode* parentItem;

	if (!parent.isValid())
		parentItem = _rootNode;
	else
		parentItem = static_cast<ProjectNode*>(parent.internalPointer());

	ProjectNode* childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	return QModelIndex();
}

QModelIndex ProjectTreeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	ProjectNode* childItem = static_cast<ProjectNode*>(index.internalPointer());
	ProjectNode* parentItem = childItem->parentItem();

	if (parentItem == _rootNode)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int ProjectTreeModel::rowCount(const QModelIndex& parent) const
{
	ProjectNode* parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = _rootNode;
	else
		parentItem = static_cast<ProjectNode*>(parent.internalPointer());

	return parentItem->childCount();
}

int ProjectTreeModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return static_cast<ProjectNode*>(parent.internalPointer())->columnCount();
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
	if (node->type() == ProjectNode::NodeType::event_node)
	{
		BehaviorBuilder* builder = (BehaviorBuilder * )_resourceManager.fieldsHandler(node->data(2).value<int>());
		return builder->renameEvent(node->data(3).value<int>(), value.value<QString>());
	}
	return false;
}

QModelIndex ProjectTreeModel::getIndex(ProjectNode* node) const
{
	ProjectNode* parentItem = node->parentItem();

	if (parentItem == _rootNode)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

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