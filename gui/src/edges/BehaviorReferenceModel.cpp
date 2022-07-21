#include <src/edges/BehaviorReferenceModel.h>

using namespace ckcmd::HKX;

std::vector<const hkClass*> BehaviorReferenceModel::handled_hkclasses() const
{
	return { &hkbBehaviorReferenceGeneratorClass };
}

hkbBehaviorReferenceGenerator* BehaviorReferenceModel::variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* bfr = reinterpret_cast<hkbBehaviorReferenceGenerator*>(variant->m_object);
		return bfr;
	}
	return nullptr;
}

int BehaviorReferenceModel::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	auto bfr = variant(edge);
	if (bfr != NULL)
	{
		if (bfr->m_behaviorName != NULL)
		{
			if (MODELEDGE_INVALID != manager.behaviorFileIndex(edge.project(), edge.childItem<hkVariant>()))
				return 1;
		}
	}
	return 0;
}

std::pair<int, int> BehaviorReferenceModel::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	return { 4, 1 };
}

int BehaviorReferenceModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	if (row == 4 && column == 1)
	{
		if (childCount(edge, manager) > 0)
			return 0;
	}
	return MODELEDGE_INVALID;
}

ModelEdge BehaviorReferenceModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	if (row == 4 && column == 1)
	{
		int behavior_file = manager.behaviorFileIndex(edge.project(), edge.childItem<hkVariant>());
		hkVariant* root = manager.behaviorFileRoot(behavior_file);
		return ModelEdge(edge, edge.project(), behavior_file, row, column, edge.subindex(), root, NodeType::BehaviorHkxNode);
	}
	return ModelEdge();
}