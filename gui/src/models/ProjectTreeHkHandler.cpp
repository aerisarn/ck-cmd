#include <src/models/ProjectTreeHkHandler.h>
#include <src/hkx/HkxLinkedTableVariant.h>

#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <hkbCharacterData_7.h>
#include <hkbBehaviorGraph_1.h>
#include <hkbBehaviorGraphData_2.h>
#include <hkbBehaviorGraphStringData_1.h>
#include <hkbStateMachine_4.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbStateMachineTransitionInfoArray_0.h>
#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
#include <hkbBehaviorReferenceGenerator_0.h>
#include <hkbNode_1.h>


using namespace ckcmd::HKX;

struct  HandleCharacterData
{
	static const size_t DATA_SUPPORTS = 4;
	static const size_t RIG_INDEX = DATA_SUPPORTS;
	static const size_t RAGDOLL_INDEX = DATA_SUPPORTS + 1;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::deformableSkinNames:
			return "Deformable Skins";
		case NodeType::animationNames:
			return "Animations";
		case NodeType::characterPropertyNames:
			return "Character Properties";
		default:
			break;
		}
		return "Invalid Character Entry";
	};

	static int getChildCount(int project, hkVariant* variant, NodeType childType, ResourceManager& manager)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		if (data == NULL)
			return 0;
		auto string_data = data->m_stringData;
		if (string_data == NULL)
			return 0;
		if (childType == NodeType::CharacterHkxNode)
		{
			int count = DATA_SUPPORTS;
			count += manager.hasRigAndRagdoll(project, string_data);
			return count;
		}
		else if (childType == NodeType::deformableSkinNames)
		{
			return string_data->m_deformableSkinNames.getSize();
		}
		else if (childType == NodeType::animationNames)
		{
			return string_data->m_animationNames.getSize();
		}
		else if (childType == NodeType::characterPropertyNames)
		{
			return string_data->m_characterPropertyNames.getSize();
		}
		return 0;
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		auto string_data = data->m_stringData;
		if (childType == NodeType::CharacterHkxNode) {
			return string_data->m_name.cString();
		}
		if (childType == NodeType::deformableSkinNames ||
			childType == NodeType::animationNames ||
			childType == NodeType::characterPropertyNames)
		{
			return DataListsName((int)childType);
		}
		if (childType == NodeType::deformableSkinName)
		{
			return string_data->m_deformableSkinNames[row].cString();
		}
		if (childType == NodeType::animationName)
		{
			return string_data->m_animationNames[row].cString();
		}
		if (childType == NodeType::characterPropertyName)
		{
			return string_data->m_characterPropertyNames[row].cString();
		}
		return QVariant();
	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		if (childType == NodeType::CharacterHkxNode)
		{
			switch (index) {
			case 0:
			{
				int file = manager.behaviorFileIndex(project, variant);
				hkVariant* root = manager.behaviorFileRoot(file);
				return ModelEdge(variant, project, file, index, 0, root, NodeType::BehaviorHkxNode);
			}
			case 1:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::deformableSkinNames);
			case 2:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::animationNames);
			case 3:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::characterPropertyNames);
			case RIG_INDEX:
			{
				auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
				auto string_data = data->m_stringData;
				int rig_index = manager.getRigIndex(project, string_data);
				hkVariant* rig_root = manager.getRigRoot(project, rig_index);
				return ModelEdge(variant, project, rig_index, 0, 0, rig_root);
			}
			case RAGDOLL_INDEX:
			{
				auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
				auto string_data = data->m_stringData;
				int ragdoll_index = -1;
				if (string_data->m_ragdollName.getLength() > 0)
					ragdoll_index = manager.getRagdollIndex(project, string_data->m_ragdollName.cString());
				else
					ragdoll_index = manager.getRagdollIndex(project, string_data->m_rigName.cString());
				hkVariant* ragdoll_root = manager.getRagdollRoot(project, ragdoll_index);
				return ModelEdge(variant, project, ragdoll_index, 0, 0, ragdoll_root);
			}
			default:
				break;
			}
			return ModelEdge();
		}
		else if (childType == NodeType::deformableSkinNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::deformableSkinName);
		}
		else if (childType == NodeType::animationNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::animationName);
		}
		else if (childType == NodeType::characterPropertyNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::characterPropertyName);
		}
		else {
			return ModelEdge();
		}
	}
};

