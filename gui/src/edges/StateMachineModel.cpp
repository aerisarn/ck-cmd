#include <src/edges/StateMachineModel.h>

#include <src/utility/Containers.h>
#include <src/hkx/HkxTableVariant.h>
#include <src/edges/TransitionModel.h>

#include <src/utility/Containers.h>

#include <hkbStateMachineTransitionInfo_1.h>
#include <hkbStateMachineTransitionInfoArray_0.h>

using namespace ckcmd::HKX;

std::vector<const hkClass*> StateMachineModel::handled_hkclasses() const
{
	return { &hkbStateMachineClass };
}

std::vector<NodeType> StateMachineModel::handled_types() const
{
	return {
		NodeType::FSMStateTransitions,
		NodeType::FSMWildcardTransitions,
		NodeType::FSMStateTransition,
		NodeType::FSMWildcardTransitions
	};
}

hkbStateMachine* StateMachineModel::variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
		return FSM;
	}
	return nullptr;
}

hkbStateMachineTransitionInfo* getStateTransitionFromRow(hkbStateMachine* FSM, int index)
{
	for (const auto& state : FSM->m_states)
	{
		if (NULL != state->m_transitions)
		{
			if (index < state->m_transitions->m_transitions.getSize())
			{
				return &state->m_transitions->m_transitions[index];
			}
			index -= state->m_transitions->m_transitions.getSize();
		}
	}
	return nullptr;
}

int StateMachineModel::rows(const ModelEdge& edge, ResourceManager& manager) const
{
	auto* FSM = variant(edge);
	if (nullptr != FSM)
	{
		if (edge.childType() == NodeType::FSMStateTransitions)
		{
			int count = 0;
			for (int i = 0; i < FSM->m_states.getSize(); i++)
			{
				if (NULL != FSM->m_states[i]->m_transitions)
					count += FSM->m_states[i]->m_transitions->m_transitions.getSize();
			}
			return count;
		}
		else if (edge.childType() == NodeType::FSMWildcardTransitions)
		{
			if (FSM->m_wildcardTransitions == NULL)
				return 0;
			return FSM->m_wildcardTransitions->m_transitions.getSize();
		}
		else if (edge.childType() == NodeType::FSMStateTransition)
		{
			
			hkVariant v;
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = getStateTransitionFromRow(FSM, edge.row());
			if (v.m_object != nullptr)
			{
				ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.row(), &v, NodeType::FSMWildcardTransition);
				return TransitionModel().rows(transitionEdge, manager);
			}
			return 0;
		}
		else if (edge.childType() == NodeType::FSMWildcardTransition)
		{
			if (edge.row() < FSM->m_wildcardTransitions->m_transitions.getSize())
			{
				hkVariant v;
				v.m_class = &hkbStateMachineTransitionInfoClass;
				v.m_object = &FSM->m_wildcardTransitions->m_transitions[edge.row()];
				ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.row(), &v, NodeType::FSMWildcardTransition);
				return TransitionModel().rows(transitionEdge, manager);
			}
			return 0;
		}
		return SupportEnhancedEdge::rows(edge, manager);
	}
	return 0;
}

int StateMachineModel::columns(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	auto* FSM = variant(edge);
	if (nullptr != FSM)
	{
		if (edge.childType() == NodeType::FSMWildcardTransition)
		{
			if (edge.row() < FSM->m_wildcardTransitions->m_transitions.getSize())
			{
				hkVariant v;
				v.m_class = &hkbStateMachineTransitionInfoClass;
				v.m_object = &FSM->m_wildcardTransitions->m_transitions[edge.row()];
				ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
				return TransitionModel().columns(row, transitionEdge, manager);
			}
			return 0;
		}
		if (edge.childType() == NodeType::FSMStateTransition)
		{
			if (row == 0)
			{
				return 2;
			}
			hkVariant v;
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = getStateTransitionFromRow(FSM, edge.row());
			if (v.m_object != nullptr)
			{
				ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
				return TransitionModel().columns(row, transitionEdge, manager);
			}
			return 0;
		}
		
		if (edge.childType() == NodeType::FSMWildcardTransitions)
			return rows(edge, manager) > 0 ? 1 : 0;
		if (edge.childType() == NodeType::FSMStateTransitions)
			return rows(edge, manager) > 0 ? 1 : 0;
		
		return SupportEnhancedEdge::columns(row, edge, manager);
	}
	return 0;
}

