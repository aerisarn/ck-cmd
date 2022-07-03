#include <src/models/ProjectTreeHkHandler.h>
#include <src/hkx/HkxLinkedTableVariant.h>

#include <src/edges/CharacterModel.h>
#include <src/edges/BehaviorModel.h>
#include <src/edges/SkeletonModel.h>
#include <src/edges/RagdollModel.h>
#include <src/edges/StateMachineModel.h>
#include <src/edges/BehaviorReferenceModel.h>

#include <Common/Serialize/Util/hkRootLevelContainer.h>

using namespace ckcmd::HKX;

int ProjectTreeHkHandler::rows(const ModelEdge& edge, ResourceManager& manager)
{
	NodeType type = edge.childType();
	switch (type)
	{
	case NodeType::CharacterHkxNode:
	case NodeType::deformableSkinNames:
	case NodeType::deformableSkinName:
	case NodeType::animationNames:
	case NodeType::animationName:
	case NodeType::characterPropertyNames:
	case NodeType::characterProperty:
		return CharacterModel().rows(edge, manager);
	case NodeType::BehaviorHkxNode:
	case NodeType::behaviorEventNames:
	case NodeType::behaviorEventName:
	case NodeType::behaviorVariableNames:
	case NodeType::behaviorVariable:
	case NodeType::behaviorCharacterPropertyNames:
	case NodeType::behaviorCharacterProperty:
		return BehaviorModel().rows(edge, manager);
	case NodeType::SkeletonHkxNode:
	case NodeType::SkeletonBones:
	case NodeType::SkeletonBone:
	case NodeType::SkeletonFloats:
	case NodeType::SkeletonFloat:
		return SkeletonModel().rows(edge, manager);
	case NodeType::RagdollHkxNode:
	case NodeType::RagdollBones:
	case NodeType::RagdollBone:
		return RagdollModel().rows(edge, manager);
	default:
		break;
	}
	hkVariant* variant = edge.childItem<hkVariant>();
	if (variant != nullptr)
	{
		if (&hkbStateMachineClass == variant->m_class)
		{
			return StateMachineModel().rows(edge, manager);
		}
		if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
		{
			return BehaviorReferenceModel().rows(edge, manager);
		}
		hkVariant* variant = edge.childItem<hkVariant>();
		if (nullptr != variant)
		{
			return HkxTableVariant(*variant).rows();
		}
	}
	return 0;


	//return HkxTableVariant(*variant).rows();
	//auto& links = v.links();
	//for (const auto& link : links)
	//{
	//	if (link._row == row && link._column == column - 1)
	//	{
	//		int file_index = manager.findIndex(edge.file(), link._ref);
	//		if (file_index == -1)
	//			__debugbreak();
	//		return HkxTableVariant(*manager.at(edge.file(), file_index)).rows();
	//	}
	//}
	//return 0;
}

int ProjectTreeHkHandler::columns(int row, const ModelEdge& edge, ResourceManager& manager)
{
	NodeType type = edge.childType();
	switch (type)
	{
	case NodeType::CharacterHkxNode:
	case NodeType::deformableSkinNames:
	case NodeType::deformableSkinName:
	case NodeType::animationNames:
	case NodeType::animationName:
	case NodeType::characterPropertyNames:
	case NodeType::characterProperty:
		return CharacterModel().columns(row, edge, manager);
	case NodeType::BehaviorHkxNode:
	case NodeType::behaviorEventNames:
	case NodeType::behaviorEventName:
	case NodeType::behaviorVariableNames:
	case NodeType::behaviorVariable:
	case NodeType::behaviorCharacterPropertyNames:
	case NodeType::behaviorCharacterProperty:
		return BehaviorModel().columns(row, edge, manager);
	case NodeType::SkeletonHkxNode:
	case NodeType::SkeletonBones:
	case NodeType::SkeletonBone:
	case NodeType::SkeletonFloats:
	case NodeType::SkeletonFloat:
		return SkeletonModel().columns(row, edge, manager);
	case NodeType::RagdollHkxNode:
	case NodeType::RagdollBones:
	case NodeType::RagdollBone:
		return RagdollModel().columns(row, edge, manager);
	default:
		break;
	}
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		if (&hkbStateMachineClass == variant->m_class)
		{
			return StateMachineModel().columns(row, edge, manager);
		}
		if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
		{
			return BehaviorReferenceModel().columns(row, edge, manager);
		}
		return HkxLinkedTableVariant(*variant).columns(row);
	}
	return 0;
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return BehaviorModel::childColumns(row, column, edge, manager);
	//}
	//if (&hkbStateMachineClass == variant->m_class)
	//{
	//	return StateMachineModel::childColumns(row, column, edge, manager);
	//}

}

