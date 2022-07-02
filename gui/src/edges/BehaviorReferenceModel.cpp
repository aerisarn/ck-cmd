#include <src/edges/BehaviorReferenceModel.h>

using namespace ckcmd::HKX;

//	static int getChildCount(hkVariant* variant, NodeType childType)
//	{
//		return 1;
//	}

//	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
//	{
//		hkbBehaviorReferenceGenerator* bfg = reinterpret_cast<hkbBehaviorReferenceGenerator*>(variant->m_object);
//		return bfg->m_behaviorName.cString();
//	}

//	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
//	{
//		int behavior_file = manager.behaviorFileIndex(project, variant);
//		hkVariant* root = manager.behaviorFileRoot(behavior_file);
//		return ModelEdge(variant, project, behavior_file, index, 0, root, NodeType::BehaviorHkxNode);
//	}

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