int StateMachineModel::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	auto* FSM = variant(edge);
	if (nullptr != FSM)
	{
		if (
			edge.childType() == NodeType::FSMWildcardTransitions ||
			edge.childType() == NodeType::FSMStateTransitions 
			)
		{
			return rows(edge, manager);
		}
		if (edge.childType() == NodeType::FSMWildcardTransition)
		{
			int count = 0;
			if (FSM->m_wildcardTransitions->m_transitions[edge.subindex()].m_transition != nullptr)
				count++;
			if (FSM->m_wildcardTransitions->m_transitions[edge.subindex()].m_condition != nullptr)
				count++;
			return count;
		}
		if (edge.childType() == NodeType::FSMStateTransition)
		{
			auto transition = getStateTransitionFromRow(FSM, edge.subindex());
			int count = 0;
			if (transition->m_transition != nullptr)
				count++;
			if (transition->m_condition != nullptr)
				count++;
			return count;
		}
		return SupportEnhancedEdge::childCount(edge, manager);
	}
	return 0;
}

std::pair<int, int> StateMachineModel::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType())
	{
	case NodeType::FSMWildcardTransitions:
	case NodeType::FSMStateTransitions:
		return { index, 0 };
	case NodeType::FSMWildcardTransition:
	{
		auto* FSM = variant(edge);
		switch (index)
		{
		case 0:
		{
			if (FSM != nullptr && FSM->m_wildcardTransitions != nullptr && FSM->m_wildcardTransitions->m_transitions[edge.subindex()].m_transition != nullptr)
				return { 9, 1 };
			return { MODELEDGE_INVALID, MODELEDGE_INVALID };
		}
		case 1:
		{
			if (FSM != nullptr && FSM->m_wildcardTransitions != nullptr && FSM->m_wildcardTransitions->m_transitions[edge.subindex()].m_condition != nullptr)
				return { 10, 1 };
			return { MODELEDGE_INVALID, MODELEDGE_INVALID };
		}
		default:
			break;
		}
		return  { MODELEDGE_INVALID, MODELEDGE_INVALID };
	}
	case NodeType::FSMStateTransition:
	{
		auto* FSM = variant(edge);
		auto transition = getStateTransitionFromRow(FSM, edge.subindex());
		if (transition != nullptr)
		{
			switch (index)
			{
			case 0:
			{
				return { 9, 1 };
			}
			case 1:
			{
				return { 10, 1 };
			}
			default:
				break;
			}
		}
		return  { MODELEDGE_INVALID, MODELEDGE_INVALID };
	}
	default:
		break;
	}
	return SupportEnhancedEdge::child(index, edge, manager);
}

int StateMachineModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType())
	{
	case NodeType::FSMWildcardTransitions:
	case NodeType::FSMStateTransitions:
		return row;
	case NodeType::FSMWildcardTransition:
	{
		auto* FSM = variant(edge);
		if (row == 9 && column == 1 && FSM != nullptr && FSM->m_wildcardTransitions != nullptr && FSM->m_wildcardTransitions->m_transitions[edge.subindex()].m_transition != nullptr)
			return 0;
		if (row == 9 && column == 1 && FSM != nullptr && FSM->m_wildcardTransitions != nullptr && FSM->m_wildcardTransitions->m_transitions[edge.subindex()].m_condition != nullptr)
			return 1;
		return MODELEDGE_INVALID;
	}
	case NodeType::FSMStateTransition:
	{
		auto* FSM = variant(edge);
		if (nullptr != FSM)
		{
			auto transition = getStateTransitionFromRow(FSM, edge.row());
			if (nullptr != transition)
			{
				if (row == 9 && column == 1 && FSM != nullptr && transition->m_transition != nullptr)
					return 0;
				if (row == 10 && column == 1 && FSM != nullptr && transition->m_condition != nullptr)
					return 1;
			}
		}
		return MODELEDGE_INVALID;
	}
	default:
		break;
	}
	return SupportEnhancedEdge::childIndex(row, column, edge, manager);
}