struct  HandleBehaviorData
{

	static const size_t DATA_SUPPORTS = 3;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::behaviorEventNames:
			return "Events";
		case NodeType::behaviorVariableNames:
			return "Variables";
		case NodeType::behaviorCharacterPropertyNames:
			return "Properties";
		default:
			break;
		}
		return "Invalid Behavior Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		if (graph == NULL)
			return 0;
		auto data = graph->m_data;
		if (data == NULL)
			return 0;
		auto string_data = data->m_stringData;
		if (string_data == NULL)
			return 0;
		if (childType == NodeType::BehaviorHkxNode)
		{
			return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).links().size();
		}
		else if (childType == NodeType::behaviorEventNames)
		{
			return string_data->m_eventNames.getSize();
		}
		else if (childType == NodeType::behaviorVariableNames)
		{
			return string_data->m_variableNames.getSize();
		}
		else if (childType == NodeType::behaviorCharacterPropertyNames)
		{
			return string_data->m_characterPropertyNames.getSize();
		}
		else if (childType == NodeType::behaviorEventName)
		{
			return 0;
		}
		else if (childType == NodeType::behaviorVariableName)
		{
			return 0;
		}
		else if (childType == NodeType::behaviorCharacterPropertyName)
		{
			return 0;
		}
		return HkxLinkedTableVariant(*variant).links().size();
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		auto string_data = data->m_data->m_stringData;
		if (childType == NodeType::behaviorEventNames ||
			childType == NodeType::behaviorVariableNames ||
			childType == NodeType::behaviorCharacterPropertyNames)
		{
			return DataListsName((int)childType);
		}
		else if (childType == NodeType::behaviorEventName)
		{
			return string_data->m_eventNames[row].cString();
		}
		else if (childType == NodeType::behaviorVariableName)
		{
			return string_data->m_variableNames[row].cString();
		}
		else if (childType == NodeType::behaviorCharacterPropertyName)
		{
			return string_data->m_characterPropertyNames[row].cString();
		}
		else {
			if (column == 0)
			{
				return HkxVariant(*variant).name();
			}
			HkxLinkedTableVariant v(*variant);
			return v.data(row - DATA_SUPPORTS, column - 1);
		}
	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		if (childType == NodeType::BehaviorHkxNode)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorEventNames);
			case 1:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorVariableNames);
			case 2:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorCharacterPropertyNames);
			default:
				break;
			}
		}
		else if (childType == NodeType::behaviorEventNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorEventName);
		}
		else if (childType == NodeType::behaviorVariableNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorVariableName);
		}
		else if (childType == NodeType::behaviorCharacterPropertyNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorCharacterPropertyName);
		}
		HkxLinkedTableVariant v(*variant);
		auto& links = v.links();
		auto& link = links.at(index - DATA_SUPPORTS);
		int file_index = manager.findIndex(file, link._ref);
		if (file_index == -1)
			__debugbreak();
		return ModelEdge((hkVariant*)nullptr, project, file, link._row, link._column, manager.at(file, file_index));
	}
};

struct  HandleStateMachineData
{