int ProjectTreeHkHandler::childCount(const ModelEdge& edge, ResourceManager& manager)
{
	NodeType type = edge.childType();
	switch (type)
	{
	case NodeType::CharacterHkxNode:
	case NodeType::deformableSkinNames:
	case NodeType::deformableSkinName:
	case NodeType::animationNames:
	case NodeType::animationName:
	case NodeType::characterPropertyNames:
	case NodeType::characterProperty:
		return CharacterModel().childCount(edge, manager);
	case NodeType::BehaviorHkxNode:
	case NodeType::behaviorEventNames:
	case NodeType::behaviorEventName:
	case NodeType::behaviorVariableNames:
	case NodeType::behaviorVariable:
	case NodeType::behaviorCharacterPropertyNames:
	case NodeType::behaviorCharacterProperty:
		return BehaviorModel().childCount(edge, manager);
	case NodeType::SkeletonHkxNode:
	case NodeType::SkeletonBones:
	case NodeType::SkeletonBone:
	case NodeType::SkeletonFloats:
	case NodeType::SkeletonFloat:
		return SkeletonModel().childCount(edge, manager);
	case NodeType::RagdollHkxNode:
	case NodeType::RagdollBones:
	case NodeType::RagdollBone:
		return RagdollModel().childCount(edge, manager);
	default:
		break;
	}
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		if (&hkbStateMachineClass == variant->m_class)
		{
			return StateMachineModel().childCount(edge, manager);
		}
		if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
		{
			return BehaviorReferenceModel().childCount(edge, manager);
		}
		return HkxLinkedTableVariant(*variant).links().size();
	}
	return 0;
}

ModelEdge ProjectTreeHkHandler::child(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	NodeType type = edge.childType();
	switch (type)
	{
	case NodeType::CharacterHkxNode:
	case NodeType::deformableSkinNames:
	case NodeType::deformableSkinName:
	case NodeType::animationNames:
	case NodeType::animationName:
	case NodeType::characterPropertyNames:
	case NodeType::characterProperty:
		return CharacterModel().child(row, column, edge, manager);
	case NodeType::BehaviorHkxNode:
	case NodeType::behaviorEventNames:
	case NodeType::behaviorEventName:
	case NodeType::behaviorVariableNames:
	case NodeType::behaviorVariable:
	case NodeType::behaviorCharacterPropertyNames:
	case NodeType::behaviorCharacterProperty:
		return BehaviorModel().child(row, column, edge, manager);
	case NodeType::SkeletonHkxNode:
	case NodeType::SkeletonBones:
	case NodeType::SkeletonBone:
	case NodeType::SkeletonFloats:
	case NodeType::SkeletonFloat:
		return SkeletonModel().child(row, column, edge, manager);
	case NodeType::RagdollHkxNode:
	case NodeType::RagdollBones:
	case NodeType::RagdollBone:
		return RagdollModel().child(row, column, edge, manager);
	default:
		break;
	}
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		if (&hkbStateMachineClass == variant->m_class)
		{
			return StateMachineModel().child(row, column, edge, manager);
		}
		if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
		{
			return BehaviorReferenceModel().child(row, column, edge, manager);
		}
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

	}
	__debugbreak();
	return ModelEdge();

	//if (&hkbCharacterDataClass == variant->m_class)
	//{
	//	return CharacterModel::child(row, column, edge, manager);
	//}
	////if (&hkbBehaviorGraphClass == variant->m_class)
	////{
	////	return BehaviorModel::child(row, column, edge, manager);
	////}
	////if (&hkbStateMachineClass == variant->m_class)
	////{
	////	return StateMachineModel::child(row, column, edge, manager);
	////}
	//if (&hkaSkeletonClass == variant->m_class)
	//{
	//	return SkeletonModel::child(row, column, edge, manager);
	//}
	//if (&hkaRagdollInstanceClass == variant->m_class)
	//{
	//	return RagdollModel::child(row, column, edge, manager);
	//}

	//HkxLinkedTableVariant v(*variant);
	//auto& links = v.links();
	//for (const auto& link : links)
	//{
	//	if (link._row == row && link._column == column)
	//	{
	//		int file_index = manager.findIndex(edge.file(), link._ref);
	//		if (file_index == -1)
	//			__debugbreak();
	//		return ModelEdge(edge, edge.project(), edge.file(), link._row, link._column, edge.subindex(), manager.at(edge.file(), file_index), NodeType::HavokNative);
	//	}
	//}
	//__debugbreak();
	//return ModelEdge();
}