ModelEdge StateMachineModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.type())
	{
	case NodeType::FSMWildcardTransitions:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, row, edge.childItem<hkVariant>(), NodeType::FSMWildcardTransition);
	case NodeType::FSMStateTransitions:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, row, edge.childItem<hkVariant>(), NodeType::FSMStateTransition);
	case NodeType::FSMWildcardTransition:
	{
		auto* FSM = variant(edge);
		if (nullptr != FSM)
		{
			if (row == 9 && column == 1)
			{
				auto variant = manager.findVariant(edge.file(), &*FSM->m_wildcardTransitions->m_transitions[edge.subindex()].m_transition);
				return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::HavokNative);
			}
			if (row == 10 && column == 1)
			{
				auto variant = manager.findVariant(edge.file(), &*FSM->m_wildcardTransitions->m_transitions[edge.subindex()].m_condition);
				return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::HavokNative);
			}
		}
		return ModelEdge();
	}
	case NodeType::FSMStateTransition:
	{
		auto* FSM = variant(edge);
		if (nullptr != FSM)
		{
			auto transition = getStateTransitionFromRow(FSM, edge.subindex());
			if (nullptr != transition)
			{
				if (row == 9 && column == 1)
				{
					auto variant = manager.findVariant(edge.file(), transition->m_transition);
					return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::HavokNative);
				}
				if (row == 10 && column == 1)
				{
					auto variant = manager.findVariant(edge.file(), transition->m_condition);
					return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::HavokNative);
				}
			}
		}
		return ModelEdge();
	}
	default:
		break;
	}
	return SupportEnhancedEdge::child(row, column, edge, manager);
}