	static const size_t DATA_SUPPORTS = 2;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::FSMWildcardTransitions:
			return "Wildcard Transitions";
		case NodeType::FSMStateTransitions:
			return "Transitions";
		default:
			break;
		}
		return "Invalid FSM Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
		if (FSM == NULL)
			return 0;

		if (childType == NodeType::HavokNative)
		{
			return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).links().size();
		}
		else if (childType == NodeType::FSMWildcardTransitions)
		{
			if (FSM->m_wildcardTransitions == NULL)
				return 0;
			return FSM->m_wildcardTransitions->m_transitions.getSize();
		}
		else if (childType == NodeType::FSMWildcardTransition)
		{
			return 0;
		}
		else if (childType == NodeType::FSMStateTransitions)
		{
			int count = 0;
			for (int i = 0; i < FSM->m_states.getSize(); i++)
			{
				if (NULL != FSM->m_states[i]->m_transitions)
					count += FSM->m_states[i]->m_transitions->m_transitions.getSize();
			}
			return count;
		}
		else if (childType == NodeType::FSMStateTransition)
		{
			return 0;
		}
		
		return HkxLinkedTableVariant(*variant).links().size();
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
		if (FSM == NULL)
			return 0;

		if (childType == NodeType::FSMWildcardTransitions || childType == NodeType::FSMStateTransitions)
		{
			return DataListsName((int)childType);
		}
		else if (childType == NodeType::FSMWildcardTransition)
		{
			auto& transition = FSM->m_wildcardTransitions->m_transitions[row];
			QString result = "Transition to Invalid State";
			for (const auto& state : FSM->m_states)
			{
				if (state->m_stateId == transition.m_toStateId)
				{
					result = QString("Transition to ") + state->m_name.cString();
				}
			}
			return result;
		}
		else if (childType == NodeType::FSMStateTransition)
		{
			int index = row;
			QString result = "Invalid state transition";
			for (const auto& state : FSM->m_states)
			{
				if (NULL != state->m_transitions)
				{
					if (index < state->m_transitions->m_transitions.getSize())
					{
						auto& transition = state->m_transitions->m_transitions[index];
						for (const auto& to_state : FSM->m_states)
						{
							if (to_state->m_stateId == transition.m_toStateId)
							{
								result = QString("Transition from %1 to %2 ").arg(state->m_name.cString()).arg(to_state->m_name.cString());
								break;
							}
						}				
						break;
					}
					index -= state->m_transitions->m_transitions.getSize();
				}
			}
			return result;
		}
		if (column == 0)
		{
			return HkxVariant(*variant).name();
		}
		HkxLinkedTableVariant v(*variant);
		if (FSM->m_wildcardTransitions != NULL)
			return v.data(row-DATA_SUPPORTS, column - 1);
		return v.data(row, column - 1);
	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		if (childType == NodeType::HavokNative)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::FSMWildcardTransitions);
			case 1:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::FSMStateTransitions);
			default:
				break;
			}
		}
		else if (childType == NodeType::FSMWildcardTransitions)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::FSMWildcardTransition);
		}
		else if (childType == NodeType::FSMStateTransitions)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::FSMStateTransition);
		}
		HkxLinkedTableVariant v(*variant);
		auto& links = v.links();
		auto& link = links.at(index - DATA_SUPPORTS);
		int file_index = manager.findIndex(file, link._ref);
		if (file_index == -1)
			__debugbreak();
		return ModelEdge((hkVariant*)nullptr, project, file, link._row, link._column, manager.at(file, file_index));
	}
};

struct  HandleSkeletonData
{
	static const size_t DATA_SUPPORTS = 2;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::SkeletonBones:
			return "Bones";
		case NodeType::SkeletonFloats:
			return "Floats";
		default:
			break;
		}
		return "Invalid Character Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkaSkeleton*>(variant->m_object);
		if (data == NULL)
			return 0;
		if (childType == NodeType::HavokNative)
		{
			return DATA_SUPPORTS;
		}
		else if (childType == NodeType::SkeletonBones)
		{
			return data->m_bones.getSize();
		}
		else if (childType == NodeType::SkeletonFloats)
		{
			return data->m_floatSlots.getSize();
		}
		else {
			return 0;
		}
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkaSkeleton*>(variant->m_object);
		if (childType == NodeType::SkeletonBones ||
			childType == NodeType::SkeletonFloats)
		{
			return DataListsName((int)childType);
		}
		else if (childType == NodeType::SkeletonBone)
		{
			return data->m_bones[row].m_name.cString();
		}
		else if (childType == NodeType::SkeletonFloat)
		{
			return data->m_floatSlots[row].cString();
		}
		else if (childType == NodeType::HavokNative) {
			return "Skeleton";
		}
		return 0;

	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		if (childType == NodeType::HavokNative)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonBones);
			case 1:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonFloats);
			default:
				break;
			}
			return ModelEdge();
		}
		else if (childType == NodeType::SkeletonBones)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonBone);
		}
		else if (childType == NodeType::SkeletonFloats)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonFloat);
		}
		else {
			return ModelEdge();
		}
	}
};

