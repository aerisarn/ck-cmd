#include <src/edges/BehaviorReferenceModel.h>

using namespace ckcmd::HKX;

int BehaviorReferenceModel::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	return 1;
}

std::pair<int, int> BehaviorReferenceModel::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	return { 4, 1 };
}

int BehaviorReferenceModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	if (row == 4 && column == 1)
		return 0;
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