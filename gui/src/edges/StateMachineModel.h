#pragma once

#include <src/models/ModelEdge.h>

#include <hkbStateMachine_4.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbStateMachineTransitionInfoArray_0.h>

namespace ckcmd {
	namespace HKX {

		struct  StateMachineModel
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
					return v.data(row - DATA_SUPPORTS, column - 1);
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
				return ModelEdge((hkVariant*)nullptr, project, file, link._row, link._column + 1, manager.at(file, file_index));
			}

			static const int getRows(int project, int file, int row, int column, hkVariant* variant, NodeType childType, ResourceManager& manager)
			{
				if (
					childType == NodeType::FSMWildcardTransitions ||
					childType == NodeType::FSMStateTransitions
					)
				{
					if (column == 0)
						return getChildCount(variant, childType);
					return 0;
				}

				auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
				if (FSM == NULL)
					return 0;

				if (childType == NodeType::HavokNative)
				{
					return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).rows();
				}
				return HkxLinkedTableVariant(*variant).rows();
			}

			static const int getColumns(int row, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
			{
				if (
					childType == NodeType::FSMWildcardTransitions ||
					childType == NodeType::FSMStateTransitions
					)
				{
					return 1;
				}
				return 1 + HkxLinkedTableVariant(*variant).columns(row);
			}

		};


	}
}
