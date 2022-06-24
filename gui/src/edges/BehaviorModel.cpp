//#include <src/edges/BehaviorModel.h>
//
//using namespace ckcmd::HKX;
//
//int BehaviorModel::childRows(int row, int column, ModelEdge& edge, ResourceManager& manager)
//{
//	if (
//		childType == NodeType::behaviorEventNames ||
//		childType == NodeType::behaviorVariableNames ||
//		childType == NodeType::behaviorCharacterPropertyNames ||
//		childType == NodeType::behaviorEventName ||
//		childType == NodeType::behaviorVariable
//		)
//	{
//		if (column == 0)
//			return getChildCount(variant, childType);
//		return 0;
//	}
//
//	auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
//	if (graph == NULL)
//		return 0;
//
//	if (childType == NodeType::BehaviorHkxNode)
//	{
//		return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).rows();
//	}
//	return HkxLinkedTableVariant(*variant).rows();
//}
//
//int BehaviorModel::childRowColumns(int row, ModelEdge& edge, ResourceManager& manager)
//{
//
//}
//
//int BehaviorModel::childColumns(int row, int column, ModelEdge& edge, ResourceManager& manager)
//{
//	if (
//		childType == NodeType::behaviorEventNames ||
//		childType == NodeType::behaviorVariableNames ||
//		childType == NodeType::behaviorCharacterPropertyNames ||
//		childType == NodeType::behaviorEventName ||
//		childType == NodeType::behaviorVariable
//		)
//	{
//		return 1;
//	}
//	return 1 + HkxLinkedTableVariant(*variant).columns(row);
//}
//
//int BehaviorModel::childCount(ModelEdge& edge, ResourceManager& manager)
//{
//	auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
//	if (graph == NULL)
//		return 0;
//	auto data = graph->m_data;
//	if (data == NULL)
//		return 0;
//	auto string_data = data->m_stringData;
//	if (string_data == NULL)
//		return 0;
//	if (childType == NodeType::BehaviorHkxNode)
//	{
//		return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).links().size();
//	}
//	else if (childType == NodeType::behaviorEventNames)
//	{
//		return string_data->m_eventNames.getSize();
//	}
//	else if (childType == NodeType::behaviorVariableNames)
//	{
//		return string_data->m_variableNames.getSize();
//	}
//	else if (childType == NodeType::behaviorCharacterPropertyNames)
//	{
//		return string_data->m_characterPropertyNames.getSize();
//	}
//	else if (childType == NodeType::behaviorEventName)
//	{
//		return 0;
//	}
//	else if (childType == NodeType::behaviorVariable)
//	{
//		return 0;
//	}
//	/*Character Properties*/
//
//	else if (childType == NodeType::behaviorCharacterProperty)
//	{
//		return 0;
//	}
//	return HkxLinkedTableVariant(*variant).links().size();
//}
//
//bool BehaviorModel::hasChild(int row, int column, ModelEdge& edge, ResourceManager& manager)
//{
//	if (column == 0)
//	{
//		if (row < DATA_SUPPORTS)
//			return true;
//		return false;
//	}
//	int rows = getRows(project, file, row, column, variant, childType, manager);
//}
//
//ModelEdge BehaviorModel::child(int row, int column, ModelEdge& edge, ResourceManager& manager)
//{
//	if (childType == NodeType::BehaviorHkxNode)
//	{
//		switch (index) {
//		case 0:
//			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorEventNames);
//		case 1:
//			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorVariableNames);
//		case 2:
//			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorCharacterPropertyNames);
//		default:
//			break;
//		}
//	}
//	else if (childType == NodeType::behaviorEventNames)
//	{
//		return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorEventName);
//	}
//	else if (childType == NodeType::behaviorVariableNames)
//	{
//		return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorVariable);
//	}
//	else if (childType == NodeType::behaviorCharacterPropertyNames)
//	{
//		return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorCharacterProperty);
//	}
//	HkxLinkedTableVariant v(*variant);
//	auto& links = v.links();
//	auto& link = links.at(index - DATA_SUPPORTS);
//	int file_index = manager.findIndex(file, link._ref);
//	if (file_index == -1)
//		__debugbreak();
//	return ModelEdge((hkVariant*)nullptr, project, file, link._row, link._column + 1, manager.at(file, file_index));
//}
//
//QVariant BehaviorModel::data(int row, int column, ModelEdge& edge, const QVariant& data, ResourceManager& manager)
//{
//	auto* data = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
//	auto string_data = data->m_data->m_stringData;
//	if (childType == NodeType::behaviorEventNames ||
//		childType == NodeType::behaviorVariableNames ||
//		childType == NodeType::behaviorCharacterPropertyNames)
//	{
//		return DataListsName((int)childType);
//	}
//	if (childType == NodeType::behaviorEventName)
//	{
//		if (column == 0)
//			return string_data->m_eventNames[row].cString();
//		return "InvalidColumn";
//	}
//	if (childType == NodeType::behaviorVariable)
//	{
//		if (column == 0)
//		{
//			return string_data->m_variableNames[row].cString();
//		}
//		return "InvalidColumn";
//	}
//	if (childType == NodeType::behaviorCharacterProperty)
//	{
//		if (column == 0)
//		{
//			return string_data->m_characterPropertyNames[row].cString();
//		}
//		return "InvalidColumn";
//	}
//	if (column == 0)
//	{
//		return HkxVariant(*variant).name();
//	}
//	HkxLinkedTableVariant v(*variant);
//	return v.data(row - DATA_SUPPORTS, column - 1);
//}
//
//bool BehaviorModel::setData(int row, int column, ModelEdge& edge, ResourceManager& manager)
//{
//	auto* data = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
//	auto string_data = data->m_data->m_stringData;
//	if (childType == NodeType::behaviorEventName)
//	{
//		string_data->m_eventNames[row] = value.toString().toUtf8().constData();
//		return true;
//	}
//	//else if (childType == NodeType::behaviorVariableName)
//	//{
//	//	string_data->m_variableNames[row] = value.toString().toUtf8().constData();
//	//}
//	//else if (childType == NodeType::characterPropertyName)
//	//{
//	//	string_data->m_characterPropertyNames[row] = value.toString().toUtf8().constData();
//	//}
//	return false;
//}
//
//bool BehaviorModel::addRows(int row_start, int count, ModelEdge& edge, ResourceManager& manager)
//{
//
//}
//
//bool BehaviorModel::removeRows(int row_start, int count, ModelEdge& edge, ResourceManager& manager)
//{
//
//}
//bool BehaviorModel::changeColumns(int row, int column_start, int delta, ModelEdge& edge, ResourceManager& manager)
//{
//
//}