std::pair<int, int> ProjectTreeHkHandler::child(int index, const ModelEdge& edge, ResourceManager& manager)
{
	NodeType type = edge.childType();
	switch (type)
	{
	case NodeType::CharacterHkxNode:
	case NodeType::deformableSkinNames:
	case NodeType::deformableSkinName:
	case NodeType::animationNames:
	case NodeType::animationName:
	case NodeType::characterPropertyNames:
	case NodeType::characterProperty:
		return CharacterModel().child(index, edge, manager);
	case NodeType::BehaviorHkxNode:
	case NodeType::behaviorEventNames:
	case NodeType::behaviorEventName:
	case NodeType::behaviorVariableNames:
	case NodeType::behaviorVariable:
	case NodeType::behaviorCharacterPropertyNames:
	case NodeType::behaviorCharacterProperty:
		return BehaviorModel().child(index, edge, manager);
	case NodeType::SkeletonHkxNode:
	case NodeType::SkeletonBones:
	case NodeType::SkeletonBone:
	case NodeType::SkeletonFloats:
	case NodeType::SkeletonFloat:
		return SkeletonModel().child(index, edge, manager);
	case NodeType::RagdollHkxNode:
	case NodeType::RagdollBones:
	case NodeType::RagdollBone:
		return RagdollModel().child(index, edge, manager);
	default:
		break;
	}
	hkVariant* variant = edge.childItem<hkVariant>();
	if (variant != nullptr)
	{
		if (&hkbStateMachineClass == variant->m_class)
		{
			return StateMachineModel().child(index, edge, manager);
		}
		if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
		{
			return BehaviorReferenceModel().child(index, edge, manager);
		}
		HkxLinkedTableVariant v(*variant);
		auto& links = v.links();
		if (index < links.size())
			return  { links[index]._row, links[index]._column };
	}
	return  { -1, -1 };
}

int ProjectTreeHkHandler::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	NodeType type = edge.childType();
	switch (type)
	{
	case NodeType::CharacterHkxNode:
	case NodeType::deformableSkinNames:
	case NodeType::deformableSkinName:
	case NodeType::animationNames:
	case NodeType::animationName:
	case NodeType::characterPropertyNames:
	case NodeType::characterProperty:
		return CharacterModel().childIndex(row, column, edge, manager);
	case NodeType::BehaviorHkxNode:
	case NodeType::behaviorEventNames:
	case NodeType::behaviorEventName:
	case NodeType::behaviorVariableNames:
	case NodeType::behaviorVariable:
	case NodeType::behaviorCharacterPropertyNames:
	case NodeType::behaviorCharacterProperty:
		return BehaviorModel().childIndex(row, column, edge, manager);
	case NodeType::SkeletonHkxNode:
	case NodeType::SkeletonBones:
	case NodeType::SkeletonBone:
	case NodeType::SkeletonFloats:
	case NodeType::SkeletonFloat:
		return SkeletonModel().childIndex(row, column, edge, manager);
	case NodeType::RagdollHkxNode:
	case NodeType::RagdollBones:
	case NodeType::RagdollBone:
		return RagdollModel().childIndex(row, column, edge, manager);
	default:
		break;
	}
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		if (&hkbStateMachineClass == variant->m_class)
		{
			return StateMachineModel().childIndex(row, column, edge, manager);
		}
		if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
		{
			return BehaviorReferenceModel().childIndex(row, column, edge, manager);
		}
		HkxLinkedTableVariant v(*variant);
		auto& links = v.links();
		int index = 0;
		for (const auto& link : links)
		{
			if (link._row == row && link._column == column)
				return index;
			index++;
		}
	}
	return MODELEDGE_INVALID;
}

QVariant ProjectTreeHkHandler::data(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	NodeType type = edge.childType();
	switch (type)
	{
	case NodeType::CharacterHkxNode:
	case NodeType::deformableSkinNames:
	case NodeType::deformableSkinName:
	case NodeType::animationNames:
	case NodeType::animationName:
	case NodeType::characterPropertyNames:
	case NodeType::characterProperty:
		return CharacterModel().data(row, column, edge, manager);
	case NodeType::BehaviorHkxNode:
	case NodeType::behaviorEventNames:
	case NodeType::behaviorEventName:
	case NodeType::behaviorVariableNames:
	case NodeType::behaviorVariable:
	case NodeType::behaviorCharacterPropertyNames:
	case NodeType::behaviorCharacterProperty:
		return BehaviorModel().data(row, column, edge, manager);
	case NodeType::SkeletonHkxNode:
	case NodeType::SkeletonBones:
	case NodeType::SkeletonBone:
	case NodeType::SkeletonFloats:
	case NodeType::SkeletonFloat:
		return SkeletonModel().data(row, column, edge, manager);
	case NodeType::RagdollHkxNode:
	case NodeType::RagdollBones:
	case NodeType::RagdollBone:
		return RagdollModel().data(row, column, edge, manager);
	default:
		break;
	}
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		if (&hkbStateMachineClass == variant->m_class)
		{
			return StateMachineModel().data(row, column, edge, manager);
		}
		if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
		{
			return BehaviorReferenceModel().data(row, column, edge, manager);
		}
		HkxLinkedTableVariant v(*variant);
		if (row == 0 && column == 0)
			return v.name();
		return v.data(row - 1, column - 1);
	}
	return QVariant("NULL");
}

bool ProjectTreeHkHandler::setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return CharacterModel().setData(row, column, edge, data, manager);
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
		return CharacterModel().addRows(row_start, count, edge, manager);
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
		return CharacterModel().removeRows(row_start, count, edge, manager);
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
		return CharacterModel().changeColumns(row, column_start, delta, edge, manager);
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