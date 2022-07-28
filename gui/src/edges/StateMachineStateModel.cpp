#include <src/edges/StateMachineStateModel.h>

using namespace ckcmd::HKX;

std::vector<const hkClass*> StateMachineStateModel::handled_hkclasses() const
{
	return { &hkbStateMachineStateInfoClass };
}

hkbStateMachineStateInfo* StateMachineStateModel::variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* state = reinterpret_cast<hkbStateMachineStateInfo*>(variant->m_object);
		return state;
	}
	return nullptr;
}

hkVariant* EnterNotifyEvent(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	if (variant != nullptr)
	{
		auto* state = reinterpret_cast<hkbStateMachineStateInfo*>(variant->m_object);
		if (nullptr != state)
		{
			if (nullptr != state->m_enterNotifyEvents)
			{
				auto data = state->m_enterNotifyEvents.val();
				if (data != NULL)
				{
					return manager.findVariant(edge.file(), &*data);
				}
			}
		}
	}
	return nullptr;
}

hkVariant* ExitNotifyEvent(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	if (variant != nullptr)
	{
		auto* state = reinterpret_cast<hkbStateMachineStateInfo*>(variant->m_object);
		if (nullptr != state)
		{
			if (nullptr != state->m_exitNotifyEvents)
			{
				auto data = state->m_exitNotifyEvents.val();
				if (data != NULL)
				{
					return manager.findVariant(edge.file(), &*data);
				}
			}
		}
	}
	return nullptr;
}

std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> StateMachineStateModel::additional_variants() const
{
	return {
		EnterNotifyEvent,
		ExitNotifyEvent,
	};
}

std::vector<QString> StateMachineStateModel::additional_variants_names() const 
{ 
	return 
	{
		"enterNotifyEvent",
		"exitNotifyEvent"
	}; 
};