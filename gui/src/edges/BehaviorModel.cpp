#include <src/edges/BehaviorModel.h>
#include <src/hkx/HkxLinkedTableVariant.h>

#include <src/utility/Containers.h>

using namespace ckcmd::HKX;

std::vector<NodeType> BehaviorModel::handled_types() const
{
	return {
		NodeType::BehaviorHkxNode,
		NodeType::behaviorEventNames,
		NodeType::behaviorVariableNames,
		NodeType::behaviorCharacterPropertyNames,
		NodeType::behaviorEventName,
		NodeType::behaviorVariable,
		NodeType::behaviorCharacterProperty
	};
}

hkbBehaviorGraph* BehaviorModel::variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		return graph;
	}
	return nullptr;
}

hkbBehaviorGraphData* BehaviorModel::data_variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		if (graph != nullptr)
		{
			return graph->m_data;
		}
	}
	return nullptr;
}

hkbBehaviorGraphStringData* BehaviorModel::string_variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		if (graph != nullptr)
		{
			auto data = graph->m_data;
			if (data != NULL)
				return data->m_stringData;
		}
	}
	return nullptr;
}

hkVariant* BehaviorData(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	if (variant != nullptr)
	{
		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		if (nullptr != graph)
		{
			auto data = graph->m_data;
			if (data != NULL)
			{
				int data_index = manager.findIndex(edge.file(), &*data);
				return manager.at(edge.file(), data_index);
			}
		}
	}
	return nullptr;
}

hkVariant* BehaviorStringData(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	if (variant != nullptr)
	{
		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		if (nullptr != graph)
		{
			auto data = graph->m_data;
			if (data != NULL)
			{
				auto string_data = data->m_stringData;
				if (string_data != NULL)
				{
					int string_data_index = manager.findIndex(edge.file(), &*string_data);
					return manager.at(edge.file(), string_data_index);
				}
			}
		}
	}
	return nullptr;
}


std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> BehaviorModel::additional_variants() const
{
	return {
		BehaviorData,
		BehaviorStringData
	};
}

int BehaviorModel::supports() const
{
	return 3;
}

const char* BehaviorModel::supportName(int support_index) const
{
	switch (support_index)
	{
	case 0:
		return "Events";
	case 1:
		return "Variables";
	case 2:
		return "Character Properties";
	default:
		break;
	}
	return "Invalid Support";
}

NodeType BehaviorModel::supportType(int support_index) const
{
	switch (support_index)
	{
	case 0:
		return NodeType::behaviorEventNames;
	case 1:
		return NodeType::behaviorVariableNames;
	case 2:
		return NodeType::behaviorCharacterPropertyNames;
	default:
		break;
	}
	return NodeType::Invalid;
}

int BehaviorModel::rows(const ModelEdge& edge, ResourceManager& manager) const
{
	auto string_data = string_variant(edge);
	if (string_data != nullptr)
	{
		if (edge.childType() == NodeType::behaviorEventNames)
		{
			return string_data->m_eventNames.getSize();
		}
		else if (edge.childType() == NodeType::behaviorVariableNames)
		{
			return string_data->m_variableNames.getSize();
		}
		else if (edge.childType() == NodeType::behaviorCharacterPropertyNames)
		{
			return string_data->m_characterPropertyNames.getSize();
		}
		else if (edge.childType() == NodeType::behaviorEventName)
		{
			return 0;
		}
		else if (edge.childType() == NodeType::behaviorVariable)
		{
			return 0;
		}
		else if (edge.childType() == NodeType::behaviorCharacterProperty)
		{
			return 0;
		}
		return SupportEnhancedEdge::rows(edge, manager);
	}
	return 0;
}

int BehaviorModel::columns(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	if (nullptr != variant(edge))
	{
		if (edge.childType() == NodeType::behaviorEventName)
		{
			return 0;
		}
		else if (edge.childType() == NodeType::behaviorVariable)
		{
			return 0;
		}
		else if (edge.childType() == NodeType::behaviorCharacterProperty)
		{
			return 0;
		}
		return SupportEnhancedEdge::columns(row, edge, manager);
	}
	return 0;
}

int BehaviorModel::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	if (nullptr != variant(edge))
	{
		if (
			edge.childType() == NodeType::behaviorEventNames ||
			edge.childType() == NodeType::behaviorEventName ||
			edge.childType() == NodeType::behaviorVariableNames ||
			edge.childType() == NodeType::behaviorVariable ||
			edge.childType() == NodeType::behaviorCharacterPropertyNames ||
			edge.childType() == NodeType::behaviorCharacterProperty
			)
		{
			return rows(edge, manager);
		}
		return SupportEnhancedEdge::childCount(edge, manager);
	}
	return 0;
}

std::pair<int, int> BehaviorModel::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType())
	{
	case NodeType::behaviorEventNames:
	case NodeType::behaviorVariableNames:
	case NodeType::behaviorCharacterPropertyNames:
		return { index, 0 };
	case NodeType::behaviorEventName:
	case NodeType::behaviorVariable:
	case NodeType::behaviorCharacterProperty:
		return { -1, -1 };
	default:
		break;
	}
	return SupportEnhancedEdge::child(index, edge, manager);
}

int BehaviorModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType())
	{
	case NodeType::behaviorEventNames:
	case NodeType::behaviorVariableNames:
	case NodeType::behaviorCharacterPropertyNames:
		return row;
	default:
		break;
	}
	return SupportEnhancedEdge::childIndex(row, column, edge, manager);
}

ModelEdge BehaviorModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.type())
	{
	case NodeType::behaviorEventNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::behaviorEventName);
	case NodeType::behaviorVariableNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::behaviorVariable);
	case NodeType::behaviorCharacterPropertyNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::behaviorCharacterProperty);
	case NodeType::behaviorEventName:
	case NodeType::behaviorVariable:
	case NodeType::behaviorCharacterProperty:
		throw std::runtime_error("Child from leaf node requested!");
	default:
		break;
	}
	return SupportEnhancedEdge::child(row, column, edge, manager);
}

QVariant BehaviorModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	auto* string_data = string_variant(edge);
	if (nullptr != string_data)
	{
		if (edge.childType() == NodeType::behaviorEventNames)
		{
			if (column == 0)
			{
				return supportName(0);
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::behaviorVariableNames)
		{
			if (column == 0)
			{
				return supportName(1);
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::behaviorCharacterPropertyNames)
		{
			if (column == 0)
			{
				return supportName(2);
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::behaviorEventName)
		{
			if (column == 0)
			{
				return string_data->m_eventNames[edge.row()].cString();
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::behaviorVariable)
		{
			if (column == 0)
			{
				return string_data->m_variableNames[edge.row()].cString();
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::behaviorCharacterProperty)
		{
			if (column == 0)
			{
				return string_data->m_characterPropertyNames[edge.row()].cString();
			}
			return "InvalidColumn";
		}
		return SupportEnhancedEdge::data(row, column, edge, manager);
	}
	return "No Behavior";
}
