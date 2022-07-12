#include <src/edges/StateMachineModel.h>

#include <src/utility/Containers.h>
#include <src/hkx/HkxTableVariant.h>
#include <src/edges/TransitionModel.h>

#include <hkbStateMachineTransitionInfo_1.h>

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
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.row(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().rows(transitionEdge, manager);
		}
		else if (edge.childType() == NodeType::FSMWildcardTransition)
		{
			hkVariant v;
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = &FSM->m_wildcardTransitions->m_transitions[edge.row()];
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.row(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().rows(transitionEdge, manager);
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
			hkVariant v;
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = &FSM->m_wildcardTransitions->m_transitions[edge.row()];
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().columns(row, transitionEdge, manager);
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
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().columns(row, transitionEdge, manager);
		}
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
			auto transition = getStateTransitionFromRow(FSM, edge.row());
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
	case NodeType::FSMStateTransition:
		switch (index)
		{
		case 0:
			return { 3, 1 };
		case 1:
			return { 4, 1 };
		default:
			break;
		}
		return  { MODELEDGE_INVALID, MODELEDGE_INVALID };
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
	case NodeType::FSMStateTransition:
		if (row == 3 && column == 1)
			return 0;
		if (row == 4 && column == 1)
			return 1;
		return MODELEDGE_INVALID;
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
			if (row == 3 && column == 1)
			{
				auto variant = manager.findVariant(edge.file(), &*FSM->m_wildcardTransitions->m_transitions[edge.subindex()].m_transition);
				return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::HavokNative);
			}
			if (row == 4 && column == 1)
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
				if (row == 3 && column == 1)
				{
					auto variant = manager.findVariant(edge.file(), transition->m_transition);
					return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::HavokNative);
				}
				if (row == 4 && column == 1)
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
			if (row == 0 && column == 0)
			{
				auto& transition = FSM->m_wildcardTransitions->m_transitions[edge.subindex()];
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
			if (row == 4 && column == 1)
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
							return state->m_name;
						}
						index -= state->m_transitions->m_transitions.getSize();
					}
				}
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
		if (edge.childType() == NodeType::FSMStateTransition)
		{
			hkVariant v;
			v.m_class = &hkbStateMachineTransitionInfoClass;
			v.m_object = getStateTransitionFromRow(FSM, edge.subindex());
			ModelEdge transitionEdge(edge, edge.project(), edge.file(), edge.row(), edge.subindex(), edge.column(), &v, NodeType::FSMWildcardTransition);
			return TransitionModel().setData(row, column, transitionEdge, data, manager);
		}
		return false;
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
	return SupportEnhancedEdge::rowClass(row == 0 ? row : row - supports(), edge, manager);
}