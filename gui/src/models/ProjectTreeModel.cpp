#include "ProjectTreeModel.h"

using namespace ckcmd::HKX;

QModelIndex ProjectTreeModel::mapToSource(const QModelIndex& proxyIndex) const
{
	if (!proxyIndex.isValid())
		return QModelIndex();

	auto source_index = sourceModel()->createIndex(0, 0, proxyIndex.internalId());
	auto source_parent = sourceModel()->parent(source_index);
	
	//Optmimization
	switch (sourceModel()->nodeType(source_parent))
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::animationNames:
	case NodeType::deformableSkinNames:
	case NodeType::behaviorEventNames:
	case NodeType::behaviorVariableNames:
		return sourceModel()->createIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.internalId());
	default:
		break;
	}

	int source_rows = 0; 
	int source_columns = 0;
	//Optmimization
	if (sourceModel()->nodeType(source_parent) == NodeType::CharacterHkxNode)
	{
		source_rows = sourceModel()->childCount(source_parent);
		source_columns = 1;
	}
	else {
		source_rows = sourceModel()->rowCount(source_parent);
		source_columns = sourceModel()->columnCount(source_parent);
	}


	int target_children = proxyIndex.row();
	int child_index = 0;
	for (int r = 0; r < source_rows; ++r)
	{
		for (int c = 0; c < source_columns; ++c)
		{
			if (sourceModel()->hasChildren(r, c, source_parent))
			{
				if (child_index == target_children)
					return sourceModel()->createIndex(r, c, proxyIndex.internalId());
				child_index++;
			}
		}
	}
	return QModelIndex();
}

QModelIndex ProjectTreeModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();
	auto source_parent = sourceModel()->parent(sourceIndex);
	auto type = sourceModel()->nodeType(source_parent);
	if (type == NodeType::animationName)
		int debug = 1;
	switch (type)
	{
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	case NodeType::animationNames:
	case NodeType::deformableSkinNames:
	case NodeType::behaviorEventNames:
	case NodeType::behaviorVariableNames:
	{
		QModelIndex index = sourceModel()->index(sourceIndex.row(), sourceIndex.column(), source_parent);
		return createIndex(sourceIndex.row(), sourceIndex.column(), index.internalId());
	}
	default:
		break;
	}

	int child_index = 0;
	int source_rows = 0;
	int source_columns = 0;
	//Optmimization
	if (sourceModel()->nodeType(source_parent) == NodeType::CharacterHkxNode)
	{
		source_rows = sourceModel()->childCount(source_parent);
		source_columns = 1;
	}
	else {
		source_rows = sourceModel()->rowCount(source_parent);
		source_columns = sourceModel()->columnCount(source_parent);
	}
	for (int r = 0; r < source_rows; ++r)
	{
		for (int c = 0; c < source_columns; ++c)
		{
			if (sourceModel()->hasChildren(r, c, source_parent))
			{
				QModelIndex index = sourceModel()->index(r, c, source_parent);
				if (index == sourceIndex)
					return createIndex(child_index, 0, index.internalId());
			}
			child_index++;
		}
	}

	return createIndex(0, 0, source_parent.internalId());
}

int ProjectTreeModel::columnCount(const QModelIndex& parent) const
{
	if (nullptr == sourceModel())
		return 0;
	return 1;
}

int ProjectTreeModel::rowCount(const QModelIndex& parent) const
{
	if (nullptr == sourceModel()) 
		return 0;
	int row = parent.row();
	auto count = sourceModel()->childCount(mapToSource(parent));
	return count;
}

QModelIndex ProjectTreeModel::parent(const QModelIndex& child) const {
	if (!child.isValid() || !sourceModel()) return QModelIndex();
	Q_ASSERT(child.model() == this);
	QModelIndex mapped_child = mapToSource(child);
	QModelIndex mapped_parent = sourceModel()->parent(mapped_child);
	QModelIndex unmapped_parent = mapFromSource(mapped_parent);
	return unmapped_parent;
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex& parent) const {
	if (!sourceModel()) return {};
	Q_ASSERT(!parent.isValid() || parent.model() == this);
	QModelIndex mapped_parent = mapToSource(parent);
	QModelIndex mapped_child = sourceModel()->index(row, column, mapped_parent);
	QModelIndex unmapped_child = mapFromSource(mapped_child);
	return unmapped_child;
}

QVariant ProjectTreeModel::data(const QModelIndex& proxyIndex, int role) const
{
	return sourceModel()->data(mapToSource(proxyIndex), role);
}

void ProjectTreeModel::select(const QModelIndex& index)
{
	if (nullptr != sourceModel())
		sourceModel()->select(mapToSource(index));
}

void ProjectTreeModel::activate(const QModelIndex& index)
{
	if (nullptr != sourceModel())
	{
		auto source_index = mapToSource(index);
		auto type = sourceModel()->nodeType(source_index);
		if (type == NodeType::CharacterNode || type == NodeType::MiscNode)
		{
			if (rowCount(index) <= 0)
			{
				emit beginInsertRows(index, 0, 0);
				sourceModel()->activate(source_index);
				emit endInsertRows();
			}
			else {
				emit beginRemoveRows(index, 0, 0);
				sourceModel()->activate(source_index);
				emit endRemoveRows();
			}
		}
	}
}

bool ProjectTreeModel::insertRows(int row, int count, const QModelIndex& index)
{
	emit beginInsertRows(index, 0, 0);
	bool result = sourceModel()->insertRows(row, count, mapToSource(index));
	emit endInsertRows();
	return result;
}

bool ProjectTreeModel::removeRows(int row, int count, const QModelIndex& index)
{
	emit beginRemoveRows(index, 0, 0);
	bool result = sourceModel()->removeRows(row, count, mapToSource(index));
	emit endRemoveRows();
	return result;
}

//bool ProjectTreeModel::hasChildren(const QModelIndex& parent) const
//{
//	if (nullptr != sourceModel())
//	{
//		bool hasChildren = sourceModel()->hasChildren(mapToSource(parent));
//		return hasChildren;
//	}
//	return false;
//}