QVariant StateMachineModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	auto* FSM = variant(edge);
	if (nullptr != FSM)
	{
		if (edge.childType() == NodeType::FSMWildcardTransitions)
		{
			if (column == 0)
			{
				return supportName(0);
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::FSMStateTransitions)
		{
			if (column == 0)
			{
				return supportName(1);
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::FSMWildcardTransition)
		{
			auto& transition = FSM->m_wildcardTransitions->m_transitions[edge.subindex()];
			if (row == 0 && column == 0)
			{
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
			if (row == 9 && column == 1)
			{
				return "Transition effect not set";
			}
			if (row == 10 && column == 1)
			{
				return "Condition not set";
			}
			hkVariant v;
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = &FSM->m_wildcardTransitions->m_transitions[edge.row()];
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().data(row, column, transitionEdge, manager);
		}
		if (edge.childType() == NodeType::FSMStateTransition)
		{
			if (row == 0 && column == 0)
			{
				int index = edge.row();
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
			if (row == 0 && column == 1)
			{
				int index = edge.row();
				for (const auto& state : FSM->m_states)
				{
					if (NULL != state->m_transitions)
					{
						if (index < state->m_transitions->m_transitions.getSize())
						{
							return state->m_name.cString();
						}
						index -= state->m_transitions->m_transitions.getSize();
					}
				}
			}
			auto transition = getStateTransitionFromRow(FSM, edge.subindex());
			if (row == 9 && column == 1)
			{
				if (transition->m_transition == NULL)
					return "Transition effect not set";
			}
			if (row == 10 && column == 1)
			{
				if (transition->m_condition == NULL)
					return "Condition not set";
			}
			hkVariant v;
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = getStateTransitionFromRow(FSM, edge.subindex());
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().data(row, column, transitionEdge, manager);
		}
		return SupportEnhancedEdge::data(row, column, edge, manager);
	}
	return "No Generator";
}

bool StateMachineModel::setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager)
{
	auto* FSM = variant(edge);
	if (nullptr != FSM)
	{
		if (edge.childType() == NodeType::FSMWildcardTransition)
		{
			hkVariant v;
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = &FSM->m_wildcardTransitions->m_transitions[edge.row()];
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().setData(row, column, transitionEdge, data, manager);
		}
		if (edge.childType() == NodeType::FSMStateTransitions)
		{
			if (data.canConvert<QVector<int>>())
			{
				//special case from addactionhandler
				auto payload = data.value<QVector<int>>();
				int from_state_id = payload[0];
				int to_state_id = payload[1];
				int event_index = payload[2];
				for (const auto& state : FSM->m_states)
				{
					if (state->m_stateId == from_state_id)
					{
						if (state->m_transitions != nullptr && state->m_transitions->m_transitions.getSize() > 0)
						{
							auto& transition = state->m_transitions->m_transitions[state->m_transitions->m_transitions.getSize() - 1];
							transition.m_toStateId = to_state_id;
							transition.m_eventId = event_index;
							return true;
						}
					}
				}
			}
			return false;
		}
		if (edge.childType() == NodeType::FSMStateTransition)
		{		
			hkVariant v;
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = getStateTransitionFromRow(FSM, edge.subindex());
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().setData(row, column, transitionEdge, data, manager);
		}
	}
	return SupportEnhancedEdge::setData(row, column, edge, data, manager);
}

int StateMachineModel::supports() const
{
	return 2;
}

const char* StateMachineModel::supportName(int support_index) const
{
	switch (support_index) {
	case 0:
		return "Wildcard Transitions";
	case 1:
		return "Transitions";
	default:
		break;
	}
	return "Invalid FSM Entry";
}

NodeType  StateMachineModel::supportType(int support_index) const
{
	switch (support_index) {
	case 0:
		return NodeType::FSMWildcardTransitions;
	case 1:
		return NodeType::FSMStateTransitions;
	default:
		break;
	}
	return NodeType::Invalid;
}

TypeInfo StateMachineModel::rowClass(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	auto* FSM = variant(edge);
	if (nullptr != FSM)
	{
		if (edge.childType() == NodeType::FSMWildcardTransition)
		{
			hkVariant v;
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = &FSM->m_wildcardTransitions->m_transitions[edge.subindex()];
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().rowClass(row, transitionEdge, manager);
		}
		if (edge.childType() == NodeType::FSMStateTransition)
		{
			hkVariant v;
			auto transition = getStateTransitionFromRow(FSM, edge.subindex());
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = transition;
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().rowClass(row, transitionEdge, manager);
		}
	}
	return SupportEnhancedEdge::rowClass(row, edge, manager);
}

bool StateMachineModel::isArray(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	if (edge.childType() == NodeType::FSMWildcardTransitions ||
		edge.childType() == NodeType::FSMStateTransitions)
	{
		return true;
	}
	return SupportEnhancedEdge::isArray(row, edge, manager);
}

bool StateMachineModel::addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	auto* FSM = variant(edge);
	if (nullptr != FSM && nullptr != FSM->m_wildcardTransitions)
	{
		if (edge.childType() == NodeType::FSMWildcardTransitions)
		{
			return addToContainer(row_start, count, FSM->m_wildcardTransitions->m_transitions);
		}
		if (edge.childType() == NodeType::FSMStateTransitions)
		{
			int state_id = row_start;
			for (const auto& state : FSM->m_states)
			{
				if (state->m_stateId == row_start)
				{
					if (nullptr == state->m_transitions)
					{
						state->m_transitions = manager.createObject<hkbStateMachineTransitionInfoArray>(
							edge.file(), &hkbStateMachineTransitionInfoArrayClass);
					}
					auto& new_transition =  state->m_transitions->m_transitions.expandOne();
					memset(&new_transition, 0, sizeof(hkbStateMachineTransitionInfo));
					return true;
				}
			}
			return false;
		}
	}
	return SupportEnhancedEdge::addRows(row_start, count, edge, manager);
}

bool StateMachineModel::removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	auto* FSM = variant(edge);
	if (nullptr != FSM && nullptr != FSM->m_wildcardTransitions)
	{
		if (edge.childType() == NodeType::FSMWildcardTransitions)
		{
			if (row_start < FSM->m_wildcardTransitions->m_transitions.getSize())
			{
				FSM->m_wildcardTransitions->m_transitions.removeAtAndCopy(row_start);
				return true;
			}
			return false;
		}
		if (edge.childType() == NodeType::FSMStateTransitions)
		{
			int index = row_start;
			for (const auto& state : FSM->m_states)
			{
				if (NULL != state->m_transitions)
				{
					if (index < state->m_transitions->m_transitions.getSize())
					{
						state->m_transitions->m_transitions.removeAtAndCopy(index);
						return true;
					}
					index -= state->m_transitions->m_transitions.getSize();
				}
			}
			return false;
		}
	}
	return SupportEnhancedEdge::removeRows(row_start, count, edge, manager);
}