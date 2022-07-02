#include <src/edges/RagdollModel.h>

using namespace ckcmd::HKX;

int RagdollModel::rows(const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (variant != nullptr)
	{
		auto* data = reinterpret_cast<hkaRagdollInstance*>(variant->m_object);
		if (data == NULL)
			return 0;
		if (edge.childType() == NodeType::RagdollHkxNode)
		{
			return DATA_SUPPORTS;
		}
		else if (edge.childType() == NodeType::RagdollBones)
		{
			if (NULL == data->m_skeleton)
				return 0;
			return data->m_skeleton->m_bones.getSize();
		}
	}
	return 0;
}
int RagdollModel::columns(int row, const ModelEdge& edge, ResourceManager& manager)
{
	return 1;
}
int RagdollModel::childCount(const ModelEdge& edge, ResourceManager& manager)
{
	return rows(edge, manager);
}
std::pair<int, int> RagdollModel::child(int index, const ModelEdge& edge, ResourceManager& manager)
{
	return { index, 0 };
}
int RagdollModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	return row;
}
ModelEdge RagdollModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		if (edge.childType() == NodeType::RagdollHkxNode)
		{
			switch (row) {
			case 0:
				return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), variant, NodeType::RagdollBones);
			default:
				break;
			}
			return ModelEdge();
		}
		else if (edge.childType() == NodeType::RagdollBones)
		{
			return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), variant, NodeType::RagdollBone);
		}
	}
	return ModelEdge();
}

QVariant RagdollModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (edge.childType() == NodeType::RagdollHkxNode)
	{
		return "Ragdoll";
	}
	else if (edge.childType() == NodeType::RagdollBones)
	{
		return DataListsName(edge.childType());
	}
	if (nullptr != variant)
	{
		auto* data = reinterpret_cast<hkaRagdollInstance*>(variant->m_object);
		if (data != nullptr && edge.childType() == NodeType::RagdollBone)
		{
			return data->m_skeleton->m_bones[edge.row()].m_name.cString();
		}

	}
	return 0;
}