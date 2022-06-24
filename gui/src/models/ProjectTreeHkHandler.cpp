#include <src/models/ProjectTreeHkHandler.h>
#include <src/hkx/HkxLinkedTableVariant.h>

#include <src/edges/CharacterModel.h>
//#include <src/edges/BehaviorModel.h>
//#include <src/edges/SkeletonModel.h>
//#include <src/edges/RagdollModel.h>
//#include <src/edges/StateMachineModel.h>
//#include <src/edges/BehaviorReferenceModel.h>

#include <Common/Serialize/Util/hkRootLevelContainer.h>

using namespace ckcmd::HKX;

int ProjectTreeHkHandler::childRows(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel::childRows(row, column, edge, manager);
	}
/*	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return BehaviorModel::childRows(row, column, edge, manager);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return StateMachineModel::childRows(row, column, edge, manager);
	}
	if (&hkaSkeletonClass == variant->m_class)
	{
		return SkeletonModel::childRows(row, column, edge, manager);
	}
	if (&hkaRagdollInstanceClass == variant->m_class)
	{
		return RagdollModel::childRows(row, column, edge, manager);
	}
	if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	{
		return BehaviorReferenceModel::childRows(row, column, edge, manager);
	}*/

	HkxLinkedTableVariant v(*variant);
	auto& links = v.links();
	for (const auto& link : links)
	{
		if (link._row == row && link._column == column - 1)
		{
			int file_index = manager.findIndex(edge.file(), link._ref);
			if (file_index == -1)
				__debugbreak();
			return HkxTableVariant(*manager.at(edge.file(), file_index)).rows();
		}
	}
	return 0;
}

int ProjectTreeHkHandler::childColumns(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel::childColumns(row, column, edge, manager);
	}
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return BehaviorModel::childColumns(row, column, edge, manager);
	//}
	//if (&hkbStateMachineClass == variant->m_class)
	//{
	//	return StateMachineModel::childColumns(row, column, edge, manager);
	//}
	//if (&hkaSkeletonClass == variant->m_class)
	//{
	//	return SkeletonModel::childColumns(row, column, edge, manager);
	//}
	//if (&hkaRagdollInstanceClass == variant->m_class)
	//{
	//	return RagdollModel::childColumns(row, column, edge, manager);
	//}
	//if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	//{
	//	return BehaviorReferenceModel::childColumns(row, column, edge, manager);
	//}
	HkxLinkedTableVariant v(*variant);
	auto& links = v.links();
	for (const auto& link : links)
	{
		if (link._row == row && link._column == column)
		{
			int file_index = manager.findIndex(edge.file(), link._ref);
			if (file_index == -1)
				__debugbreak();
			return HkxTableVariant(*manager.at(edge.file(), file_index)).columns();
		}
	}
	return 0;
}

int ProjectTreeHkHandler::childCount(const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel::childCount(edge, manager);
	}
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return BehaviorModel::childCount(edge, manager);
	//}
	//if (&hkbStateMachineClass == variant->m_class)
	//{
	//	return StateMachineModel::childCount(edge, manager);
	//}
	//if (&hkaSkeletonClass == variant->m_class)
	//{
	//	return SkeletonModel::childCount(edge, manager);
	//}
	//if (&hkaRagdollInstanceClass == variant->m_class)
	//{
	//	return RagdollModel::childCount(edge, manager);
	//}
	//if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	//{
	//	return BehaviorReferenceModel::childCount(edge, manager);
	//}
	return HkxLinkedTableVariant(*variant).links().size();
}

bool ProjectTreeHkHandler::hasChild(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel::hasChild(row, column, edge, manager);
	}
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return BehaviorModel::hasChild(row, column, edge, manager);
	//}
	//if (&hkbStateMachineClass == variant->m_class)
	//{
	//	return StateMachineModel::hasChild(row, column, edge, manager);
	//}
	//if (&hkaSkeletonClass == variant->m_class)
	//{
	//	return SkeletonModel::hasChild(row, column, edge, manager);
	//}
	//if (&hkaRagdollInstanceClass == variant->m_class)
	//{
	//	return RagdollModel::hasChild(row, column, edge, manager);
	//}
	//if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	//{
	//	return BehaviorReferenceModel::hasChild(row, column, edge, manager);
	//}
	HkxLinkedTableVariant v(*variant);
	auto& links = v.links();
	for (const auto& link : links)
	{
		if (link._row == row && link._column == column)
		{
			return true;
		}
	}
	return false;
}

