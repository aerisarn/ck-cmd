#include <src/edges/SkeletonModel.h>

using namespace ckcmd::HKX;

std::vector<NodeType> SkeletonModel::handled_types() const
{
	return {
		NodeType::SkeletonHkxNode,
		NodeType::SkeletonBones,
		NodeType::SkeletonBone,
		NodeType::SkeletonFloats,
		NodeType::SkeletonFloat
	};
}

int SkeletonModel::supports() const
{
	return 2;
}

const char* SkeletonModel::supportName(int support_index) const
{
	switch (support_index) {
	case 0:
		return "Bones";
	case 1:
		return "Floats";
	default:
		break;
	}
	return "Invalid Entry";
}

NodeType SkeletonModel::supportType(int support_index) const
{
	switch (support_index) {
	case 0:
		return NodeType::SkeletonBones;
	case 1:
		return NodeType::SkeletonFloats;
	default:
		break;
	}
	return NodeType::Invalid;
}

hkaSkeleton* SkeletonModel::variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* ragdoll = reinterpret_cast<hkaSkeleton*>(variant->m_object);
		return ragdoll;
	}
	return nullptr;
}

int SkeletonModel::rows(const ModelEdge& edge, ResourceManager& manager) const
{
	auto* skeleton = variant(edge);
	if (nullptr != skeleton)
	{
		if (edge.childType() == NodeType::SkeletonBones)
		{
			return skeleton->m_bones.getSize();
		}
		if (edge.childType() == NodeType::SkeletonFloats)
		{
			return skeleton->m_floatSlots.getSize();
		}
		if (edge.childType() == NodeType::SkeletonBone)
		{
			return 0;
		}
		if (edge.childType() == NodeType::SkeletonFloat)
		{
			return 0;
		}
		return SupportEnhancedEdge::rows(edge, manager);
	}
	return 0;
}

int SkeletonModel::columns(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	auto* skeleton = variant(edge);
	if (nullptr != skeleton)
	{
		return SupportEnhancedEdge::columns(row, edge, manager);
	}
	return 0;
}

int SkeletonModel::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	auto* skeleton = variant(edge);
	if (nullptr != skeleton)
	{
		if (
			edge.childType() == NodeType::SkeletonBones ||
			edge.childType() == NodeType::SkeletonFloats ||
			edge.childType() == NodeType::SkeletonBone ||
			edge.childType() == NodeType::SkeletonFloat
			)
		{
			return rows(edge, manager);
		}
		return SupportEnhancedEdge::childCount(edge, manager);
	}
	return 0;
}

std::pair<int, int> SkeletonModel::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType())
	{
	case NodeType::SkeletonBones:
	case NodeType::SkeletonFloats:
		return { index, 0 };
	case NodeType::SkeletonBone:
	case NodeType::SkeletonFloat:
		return { -1, -1 };
	default:
		break;
	}
	return SupportEnhancedEdge::child(index, edge, manager);
}

int SkeletonModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType())
	{
	case NodeType::SkeletonBones:
	case NodeType::SkeletonFloats:
		return row;
	default:
		break;
	}
	return SupportEnhancedEdge::childIndex(row, column, edge, manager);
}

ModelEdge SkeletonModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.type())
	{
	case NodeType::SkeletonBones:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::SkeletonBone);
	case NodeType::SkeletonFloats:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::SkeletonFloat);
	default:
		break;
	}
	return SupportEnhancedEdge::child(row, column, edge, manager);
}

QVariant SkeletonModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	auto* skeleton = variant(edge);
	if (nullptr != skeleton)
	{
		if (edge.childType() == NodeType::SkeletonBones)
		{
			if (column == 0)
			{
				return supportName(0);
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::SkeletonFloats)
		{
			if (column == 0)
			{
				return supportName(1);
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::SkeletonBone)
		{
			if (column == 0)
			{
				return skeleton->m_bones[edge.row()].m_name.cString();
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::SkeletonFloat)
		{
			if (column == 0)
			{
				return skeleton->m_floatSlots[edge.row()].cString();
			}
			return "InvalidColumn";
		}
		if (row == 0 && column == 0)
		{
			return "Skeleton";
		}
		return SupportEnhancedEdge::data(row, column, edge, manager);
	}
	return "No Skeleton";
}