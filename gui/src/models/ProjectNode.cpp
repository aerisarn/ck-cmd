#include "ProjectNode.h"

using namespace ckcmd::HKX;

ProjectNode::ProjectNode(NodeType type, const QVector<QVariant>& data, ProjectNode* parent)
	: m_type(type), m_itemData(data), m_parentItem(parent)
{}

ProjectNode::~ProjectNode()
{
	qDeleteAll(m_childItems);
}

ProjectNode* ProjectNode::appendChild(ProjectNode* item)
{
	m_childItems.append(item);
	return m_childItems.last();
}

ProjectNode* ProjectNode::child(int row)
{
	if (row < 0 || row >= m_childItems.size())
		return nullptr;
	return m_childItems.at(row);
}

ProjectNode* ProjectNode::setChild(int row, ProjectNode* new_child)
{
	if (row < 0 || row >= m_childItems.size())
		return nullptr;
	auto old_node = m_childItems.at(row);
	m_childItems[row] = new_child;
	return old_node;
}

int ProjectNode::childCount() const
{
	return m_childItems.count();
}

int ProjectNode::row() const
{
	if (m_parentItem)
		return m_parentItem->m_childItems.indexOf(const_cast<ProjectNode*>(this));

	return 0;
}

int ProjectNode::columnCount() const
{
	return m_itemData.count();
}

ProjectNode* ProjectNode::parentItem()
{
	return m_parentItem;
}

QVariant ProjectNode::data(int column) const
{
	if (column < 0 || column >= m_itemData.size())
		return QVariant();
	return m_itemData.at(column);
}

ProjectNode::NodeType ProjectNode::type() const {
	return m_type;
}

bool ProjectNode::isProjectRoot() const {
	return m_type == ProjectNode::NodeType::character_node 
		|| m_type == ProjectNode::NodeType::misc_node 
		|| m_type == ProjectNode::NodeType::project_node;
}

bool ProjectNode::isSkeleton() const {
	return m_type == ProjectNode::NodeType::skeleton_node;
}

bool ProjectNode::isVariant() const {
	return m_type == ProjectNode::NodeType::hkx_character_node
		|| m_type == ProjectNode::NodeType::hkx_project_node
		|| m_type == ProjectNode::NodeType::hkx_node;
}