ModelEdge ProjectTreeHkHandler::child(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel::child(row, column, edge, manager);
	}
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return BehaviorModel::child(row, column, edge, manager);
	//}
	//if (&hkbStateMachineClass == variant->m_class)
	//{
	//	return StateMachineModel::child(row, column, edge, manager);
	//}
	//if (&hkaSkeletonClass == variant->m_class)
	//{
	//	return SkeletonModel::child(row, column, edge, manager);
	//}
	//if (&hkaRagdollInstanceClass == variant->m_class)
	//{
	//	return RagdollModel::child(row, column, edge, manager);
	//}
	//if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	//{
	//	return BehaviorReferenceModel::child(row, column, edge, manager);
	//}
	HkxLinkedTableVariant v(*variant);
	auto& links = v.links();
	for (const auto& link : links)
	{
		if (link._row == row && link._column == column)
		{
			int file_index = manager.findIndex(edge.file(), link._ref);
			if (file_index == -1)
				__debugbreak();
			return ModelEdge(edge, edge.project(), edge.file(), link._row, link._column, edge.subindex(), manager.at(edge.file(), file_index), NodeType::HavokNative);
		}
	}
	__debugbreak();
	return ModelEdge();
}

QVariant ProjectTreeHkHandler::data(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel::data(row, column, edge, manager);
	}
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return BehaviorModel::data(row, column, edge, manager);
	//}
	//if (&hkbStateMachineClass == variant->m_class)
	//{
	//	return StateMachineModel::data(row, column, edge, manager);
	//}
	//if (&hkaSkeletonClass == variant->m_class)
	//{
	//	return SkeletonModel::data(row, column, edge, manager);
	//}
	//if (&hkaRagdollInstanceClass == variant->m_class)
	//{
	//	return RagdollModel::data(row, column, edge, manager);
	//}
	//if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	//{
	//	return BehaviorReferenceModel::data(row, column, edge, manager);
	//}
	HkxLinkedTableVariant v(*variant);
	return v.data(row, column - 1);
}

bool ProjectTreeHkHandler::setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel::setData(row, column, edge, data, manager);
	}
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return BehaviorModel::setData(row, column, edge, data, manager);
	//}
	//if (&hkbStateMachineClass == variant->m_class)
	//{
	//	return StateMachineModel::setData(row, column, edge, data, manager);
	//}
	//if (&hkaSkeletonClass == variant->m_class)
	//{
	//	return SkeletonModel::setData(row, column, edge, data, manager);
	//}
	//if (&hkaRagdollInstanceClass == variant->m_class)
	//{
	//	return RagdollModel::setData(row, column, edge, data, manager);
	//}
	//if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	//{
	//	return BehaviorReferenceModel::setData(row, column, edge, data, manager);
	//}
	return false;
}

bool ProjectTreeHkHandler::addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel::addRows(row_start, count, edge, manager);
	}
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return BehaviorModel::addRows(row_start, count, edge, manager);
	//}
	//if (&hkbStateMachineClass == variant->m_class)
	//{
	//	return StateMachineModel::addRows(row_start, count, edge, manager);
	//}
	//if (&hkaSkeletonClass == variant->m_class)
	//{
	//	return SkeletonModel::addRows(row_start, count, edge, manager);
	//}
	//if (&hkaRagdollInstanceClass == variant->m_class)
	//{
	//	return RagdollModel::addRows(row_start, count, edge, manager);
	//}
	//if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	//{
	//	return BehaviorReferenceModel::addRows(row_start, count, edge, manager);
	//}
	return false;
}

bool ProjectTreeHkHandler::removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel::removeRows(row_start, count, edge, manager);
	}
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return BehaviorModel::removeRows(row_start, count, edge, manager);
	//}
	//if (&hkbStateMachineClass == variant->m_class)
	//{
	//	return StateMachineModel::removeRows(row_start, count, edge, manager);
	//}
	//if (&hkaSkeletonClass == variant->m_class)
	//{
	//	return SkeletonModel::removeRows(row_start, count, edge, manager);
	//}
	//if (&hkaRagdollInstanceClass == variant->m_class)
	//{
	//	return RagdollModel::removeRows(row_start, count, edge, manager);
	//}
	//if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	//{
	//	return BehaviorReferenceModel::removeRows(row_start, count, edge, manager);
	//}
	return false;
}
bool ProjectTreeHkHandler::changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel::changeColumns(row, column_start, delta, edge, manager);
	}
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return BehaviorModel::changeColumns(row, column_start, delta, edge, manager);
	//}
	//if (&hkbStateMachineClass == variant->m_class)
	//{
	//	return StateMachineModel::changeColumns(row, column_start, delta, edge, manager);
	//}
	//if (&hkaSkeletonClass == variant->m_class)
	//{
	//	return SkeletonModel::changeColumns(row, column_start, delta, edge, manager);
	//}
	//if (&hkaRagdollInstanceClass == variant->m_class)
	//{
	//	return RagdollModel::changeColumns(row, column_start, delta, edge, manager);
	//}
	//if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	//{
	//	return BehaviorReferenceModel::changeColumns(row, column_start, delta, edge, manager);
	//}
	return HkxTableVariant(*variant).resizeColumns(row, column_start, delta);
}