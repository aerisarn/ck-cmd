#include <src/edges/SkeletonModel.h>

using namespace ckcmd::HKX;

int SkeletonModel::rows(const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (variant != nullptr)
	{
		auto* data = reinterpret_cast<hkaSkeleton*>(variant->m_object);
		if (data == NULL)
			return 0;
		if (edge.childType() == NodeType::SkeletonHkxNode)
		{
			return DATA_SUPPORTS;
		}
		else if (edge.childType() == NodeType::SkeletonBones)
		{
			return data->m_bones.getSize();
		}
		else if (edge.childType() == NodeType::SkeletonFloats)
		{
			return data->m_floatSlots.getSize();
		}
	}
	return 0;
}

int SkeletonModel::columns(int row, const ModelEdge& edge, ResourceManager& manager)
{
	return 1;
}

int SkeletonModel::childCount(const ModelEdge& edge, ResourceManager& manager)
{
	return rows(edge, manager);
}

std::pair<int, int> SkeletonModel::child(int index, const ModelEdge& edge, ResourceManager& manager)
{
	return { index, 0 };
}

int SkeletonModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	return row;
}

ModelEdge SkeletonModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (edge.childType() == NodeType::SkeletonHkxNode)
	{
		switch (row) {
		case 0:
			return ModelEdge(edge, edge.project(), edge.file(), row, 0, -1, variant, NodeType::SkeletonBones);
		case 1:
			return ModelEdge(edge, edge.project(), edge.file(), row, 0, -1, variant, NodeType::SkeletonFloats);
		default:
			break;
		}
		return ModelEdge();
	}
	else if (edge.childType() == NodeType::SkeletonBones)
	{
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, -1, variant, NodeType::SkeletonBone);
	}
	else if (edge.childType() == NodeType::SkeletonFloats)
	{
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, -1, variant, NodeType::SkeletonFloat);
	}
	return ModelEdge();
}

QVariant SkeletonModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	if (edge.childType() == NodeType::SkeletonBones ||
		edge.childType() == NodeType::SkeletonFloats)
	{
		return DataListsName(edge.childType());
	}
	else if (edge.childType() == NodeType::SkeletonHkxNode) {
		return "Skeleton";
	}
	else {
		hkVariant* variant = edge.childItem<hkVariant>();
		if (nullptr != variant)
		{
			auto* data = reinterpret_cast<hkaSkeleton*>(variant->m_object);
			if (edge.childType() == NodeType::SkeletonBone)
			{
				return data->m_bones[edge.row()].m_name.cString();
			}
			else if (edge.childType() == NodeType::SkeletonFloat)
			{
				return data->m_floatSlots[edge.row()].cString();
			}
		}
	}
	return QVariant();
}