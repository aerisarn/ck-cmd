#include <src/edges/ClipModel.h>

using namespace ckcmd::HKX;

std::vector<const hkClass*> ClipModel::handled_hkclasses() const
{
	return { &hkbClipGeneratorClass };
}

hkbClipGenerator* ClipModel::variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* graph = reinterpret_cast<hkbClipGenerator*>(variant->m_object);
		return graph;
	}
	return nullptr;
}

hkVariant* TriggerData(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	if (variant != nullptr)
	{
		auto* clip = reinterpret_cast<hkbClipGenerator*>(variant->m_object);
		if (nullptr != clip)
		{
			auto data = clip->m_triggers;
			if (data != NULL)
			{
				return manager.findVariant(edge.file(), &*data);
			}
		}
	}
	return nullptr;
}

std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> ClipModel::additional_variants() const
{
	return {
		TriggerData
	};
}