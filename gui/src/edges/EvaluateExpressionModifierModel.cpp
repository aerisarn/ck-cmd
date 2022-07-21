#include <src/edges/EvaluateExpressionModifierModel.h>

using namespace ckcmd::HKX;

std::vector<const hkClass*> EvaluateExpressionModifierModel::handled_hkclasses() const
{
	return { &hkbEvaluateExpressionModifierClass };
}

hkbEvaluateExpressionModifier* EvaluateExpressionModifierModel::variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* graph = reinterpret_cast<hkbEvaluateExpressionModifier*>(variant->m_object);
		return graph;
	}
	return nullptr;
}

hkVariant* ExpressionsData(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	if (variant != nullptr)
	{
		auto* expressions = reinterpret_cast<hkbEvaluateExpressionModifier*>(variant->m_object);
		if (nullptr != expressions)
		{
			auto data = expressions->m_expressions;
			if (data != NULL)
			{
				return manager.findVariant(edge.file(), &*data);
			}
		}
	}
	return nullptr;
}

std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> EvaluateExpressionModifierModel::additional_variants() const
{
	return {
		ExpressionsData
	};
}