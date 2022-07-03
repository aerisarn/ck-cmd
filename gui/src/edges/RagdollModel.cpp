#include <src/edges/RagdollModel.h>

using namespace ckcmd::HKX;

std::vector<NodeType> RagdollModel::handled_types() const
{
	return {
		NodeType::RagdollHkxNode,
		NodeType::RagdollBones,
		NodeType::RagdollBone
	};
}

int RagdollModel::supports() const
{
	return 1;
}

const char* RagdollModel::supportName(int support_index) const
{
	switch (support_index) {
	case 0:
		return "Bones";
	default:
		break;
	}
	return "Invalid FSM Entry";
}

NodeType  RagdollModel::supportType(int support_index) const
{
	switch (support_index) {
	case 0:
		return NodeType::RagdollBones;
	default:
		break;
	}
	return NodeType::Invalid;
}

hkaRagdollInstance* RagdollModel::variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* ragdoll = reinterpret_cast<hkaRagdollInstance*>(variant->m_object);
		return ragdoll;
	}
	return nullptr;
}

int RagdollModel::rows(const ModelEdge& edge, ResourceManager& manager) const
{
	auto* ragdoll = variant(edge);
	if (edge.childType() == NodeType::RagdollBones)
	{
		return ragdoll->m_rigidBodies.getSize();
	}
	if (edge.childType() == NodeType::RagdollBone)
	{
		return 0;
	}
	if (ragdoll == nullptr)
		return 0;
	return SupportEnhancedEdge::rows(edge, manager);
}

int RagdollModel::columns(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	return SupportEnhancedEdge::columns(row, edge, manager);
}

int RagdollModel::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	if (
		edge.childType() == NodeType::RagdollBones ||
		edge.childType() == NodeType::RagdollBone
		)
	{
		return rows(edge, manager);
	}
	auto* ragdoll = variant(edge);
	if (ragdoll == nullptr)
		return 0;
	return SupportEnhancedEdge::childCount(edge, manager);
}

std::pair<int, int> RagdollModel::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType())
	{
	case NodeType::RagdollBones:
		return { index, 0 };
	case NodeType::RagdollBone:
		return { -1, -1 };
	default:
		break;
	}
	return SupportEnhancedEdge::child(index, edge, manager);
}

int RagdollModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType())
	{
	case NodeType::RagdollBones:
		return row;
	default:
		break;
	}
	return SupportEnhancedEdge::childIndex(row, column, edge, manager);
}

ModelEdge RagdollModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.type())
	{
	case NodeType::RagdollBones:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::RagdollBone);
	case NodeType::RagdollBone:
		throw std::runtime_error("Child from leaf node requested!");
	default:
		break;
	}
	return SupportEnhancedEdge::child(row, column, edge, manager);
}

QVariant RagdollModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	auto* ragdoll = variant(edge);
	if (edge.childType() == NodeType::RagdollBones)
	{
		if (column == 0)
		{
			return supportName(0);
		}
		return "InvalidColumn";
	}
	if (edge.childType() == NodeType::RagdollBone)
	{
		if (column == 0)
		{
			return ragdoll->m_skeleton->m_bones[edge.row()].m_name.cString();
		}
		return "InvalidColumn";
	}
	if (row == 0 && column == 0)
	{
		return "Ragdoll";
	}
	return SupportEnhancedEdge::data(row, column, edge, manager);
}