#include <src/edges/BehaviorModel.h>
#include <src/hkx/HkxLinkedTableVariant.h>

#include <src/utility/Containers.h>

using namespace ckcmd::HKX;

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
	return nullptr;
}

hkVariant* BehaviorStringData(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
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

int BehaviorModel::childCount(const ModelEdge& edge, ResourceManager& manager) const
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

//const QStringList BehaviorModel::children_rows
//{
//	"eventNames",
//	"variableNames",
//	"characterPropertyNames"
//};
//
//
//int BehaviorModel::rows(const ModelEdge& edge, ResourceManager& manager)
//{
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
//		if (graph == NULL)
//			return 0;
//		auto data = graph->m_data;
//		if (data == NULL)
//			return 0;
//		auto string_data = data->m_stringData;
//		if (string_data == NULL)
//			return 0;
//
//		if (edge.childType() == NodeType::behaviorEventNames)
//		{
//			return string_data->m_eventNames.getSize();
//		}
//		else if (edge.childType() == NodeType::behaviorVariableNames)
//		{
//			return string_data->m_variableNames.getSize();
//		}
//		else if (edge.childType() == NodeType::behaviorCharacterPropertyNames)
//		{
//			return string_data->m_characterPropertyNames.getSize();
//		}
//		else if (edge.childType() == NodeType::behaviorEventName)
//		{
//			return 0;
//		}
//		else if (edge.childType() == NodeType::behaviorVariable)
//		{
//			return 0;
//		}
//		else if (edge.childType() == NodeType::behaviorCharacterProperty)
//		{
//			return 0;
//		}
//		int graph_rows = HkxTableVariant(*variant).rows();
//		int data_index = manager.findIndex(edge.file(), &*data);
//		int data_rows = HkxTableVariant(*manager.at(edge.file(), data_index)).rows();
//		int stringdata_index = manager.findIndex(edge.file(), &*string_data);
//		int stringdata_rows = HkxTableVariant(*manager.at(edge.file(), stringdata_index)).rows();
//
//		return 1 + graph_rows + data_rows + stringdata_rows;
//	}
//	return 0;
//}
//
//int BehaviorModel::columns(int row, const ModelEdge& edge, ResourceManager& manager)
//{
//	if (row == 0)
//		return 1;
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		if (
//			edge.childType() == NodeType::behaviorEventNames ||
//			edge.childType() == NodeType::behaviorVariableNames ||
//			edge.childType() == NodeType::behaviorCharacterPropertyNames ||
//			edge.childType() == NodeType::behaviorEventName ||
//			edge.childType() == NodeType::behaviorVariable
//			)
//		{
//			return 1;
//		}
//		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
//		if (graph == NULL)
//			return 0;
//		auto data = graph->m_data;
//		if (data == NULL)
//			return 0;
//		auto string_data = data->m_stringData;
//		if (string_data == NULL)
//			return 0;
//
//		int graph_rows = HkxTableVariant(*variant).rows();
//		int data_index = manager.findIndex(edge.file(), &*data);
//		int data_rows = HkxTableVariant(*manager.at(edge.file(), data_index)).rows();
//		int stringdata_index = manager.findIndex(edge.file(), &*string_data);
//		int stringdata_rows = HkxTableVariant(*manager.at(edge.file(), stringdata_index)).rows();
//
//		if (row - 1 < graph_rows)
//		{
//			return HkxTableVariant(*variant).columns(row - 1) + 1;
//		}
//		if (row - 1 - graph_rows < data_rows)
//		{
//			return HkxTableVariant(*manager.at(edge.file(), data_index)).columns(row - 1 - graph_rows) + 1;
//		}
//		if (row - 1 - graph_rows - data_rows < stringdata_rows)
//		{
//			return HkxTableVariant(*manager.at(edge.file(), stringdata_index)).columns(row - 1 - graph_rows - data_rows) + 1;
//		}
//	}
//	return 0;
//}
//
//int BehaviorModel::childCount(const ModelEdge& edge, ResourceManager& manager)
//{
//	switch (edge.childType())
//	{
//	case NodeType::behaviorEventNames:
//	case NodeType::behaviorVariableNames:
//	case NodeType::behaviorCharacterPropertyNames:
//		return rows(edge, manager);
//	case NodeType::behaviorEventName:
//	case NodeType::behaviorVariable:
//	case NodeType::behaviorCharacterProperty:
//		return 0;
//	default:
//		break;
//	}
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		if (edge.childType() == NodeType::BehaviorHkxNode)
//		{
//			int size = HkxLinkedTableVariant(*variant).links().size();
//			return children_rows.size() + size;
//		}
//	}
//	return 0;
//}
//
//std::pair<int, int> BehaviorModel::child(int index, const ModelEdge& edge, ResourceManager& manager)
//{
//	switch (edge.childType())
//	{
//	case NodeType::behaviorEventNames:
//	case NodeType::behaviorVariableNames:
//	case NodeType::behaviorCharacterPropertyNames:
//		return { index, 0 };
//	case NodeType::behaviorEventName:
//	case NodeType::behaviorVariable:
//	case NodeType::behaviorCharacterProperty:
//		return { -1, -1 };
//	default:
//		break;
//	}
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
//		if (graph == NULL)
//			return { -1, -1 };
//		auto data = graph->m_data;
//		if (data == NULL)
//			return { -1, -1 };
//		auto string_data = data->m_stringData;
//		if (string_data == NULL)
//			return { -1, -1 };
//
//		int graph_rows = HkxTableVariant(*variant).rows();
//		int data_index = manager.findIndex(edge.file(), &*data);
//		int data_rows = HkxTableVariant(*manager.at(edge.file(), data_index)).rows();
//		int stringdata_index = manager.findIndex(edge.file(), &*string_data);
//		int stringdata_rows = HkxTableVariant(*manager.at(edge.file(), stringdata_index)).rows();
//		if (index < children_rows.size())
//		{
//			const QString& row_name = children_rows[index];
//			int row_index = HkxTableVariant(*manager.at(edge.file(), stringdata_index)).rowNames().indexOf(row_name);
//			if (row_index != -1)
//				return { 1 + graph_rows + data_rows + row_index, 0 };
//		}
//		auto links = HkxLinkedTableVariant(*variant).links();
//		int graph_links = links.size();
//		int graph_link_index = index - children_rows.size();
//		if  (graph_link_index >= 0 && graph_link_index < graph_links)
//		{
//			return { links.at(graph_link_index)._row + 1, links.at(graph_link_index)._column + 1 };
//		}
//	}
//	return { -1, -1 };
//}
//
//int BehaviorModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager)
//{
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
//		if (graph == NULL)
//			return -1;
//		auto data = graph->m_data;
//		if (data == NULL)
//			return -1;
//		auto string_data = data->m_stringData;
//		if (string_data == NULL)
//			return -1;
//
//		int graph_rows = HkxTableVariant(*variant).rows();
//		int data_index = manager.findIndex(edge.file(), &*data);
//		int data_rows = HkxTableVariant(*manager.at(edge.file(), data_index)).rows();
//		int stringdata_index = manager.findIndex(edge.file(), &*string_data);
//		int stringdata_rows = HkxTableVariant(*manager.at(edge.file(), stringdata_index)).rows();
//
//		switch (edge.childType())
//		{
//		case NodeType::behaviorEventNames:
//		case NodeType::behaviorVariableNames:
//		case NodeType::behaviorCharacterPropertyNames:
//			return row;
//		default:
//			break;
//		}
//		int child_index = row - 1 - graph_rows - data_rows;
//		if (child_index >= 0 && child_index < stringdata_rows) {
//			return children_rows.indexOf(HkxTableVariant(*manager.at(edge.file(), stringdata_index)).rowName(child_index));
//		}
//
//		int link_index = variantLink(row - 1, column - 1, variant);
//		if (link_index != -1)
//			return children_rows.size() + link_index;
//	}
//	return -1;
//}
//
//ModelEdge BehaviorModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager)
//{
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
//		if (graph == NULL)
//			return ModelEdge();
//		auto data = graph->m_data;
//		if (data == NULL)
//			return ModelEdge();
//		auto string_data = data->m_stringData;
//		if (string_data == NULL)
//			return ModelEdge();
//
//		switch (edge.type())
//		{
//		case NodeType::behaviorEventNames:
//			return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), variant, NodeType::behaviorEventName);
//		case NodeType::behaviorVariableNames:
//			return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), variant, NodeType::behaviorVariable);
//		case NodeType::behaviorCharacterPropertyNames:
//			return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), variant, NodeType::behaviorCharacterProperty);
//		case NodeType::behaviorEventName:
//		case NodeType::behaviorVariable:
//		case NodeType::behaviorCharacterProperty:
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
//				return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::behaviorEventNames);
//			case 1:
//				return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::behaviorVariableNames);
//			case 2:
//				return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::behaviorCharacterPropertyNames);
//			default:
//				break;
//			}
//		}
//		int link_index = variantLink(row - 1, column -1, variant);
//		if (link_index != -1)
//		{
//			HkxLinkedTableVariant v(*variant);
//			auto& links = v.links();
//			auto& link = links.at(link_index);
//			int file_index = manager.findIndex(edge.file(), link._ref);
//			if (file_index == -1)
//				__debugbreak();
//			return ModelEdge(edge, edge.project(), edge.file(), link._row + 1, link._column + 1, edge.subindex(), manager.at(edge.file(), file_index),NodeType::HavokNative);
//		}
//	}
//	return ModelEdge();
//}
//
//QVariant BehaviorModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager)
//{
//	hkVariant* variant = edge.childItem<hkVariant>();
//	if (nullptr != variant)
//	{
//		auto* data = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
//		auto string_data = data->m_data->m_stringData;
//		if (edge.childType() == NodeType::behaviorEventNames ||
//			edge.childType() == NodeType::behaviorVariableNames ||
//			edge.childType() == NodeType::behaviorCharacterPropertyNames)
//		{
//			return DataListsName(edge.type());
//		}
//		if (edge.childType() == NodeType::behaviorEventName)
//		{
//			if (column == 0)
//				return string_data->m_eventNames[edge.row()].cString();
//			return "InvalidColumn";
//		}
//		if (edge.childType() == NodeType::behaviorVariable)
//		{
//			if (column == 0)
//			{
//				return string_data->m_variableNames[edge.row()].cString();
//			}
//			return "InvalidColumn";
//		}
//		if (edge.childType() == NodeType::behaviorCharacterProperty)
//		{
//			if (column == 0)
//			{
//				return string_data->m_characterPropertyNames[edge.row()].cString();
//			}
//			return "InvalidColumn";
//		}
//		if (column == 0 && row == 0)
//		{
//			return HkxVariant(*variant).name();
//		}
//		else if (column == 0)
//		{
//			return HkxTableVariant(*variant).rowName(row - 1);
//		}
//		HkxTableVariant v(*variant);
//		return v.data(row - 1, column - 1);
//	}
//	return "Invalid row or column";
//}
//
////bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager);
////bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
////bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
////bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager);
//
//
////int BehaviorModel::childRows(int row, int column, ModelEdge& edge, ResourceManager& manager)
////{
////	if (
////		childType == NodeType::behaviorEventNames ||
////		childType == NodeType::behaviorVariableNames ||
////		childType == NodeType::behaviorCharacterPropertyNames ||
////		childType == NodeType::behaviorEventName ||
////		childType == NodeType::behaviorVariable
////		)
////	{
////		if (column == 0)
////			return getChildCount(variant, childType);
////		return 0;
////	}
////
////	auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
////	if (graph == NULL)
////		return 0;
////
////	if (childType == NodeType::BehaviorHkxNode)
////	{
////		return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).rows();
////	}
////	return HkxLinkedTableVariant(*variant).rows();
////}
////
////int BehaviorModel::childRowColumns(int row, ModelEdge& edge, ResourceManager& manager)
////{
////
////}
////
////int BehaviorModel::childColumns(int row, int column, ModelEdge& edge, ResourceManager& manager)
////{
////	if (
////		childType == NodeType::behaviorEventNames ||
////		childType == NodeType::behaviorVariableNames ||
////		childType == NodeType::behaviorCharacterPropertyNames ||
////		childType == NodeType::behaviorEventName ||
////		childType == NodeType::behaviorVariable
////		)
////	{
////		return 1;
////	}
////	return 1 + HkxLinkedTableVariant(*variant).columns(row);
////}
////
////int BehaviorModel::childCount(ModelEdge& edge, ResourceManager& manager)
////{
////	auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
////	if (graph == NULL)
////		return 0;
////	auto data = graph->m_data;
////	if (data == NULL)
////		return 0;
////	auto string_data = data->m_stringData;
////	if (string_data == NULL)
////		return 0;
////	if (childType == NodeType::BehaviorHkxNode)
////	{
////		return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).links().size();
////	}
////	else if (childType == NodeType::behaviorEventNames)
////	{
////		return string_data->m_eventNames.getSize();
////	}
////	else if (childType == NodeType::behaviorVariableNames)
////	{
////		return string_data->m_variableNames.getSize();
////	}
////	else if (childType == NodeType::behaviorCharacterPropertyNames)
////	{
////		return string_data->m_characterPropertyNames.getSize();
////	}
////	else if (childType == NodeType::behaviorEventName)
////	{
////		return 0;
////	}
////	else if (childType == NodeType::behaviorVariable)
////	{
////		return 0;
////	}
////	/*Character Properties*/
////
////	else if (childType == NodeType::behaviorCharacterProperty)
////	{
////		return 0;
////	}
////	return HkxLinkedTableVariant(*variant).links().size();
////}
////
////bool BehaviorModel::hasChild(int row, int column, ModelEdge& edge, ResourceManager& manager)
////{
////	if (column == 0)
////	{
////		if (row < DATA_SUPPORTS)
////			return true;
////		return false;
////	}
////	int rows = getRows(project, file, row, column, variant, childType, manager);
////}
////
////ModelEdge BehaviorModel::child(int row, int column, ModelEdge& edge, ResourceManager& manager)
////{
////	if (childType == NodeType::BehaviorHkxNode)
////	{
////		switch (index) {
////		case 0:
////			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorEventNames);
////		case 1:
////			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorVariableNames);
////		case 2:
////			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorCharacterPropertyNames);
////		default:
////			break;
////		}
////	}
////	else if (childType == NodeType::behaviorEventNames)
////	{
////		return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorEventName);
////	}
////	else if (childType == NodeType::behaviorVariableNames)
////	{
////		return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorVariable);
////	}
////	else if (childType == NodeType::behaviorCharacterPropertyNames)
////	{
////		return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorCharacterProperty);
////	}
////	HkxLinkedTableVariant v(*variant);
////	auto& links = v.links();
////	auto& link = links.at(index - DATA_SUPPORTS);
////	int file_index = manager.findIndex(file, link._ref);
////	if (file_index == -1)
////		__debugbreak();
////	return ModelEdge((hkVariant*)nullptr, project, file, link._row, link._column + 1, manager.at(file, file_index));
////}
////
////QVariant BehaviorModel::data(int row, int column, ModelEdge& edge, const QVariant& data, ResourceManager& manager)
////{
////	auto* data = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
////	auto string_data = data->m_data->m_stringData;
////	if (childType == NodeType::behaviorEventNames ||
////		childType == NodeType::behaviorVariableNames ||
////		childType == NodeType::behaviorCharacterPropertyNames)
////	{
////		return DataListsName((int)childType);
////	}
////	if (childType == NodeType::behaviorEventName)
////	{
////		if (column == 0)
////			return string_data->m_eventNames[row].cString();
////		return "InvalidColumn";
////	}
////	if (childType == NodeType::behaviorVariable)
////	{
////		if (column == 0)
////		{
////			return string_data->m_variableNames[row].cString();
////		}
////		return "InvalidColumn";
////	}
////	if (childType == NodeType::behaviorCharacterProperty)
////	{
////		if (column == 0)
////		{
////			return string_data->m_characterPropertyNames[row].cString();
////		}
////		return "InvalidColumn";
////	}
////	if (column == 0)
////	{
////		return HkxVariant(*variant).name();
////	}
////	HkxLinkedTableVariant v(*variant);
////	return v.data(row - DATA_SUPPORTS, column - 1);
////}
////
////bool BehaviorModel::setData(int row, int column, ModelEdge& edge, ResourceManager& manager)
////{
////	auto* data = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
////	auto string_data = data->m_data->m_stringData;
////	if (childType == NodeType::behaviorEventName)
////	{
////		string_data->m_eventNames[row] = value.toString().toUtf8().constData();
////		return true;
////	}
////	//else if (childType == NodeType::behaviorVariableName)
////	//{
////	//	string_data->m_variableNames[row] = value.toString().toUtf8().constData();
////	//}
////	//else if (childType == NodeType::characterPropertyName)
////	//{
////	//	string_data->m_characterPropertyNames[row] = value.toString().toUtf8().constData();
////	//}
////	return false;
////}
////
////bool BehaviorModel::addRows(int row_start, int count, ModelEdge& edge, ResourceManager& manager)
////{
////
////}
////
////bool BehaviorModel::removeRows(int row_start, int count, ModelEdge& edge, ResourceManager& manager)
////{
////
////}
////bool BehaviorModel::changeColumns(int row, int column_start, int delta, ModelEdge& edge, ResourceManager& manager)
////{
////
////}
