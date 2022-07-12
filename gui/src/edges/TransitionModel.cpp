#include <src/edges/TransitionModel.h>

using namespace ckcmd::HKX;

hkbStateMachineTransitionInfo* TransitionModel::variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* graph = reinterpret_cast<hkbStateMachineTransitionInfo*>(variant->m_object);
		return graph;
	}
	return nullptr;
}

hkVariant* EffectData(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	if (variant != nullptr)
	{
		auto* transition = reinterpret_cast<hkbStateMachineTransitionInfo*>(variant->m_object);
		if (nullptr != transition)
		{
			auto data = transition->m_transition;
			if (data != NULL)
			{
				return manager.findVariant(edge.file(), &*data);
			}
		}
	}
	return nullptr;
}

hkVariant* ConditionData(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	if (variant != nullptr)
	{
		auto* transition = reinterpret_cast<hkbStateMachineTransitionInfo*>(variant->m_object);
		if (nullptr != transition)
		{
			auto data = transition->m_condition;
			if (data != NULL)
			{
				return manager.findVariant(edge.file(), &*data);
			}
		}
	}
	return nullptr;
}

std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> TransitionModel::additional_variants() const
{
	return {
		EffectData,
		ConditionData,
	};
}