struct  HandleRagdollData
{
	static const size_t DATA_SUPPORTS = 1;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::RagdollBones:
			return "Bones";
		default:
			break;
		}
		return "Invalid Character Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkaRagdollInstance*>(variant->m_object);
		if (data == NULL)
			return 0;
		if (childType == NodeType::HavokNative)
		{
			return DATA_SUPPORTS;
		}
		else if (childType == NodeType::RagdollBones)
		{
			if (NULL == data->m_skeleton)
				return 0;
			return data->m_skeleton->m_bones.getSize();
		}
		else {
			return 0;
		}
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkaRagdollInstance*>(variant->m_object);
		if (childType == NodeType::RagdollBones)
		{
			return DataListsName((int)childType);
		}
		else if (childType == NodeType::RagdollBone)
		{
			return data->m_skeleton->m_bones[row].m_name.cString();
		}
		else if (childType == NodeType::HavokNative)
		{
			return "Ragdoll";
		}
		else {
			return 0;
		}
	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		if (childType == NodeType::HavokNative)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::RagdollBones);
			default:
				break;
			}
			return ModelEdge();
		}
		else if (childType == NodeType::RagdollBones)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::RagdollBone);
		}
		else {
			return ModelEdge();
		}
	}
};

struct  HandleBehaviorReference {

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		return 1;
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		hkbBehaviorReferenceGenerator* bfg = reinterpret_cast<hkbBehaviorReferenceGenerator*>(variant->m_object);
		return bfg->m_behaviorName.cString();
	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		int behavior_file = manager.behaviorFileIndex(project, variant);
		hkVariant* root = manager.behaviorFileRoot(behavior_file);
		return ModelEdge(variant, project, behavior_file, index, 0, root, NodeType::BehaviorHkxNode);
	}

};

int ProjectTreeHkHandler::getChildCount(int project, hkVariant* variant, NodeType childType, ResourceManager& manager)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::getChildCount(project, variant, childType, manager);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::getChildCount(variant, childType);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return HandleStateMachineData::getChildCount(variant, childType);
	}
	if (&hkaSkeletonClass == variant->m_class)
	{
		return HandleSkeletonData::getChildCount(variant, childType);
	}
	if (&hkaRagdollInstanceClass == variant->m_class)
	{
		return HandleRagdollData::getChildCount(variant, childType);
	}
	if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	{
		return HandleBehaviorReference::getChildCount(variant, childType);
	}
	return HkxLinkedTableVariant(*variant).links().size();
}

QVariant ProjectTreeHkHandler::data(int row, int column, hkVariant* variant, NodeType childType)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::data(row, column, variant, childType);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::data(row, column, variant, childType);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return HandleStateMachineData::data(row, column, variant, childType);
	}
	if (&hkaSkeletonClass == variant->m_class)
	{
		return HandleSkeletonData::data(row, column, variant, childType);
	}
	if (&hkaRagdollInstanceClass == variant->m_class)
	{
		return HandleRagdollData::data(row, column, variant, childType);
	}
	if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	{
		return HandleBehaviorReference::data(row, column, variant, childType);
	}
	if (column == 0)
	{
		return HkxVariant(*variant).name();
	}
	HkxLinkedTableVariant v(*variant);
	return v.data(row, column - 1);
}

ModelEdge ProjectTreeHkHandler::getChild(hkVariant*, int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::get_child(index, project, file, variant, manager, childType);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::get_child(index, project, file, variant, manager, childType);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return HandleStateMachineData::get_child(index, project, file, variant, manager, childType);
	}
	if (&hkaSkeletonClass == variant->m_class)
	{
		return HandleSkeletonData::get_child(index, project, file, variant, manager, childType);
	}
	if (&hkaRagdollInstanceClass == variant->m_class)
	{
		return HandleRagdollData::get_child(index, project, file, variant, manager, childType);
	}
	if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	{
		return HandleBehaviorReference::get_child(index, project, file, variant, manager, childType);
	}
	HkxLinkedTableVariant v(*variant);
	auto& links = v.links();
	auto& link = links.at(index);
	int file_index = manager.findIndex(file, link._ref);
	if (file_index == -1)
		__debugbreak();
	return ModelEdge((hkVariant*)nullptr, project, file, link._row, link._column, manager.at(file, file_index));
}

bool ProjectTreeHkHandler::setData(int project, int row, int column, hkVariant* variant, NodeType childType, const QVariant& value, ResourceManager& manager)
{
	if (column == 0)
	{
		//setting name
		return HkxVariant(*variant).setName(value.toString());
	}
	return false;
}