#include <src/edges/StateMachineModel.h>

#include <src/utility/Containers.h>

using namespace ckcmd::HKX;

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

int StateMachineModel::columns(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	return SupportEnhancedEdge::columns(row, edge, manager);
}

int StateMachineModel::childCount(const ModelEdge& edge, ResourceManager& manager) const
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

//virtual bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager) override;
//virtual bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager) override;
//virtual bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager) override;
//virtual bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager) override;

//
//int StateMachineModel::rows(const ModelEdge& edge, ResourceManager& manager)
//{
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
//		if (FSM == NULL)
//			return 0;
//
//		if (edge.childType() == NodeType::FSMStateTransitions)
//		{
//			int count = 0;
//			for (int i = 0; i < FSM->m_states.getSize(); i++)
//			{
//				if (NULL != FSM->m_states[i]->m_transitions)
//					count += FSM->m_states[i]->m_transitions->m_transitions.getSize();
//			}
//			return count;
//		}
//		else if (edge.childType() == NodeType::FSMWildcardTransitions)
//		{
//			if (FSM->m_wildcardTransitions == NULL)
//				return 0;
//			return FSM->m_wildcardTransitions->m_transitions.getSize();
//		}
//		else if (edge.childType() == NodeType::FSMStateTransition)
//		{
//			return 0;
//		}
//		else if (edge.childType() == NodeType::FSMWildcardTransition)
//		{
//			return 0;
//		}
//
//		return DATA_SUPPORTS + 1 + HkxLinkedTableVariant(*variant).rows();
//	}
//	return 0;
//}
//
//int StateMachineModel::columns(int row, const ModelEdge& edge, ResourceManager& manager)
//{
//	if (
//		edge.childType() == NodeType::FSMWildcardTransitions ||
//		edge.childType() == NodeType::FSMStateTransitions
//		)
//	{
//		return 1;
//	}
//	if (row == 0)
//		return 1;
//	if (row - 1 < DATA_SUPPORTS)
//		return 1;
//	hkVariant* variant = edge.childItem<hkVariant>();
//	return 1 + HkxLinkedTableVariant(*variant).columns(row - DATA_SUPPORTS - 1);
//}
//
//int StateMachineModel::childCount(const ModelEdge& edge, ResourceManager& manager)
//{
//	hkVariant* variant = edge.childItem<hkVariant>();
//	auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
//	if (FSM == NULL)
//		return 0;
//
//
//	if (
//		edge.childType() == NodeType::FSMWildcardTransitions ||
//		edge.childType() == NodeType::FSMWildcardTransition ||
//		edge.childType() == NodeType::FSMStateTransitions ||
//		edge.childType() == NodeType::FSMStateTransition
//		)
//	{
//		return rows(edge, manager);
//	}
//
//	return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).links().size();
//}
//
//std::pair<int, int> StateMachineModel::child(int index, const ModelEdge& edge, ResourceManager& manager)
//{
//	switch (edge.childType())
//	{
//	case NodeType::FSMWildcardTransitions:
//	case NodeType::FSMStateTransitions:
//		return { index, 0 };
//	case NodeType::FSMWildcardTransition:
//	case NodeType::FSMStateTransition:
//	case NodeType::behaviorCharacterProperty:
//		return { -1, -1 };
//	default:
//		break;
//	}
//	if (index < DATA_SUPPORTS)
//		return { index + 1, 0 };
//
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
//		if (FSM == NULL)
//			return { -1, -1 };
//
//		int real_child_index = 0;  
//		int desidered_children = index - DATA_SUPPORTS;
//		auto links = HkxLinkedTableVariant(*variant).links();
//		if (desidered_children >= 0 && desidered_children < links.size())
//		{
//			return { DATA_SUPPORTS + links.at(desidered_children)._row + 1, links.at(desidered_children)._column + 1 };
//		}
//	}
//	return { -1, -1 };
//}
//int StateMachineModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager)
//{
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
//		if (FSM == NULL)
//			return -1;
//
//		int FSM_rows = HkxTableVariant(*variant).rows();
//
//		switch (edge.childType())
//		{
//		case NodeType::FSMWildcardTransitions:
//		case NodeType::FSMStateTransitions:
//			return row;
//		default:
//			break;
//		}
//		int child_index = row - 1;
//		if (child_index >= 0 && child_index < DATA_SUPPORTS) {
//			return child_index;
//		}
//
//		int link_index = variantLink(row - 1 - DATA_SUPPORTS, column - 1, variant);
//		if (link_index != -1)
//			return DATA_SUPPORTS + link_index;
//	}
//	return -1;
//}
//ModelEdge StateMachineModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager)
//{
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
//		if (FSM == NULL)
//			return ModelEdge();
//
//		switch (edge.type())
//		{
//		case NodeType::FSMWildcardTransitions:
//			return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), variant, NodeType::FSMWildcardTransition);
//		case NodeType::FSMStateTransitions:
//			return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), variant, NodeType::FSMStateTransition);
//		case NodeType::FSMWildcardTransition:
//		case NodeType::FSMStateTransition:
//			throw std::runtime_error("Child from leaf node requested!");
//		default:
//			break;
//		}
//		int child_index = childIndex(row, column, edge, manager);
//		if (child_index != -1)
//		{
//			switch (child_index)
//			{
//			case 0:
//				return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::FSMWildcardTransitions);
//			case 1:
//				return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::FSMStateTransitions);
//			default:
//				break;
//			}
//		}
//		int link_index = variantLink(row - 1 - DATA_SUPPORTS, column - 1, variant);
//		if (link_index != -1)
//		{
//			HkxLinkedTableVariant v(*variant);
//			auto& links = v.links();
//			auto& link = links.at(link_index);
//			int file_index = manager.findIndex(edge.file(), link._ref);
//			if (file_index == -1)
//				__debugbreak();
//			return ModelEdge(edge, edge.project(), edge.file(), link._row + 1 + DATA_SUPPORTS, link._column + 1, edge.subindex(), manager.at(edge.file(), file_index), NodeType::HavokNative);
//		}
//	}
//	return ModelEdge();
//}
//
//QVariant StateMachineModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager)
//{
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
//		if (edge.childType() == NodeType::FSMWildcardTransitions ||
//			edge.childType() == NodeType::FSMStateTransitions)
//		{
//			return DataListsName(edge.type());
//		}
//		if (edge.childType() == NodeType::FSMWildcardTransition)
//		{
//			if (column == 0)
//			{
//				auto& transition = FSM->m_wildcardTransitions->m_transitions[row];
//				QString result = "Transition to Invalid State";
//				for (const auto& state : FSM->m_states)
//				{
//					if (state->m_stateId == transition.m_toStateId)
//					{
//						result = QString("Transition to ") + state->m_name.cString();
//					}
//				}
//				return result;
//			}
//			return "InvalidColumn";
//		}
//		if (edge.childType() == NodeType::FSMStateTransition)
//		{
//			if (column == 0)
//			{
//				int index = row;
//				QString result = "Invalid state transition";
//				for (const auto& state : FSM->m_states)
//				{
//					if (NULL != state->m_transitions)
//					{
//						if (index < state->m_transitions->m_transitions.getSize())
//						{
//							auto& transition = state->m_transitions->m_transitions[index];
//							for (const auto& to_state : FSM->m_states)
//							{
//								if (to_state->m_stateId == transition.m_toStateId)
//								{
//									result = QString("Transition from %1 to %2 ").arg(state->m_name.cString()).arg(to_state->m_name.cString());
//									break;
//								}
//							}
//							break;
//						}
//						index -= state->m_transitions->m_transitions.getSize();
//					}
//				}
//				return result;
//			}
//			return "InvalidColumn";
//		}
//		if (column == 0 && row == 0)
//		{
//			return HkxVariant(*variant).name();
//		}
//		else if (column == 0)
//		{
//			if (row - 1 < DATA_SUPPORTS)
//			{
//				switch (row - 1)
//				{
//				case 0:
//					return DataListsName(NodeType::FSMWildcardTransitions);
//				case 1:
//					return DataListsName(NodeType::FSMStateTransitions);
//				default:
//					break;
//				}
//			}
//			return HkxTableVariant(*variant).rowName(row - DATA_SUPPORTS - 1);
//		}
//		HkxTableVariant v(*variant);
//		return v.data(row - DATA_SUPPORTS - 1, column - 1);
//	}
//	return "Invalid row or column";
//}