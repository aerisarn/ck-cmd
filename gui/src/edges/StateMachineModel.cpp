#include <src/edges/StateMachineModel.h>

#include <src/utility/Containers.h>

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
			return 0;
		}
		else if (edge.childType() == NodeType::FSMWildcardTransition)
		{
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
			edge.childType() == NodeType::FSMWildcardTransition ||
			edge.childType() == NodeType::FSMStateTransitions ||
			edge.childType() == NodeType::FSMStateTransition
			)
		{
			return rows(edge, manager);
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
		return { -1, -1 };
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
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::FSMWildcardTransition);
	case NodeType::FSMStateTransitions:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::FSMStateTransition);
	case NodeType::FSMWildcardTransition:
	case NodeType::FSMStateTransition:
		throw std::runtime_error("Child from leaf node requested!");
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
			if (column == 0)
			{
				auto& transition = FSM->m_wildcardTransitions->m_transitions[edge.row()];
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
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::FSMStateTransition)
		{
			if (column == 0)
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
			return "InvalidColumn";
		}
		return SupportEnhancedEdge::data(row, column, edge, manager);
	}
	return "No Generator";
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