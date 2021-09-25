#include "ProjectNode.h"

using namespace ckcmd::HKX;

ProjectNode::ProjectNode(NodeType type, const QVector<QVariant>& data, ProjectNode* parent)
	: m_type(type), m_itemData(data)
{
	m_parentItems.push_back(parent);
}

ProjectNode::~ProjectNode()
{
	qDeleteAll(m_childItems);
}

void ProjectNode::appendData(const QVariant& value)
{
	m_itemData.append(value);
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

ProjectNode* ProjectNode::removeChild(int row)
{
	if (row < 0 || row >= m_childItems.size())
		return nullptr;
	auto old_node = m_childItems.at(row);
	m_childItems.remove(row);
	return old_node;
}

void ProjectNode::insertChild(int row, ProjectNode* new_child)
{
	if (row < 0)
		return;
	m_childItems.insert(row, new_child);
}

int ProjectNode::childCount() const
{
	return m_childItems.count();
}

int ProjectNode::row() const
{
	if (!m_parentItems.isEmpty())
		return m_parentItems.first()->m_childItems.indexOf(const_cast<ProjectNode*>(this));

	return 0;
}

int ProjectNode::columnCount() const
{
	return m_itemData.count();
}

ProjectNode* ProjectNode::parentItem()
{
	if (m_parentItems.isEmpty())
		return nullptr;
	return m_parentItems.first();
}

int ProjectNode::parentCount() const
{
	return m_parentItems.count();
}

ProjectNode* ProjectNode::parentItem(int row)
{
	if (row < 0)
		return nullptr;
	if (m_parentItems.isEmpty())
		return nullptr;
	/*for (int i = 0; i < m_parentItems.size(); i++) {
		if (m_parentItems[i]->child(row) == this)
			return m_parentItems[i];
	}*/

	return m_parentItems[row];

	return m_parentItems.first();
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

bool ProjectNode::canSaveOrExport() const
{
	return m_type == ProjectNode::NodeType::misc_node
		|| m_type == ProjectNode::NodeType::character_node;
}

bool ProjectNode::isCharacter() const
{
	return m_type == ProjectNode::NodeType::character_node;
}


void ProjectNode::addParent(ProjectNode* new_parent)
{
	if (!m_parentItems.contains(new_parent))
		m_parentItems.push_back(new_parent);
}

void ProjectNode::setParent(ProjectNode* new_parent)
{
	if (!m_parentItems.contains(new_parent))
	{
		m_parentItems.push_front(new_parent);
		m_itemData[2] = new_parent->data(1);
	}
}

void ProjectNode::removeParent(ProjectNode* parent)
{
	if (m_parentItems.contains(parent))
		m_parentItems.remove(m_parentItems.indexOf(parent));
}