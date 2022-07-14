#include <src/edges/BehaviorModel.h>
#include <src/hkx/HkxLinkedTableVariant.h>
#include <src/items/HkxItemFlags.h>
#include <src/items/HkxItemPointer.h>
#include <src/items/HkxItemReal.h>

#include <hkbBehaviorGraphData_2.h>

#include <src/utility/Containers.h>

using namespace ckcmd::HKX;

// name
// { "role",&hkbRoleAttributeClass,HK_NULL,hkClassMember::TYPE_STRUCT,hkClassMember::TYPE_VOID,0,hkClassMember::FLAGS_NONE,HK_OFFSET_OF(hkbVariableInfo,m_role) /*0*/,HK_NULL},
//		{ "role",HK_NULL,RoleEnum,hkClassMember::TYPE_ENUM,hkClassMember::TYPE_INT16,0,hkClassMember::FLAGS_NONE,HK_OFFSET_OF(hkbRoleAttribute,m_role) /*0*/,HK_NULL},
//		{ "flags", HK_NULL, RoleFlagsEnum, hkClassMember::TYPE_FLAGS, hkClassMember::TYPE_INT16, 0, hkClassMember::FLAGS_NONE, HK_OFFSET_OF(hkbRoleAttribute, m_flags) /*2*/, HK_NULL},
// { "type", HK_NULL, VariableTypeEnum, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_INT8, 0, hkClassMember::FLAGS_NONE, HK_OFFSET_OF(hkbVariableInfo, m_type) /*4*/, HK_NULL},
// hkArray<hkbVariableValue> m_wordMinVariableValues;
// hkArray<hkbVariableValue> m_wordMaxVariableValues;
//hkRefPtr<hkbVariableValueSet> m_variableInitialValues;

#define VARIABLE_ROLE_INDEX 1
#define VARIABLE_FLAGS_INDEX 2
#define VARIABLE_TYPE_INDEX 3
//#define VARIABLE_MIN_INDEX 4
//#define VARIABLE_MAX_INDEX 5
#define VARIABLE_VALUE_INDEX 4

#define VARIABLE_ROWS 5

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
				return manager.findVariant(edge.file(), &*data);
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
					return manager.findVariant(edge.file(), &*string_data);
				}
			}
		}
	}
	return nullptr;
}

hkVariant* VariablesInitialData(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	if (variant != nullptr)
	{
		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		if (nullptr != graph)
		{
			auto data = graph->m_data;
			if (data != NULL)
			{
				auto variablesInitial_data = data->m_variableInitialValues;
				if (variablesInitial_data != NULL)
				{
					return manager.findVariant(edge.file(), &*variablesInitial_data);
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
		BehaviorStringData,
		VariablesInitialData
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
			return 2; // name, hkbEventInfo.flags
		}
		else if (edge.childType() == NodeType::behaviorVariable)
		{
			return VARIABLE_ROWS;
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
	auto data = data_variant(edge);
	auto string_data = string_variant(edge);
	if (nullptr != variant(edge) && nullptr != data && nullptr != string_data)
	{
		if (edge.childType() == NodeType::behaviorEventName)
		{
			return 2; // name, hkbEventInfo.flags
		}
		else if (edge.childType() == NodeType::behaviorVariable)
		{
			return 2;
		}
		else if (edge.childType() == NodeType::behaviorCharacterProperty)
		{
			switch (row)
			{
			case 0:
				return 1;
			case VARIABLE_ROLE_INDEX:
				return 2;
			case VARIABLE_FLAGS_INDEX:
				return 2;
			case VARIABLE_TYPE_INDEX:
				return 2;
			//case VARIABLE_MIN_INDEX:
			//	return 2;
			//case VARIABLE_MAX_INDEX:
			//	return 2;
			case VARIABLE_VALUE_INDEX:
				switch (data->m_variableInfos[edge.subindex()].m_type)
				{
				case hkbVariableInfo::VARIABLE_TYPE_BOOL:
					return 2;
				case hkbVariableInfo::VARIABLE_TYPE_INT8:
					return 2;
				case hkbVariableInfo::VARIABLE_TYPE_INT16:
					return 2;
				case hkbVariableInfo::VARIABLE_TYPE_INT32:
					return 2;
				case hkbVariableInfo::VARIABLE_TYPE_REAL:
					return 2;
				case hkbVariableInfo::VARIABLE_TYPE_POINTER:
					return 2;
				case hkbVariableInfo::VARIABLE_TYPE_VECTOR3:
					return 4;
				case hkbVariableInfo::VARIABLE_TYPE_VECTOR4:
					return 5;
				case hkbVariableInfo::VARIABLE_TYPE_QUATERNION:
					return 5;
				default:
					break;
				}
				break;
			default:
				break;
			}
			return 0;
		}
		
		if (edge.childType() == NodeType::behaviorEventNames)
			return string_data->m_eventNames.isEmpty() ? 0 : 1;
		if (edge.childType() == NodeType::behaviorVariableNames)
			return string_data->m_variableNames.isEmpty() ? 0 : 1;
		if (edge.childType() == NodeType::behaviorCharacterPropertyNames)
			return string_data->m_characterPropertyNames.isEmpty() ? 0 : 1;
		
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
			edge.childType() == NodeType::behaviorVariableNames ||
			edge.childType() == NodeType::behaviorCharacterPropertyNames
			)
		{
			return rows(edge, manager);
		}
		if (
			edge.childType() == NodeType::behaviorEventName ||
			edge.childType() == NodeType::behaviorVariable ||
			edge.childType() == NodeType::behaviorCharacterProperty
			)
		{
			return 0;
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
	case NodeType::behaviorEventName:
	case NodeType::behaviorVariable:
	case NodeType::behaviorCharacterProperty:
		return MODELEDGE_INVALID;
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
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, row, edge.childItem<hkVariant>(), NodeType::behaviorEventName);
	case NodeType::behaviorVariableNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, row, edge.childItem<hkVariant>(), NodeType::behaviorVariable);
	case NodeType::behaviorCharacterPropertyNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::behaviorCharacterProperty);
	case NodeType::behaviorEventName:
		return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), edge.childItem<hkVariant>(), NodeType::behaviorEventName);
	case NodeType::behaviorVariable:
		return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), edge.childItem<hkVariant>(), NodeType::behaviorVariable);
	case NodeType::behaviorCharacterProperty:
		throw std::runtime_error("Child from leaf node requested!");
	default:
		break;
	}
	return SupportEnhancedEdge::child(row, column, edge, manager);
}

QVariant BehaviorModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	auto* data = data_variant(edge);
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
			if (row == 0 && column == 0)
			{
				return string_data->m_eventNames[edge.row()].cString();
			}
			if (row == 1 && column == 0)
			{
				return "flags";
			}
			if (row == 1 && column == 1)
			{
				auto data = data_variant(edge);
				if (data != nullptr)
				{
					return data->m_eventInfos[edge.subindex()].m_flags.get();
				}
				return "InvalidData";
			}
			return "InvalidColumn";
		}
		if (edge.childType() == NodeType::behaviorVariable)
		{
			if (row == 0 && column == 0)
			{
				return string_data->m_variableNames[edge.row()].cString();
			}
			if (column == 1)
			{
				switch (row)
				{
				case VARIABLE_ROLE_INDEX:
					return (int)data->m_variableInfos[edge.subindex()].m_role.m_role;
				case VARIABLE_FLAGS_INDEX:
					return data->m_variableInfos[edge.subindex()].m_role.m_flags.get();
				case VARIABLE_TYPE_INDEX:
					return (hkInt8)data->m_variableInfos[edge.subindex()].m_type;
				//case VARIABLE_MIN_INDEX:
				//	return data->m_wordMinVariableValues[edge.subindex()].m_value;
				//case VARIABLE_MAX_INDEX:
				//	return data->m_wordMaxVariableValues[edge.subindex()].m_value;
				case VARIABLE_VALUE_INDEX:
					switch (data->m_variableInfos[edge.subindex()].m_type)
					{
					case hkbVariableInfo::VARIABLE_TYPE_BOOL:
					case hkbVariableInfo::VARIABLE_TYPE_INT8:
					case hkbVariableInfo::VARIABLE_TYPE_INT16:
					case hkbVariableInfo::VARIABLE_TYPE_INT32:
						return data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value;
					case hkbVariableInfo::VARIABLE_TYPE_REAL:
					{
						return *(float*)&data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value;
					}
					case hkbVariableInfo::VARIABLE_TYPE_POINTER:
					{
						int value_index = data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value;
						if (value_index == -1)
						{
							value_index = data->m_variableInitialValues->m_variantVariableValues.getSize();
							data->m_variableInitialValues->m_variantVariableValues.expandOne();
							data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value = value_index;
						}
						QVariant value;
						value.setValue(HkxItemPointer((void*)data->m_variableInitialValues->m_variantVariableValues[value_index]));
						return value;
					}
					case hkbVariableInfo::VARIABLE_TYPE_VECTOR3:
					case hkbVariableInfo::VARIABLE_TYPE_VECTOR4:
					case hkbVariableInfo::VARIABLE_TYPE_QUATERNION:
					{
						int value_index = data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value;
						if (value_index == -1)
						{
							value_index = data->m_variableInitialValues->m_quadVariableValues.getSize();
							data->m_variableInitialValues->m_quadVariableValues.expandOne();
							data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value = value_index;
						}
						hkVector4 realValue = data->m_variableInitialValues->m_quadVariableValues[value_index];
						HkxItemReal real_value({ {realValue(0), realValue(1), realValue(2), realValue(3)} });
						QVariant value;
						value.setValue(real_value);
						return value;
					}
					default:
						break;
					}
					break;
				default:
					break;
				}
				return "InvalidRow";
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

bool BehaviorModel::setData(int row, int column, const ModelEdge& edge, const QVariant& value, ResourceManager& manager)
{
	auto data = data_variant(edge);
	auto string_data = string_variant(edge);
	if (nullptr != string_data && nullptr != data)
	{
		if (edge.childType() == NodeType::behaviorEventName)
		{
			if (row == 0 && column == 0)
			{
				string_data->m_eventNames[edge.subindex()] = value.toString().toUtf8().constData();
				return true;
			}
			if (row == 1 && column == 1)
			{
				data->m_eventInfos[edge.subindex()].m_flags.setAll(value.value<int>());
				return true;
			}
			return false;
		}
		if (edge.childType() == NodeType::behaviorVariable)
		{
			if (row == 0 && column == 0)
			{
				string_data->m_variableNames[edge.subindex()] = value.toString().toUtf8().constData();
				return true;
			}
			if (column == 1)
			{
				switch (row)
				{
				case VARIABLE_ROLE_INDEX:
					data->m_variableInfos[edge.subindex()].m_role.m_role = static_cast<hkbRoleAttribute::Role>(value.toInt());
					return true;
				case VARIABLE_FLAGS_INDEX:
					data->m_variableInfos[edge.subindex()].m_role.m_flags.setAll(value.toInt());
					return true;
				case VARIABLE_TYPE_INDEX:
					data->m_variableInfos[edge.subindex()].m_type = static_cast<hkbVariableInfo::VariableType>(value.toInt());
					return true;
				//case VARIABLE_MIN_INDEX:
				//	data->m_wordMinVariableValues[edge.subindex()].m_value = value.toInt();
				//	return true;
				//case VARIABLE_MAX_INDEX:
				//	data->m_wordMaxVariableValues[edge.subindex()].m_value = value.toInt();
				//	return true;
				case VARIABLE_VALUE_INDEX:
					switch (data->m_variableInfos[edge.subindex()].m_type)
					{
					case hkbVariableInfo::VARIABLE_TYPE_BOOL:
					case hkbVariableInfo::VARIABLE_TYPE_INT8:
					case hkbVariableInfo::VARIABLE_TYPE_INT16:
					case hkbVariableInfo::VARIABLE_TYPE_INT32:
						data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value = value.toInt();
						return true;
					case hkbVariableInfo::VARIABLE_TYPE_REAL:
						data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value = (int)value.value<float>();
						return true;
					case hkbVariableInfo::VARIABLE_TYPE_POINTER:
					{
						int value_index = data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value;
						auto ptr = value.value<HkxItemPointer>();
						data->m_variableInitialValues->m_variantVariableValues[value_index] = static_cast<hkReferencedObject*>(ptr.get());
						return true;
					}
					case hkbVariableInfo::VARIABLE_TYPE_VECTOR3:
					case hkbVariableInfo::VARIABLE_TYPE_VECTOR4:
					case hkbVariableInfo::VARIABLE_TYPE_QUATERNION:
					{
						int value_index = data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value;
						auto vector_value = value.value<HkxItemReal>();
						data->m_variableInitialValues->m_quadVariableValues[value_index](column - 1) = vector_value.value(0, column - 1);
						return true;
					}
					default:
						break;
					}
					break;
				default:
					break;
				}			
			}
			return false;
		}
	}
	return SupportEnhancedEdge::setData(row, column, edge, value, manager);
}

bool BehaviorModel::addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	auto string_data = string_variant(edge);
	auto variant_data = data_variant(edge);
	if (nullptr != string_data && nullptr != variant_data)
	{
		if (edge.childType() == NodeType::behaviorEventNames)
		{
			bool result = addToContainer(row_start, count, string_data->m_eventNames);
			result = result && addToContainer(row_start, count, variant_data->m_eventInfos);
			return result;
		}
		if (edge.childType() == NodeType::behaviorVariableNames)
		{
			bool result = addToContainer(row_start, count, string_data->m_variableNames);
			result = result && addToContainer(row_start, count, variant_data->m_variableInfos);
			if (variant_data->m_variableInitialValues == nullptr)
			{
				variant_data->m_variableInitialValues = manager.createObject<hkbVariableValueSet>(edge.file(), &hkbVariableValueSetClass);
			}
			result = result && addToContainer(row_start, count, variant_data->m_variableInitialValues->m_wordVariableValues);
			for (int i = 0; i < count; ++i)
			{
				variant_data->m_variableInitialValues->m_wordVariableValues[row_start + i].m_value = -1;
			}
			return result;
		}
		return false;
	}
	return SupportEnhancedEdge::addRows(row_start, count, edge, manager);
}

bool BehaviorModel::removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	auto string_data = string_variant(edge);
	auto variant_data = data_variant(edge);
	if (nullptr != string_data && nullptr != variant_data)
	{
		if (edge.childType() == NodeType::behaviorEventNames)
		{
			bool result = removeFromContainer(row_start, count, string_data->m_eventNames);
			result = result && removeFromContainer(row_start, count, variant_data->m_eventInfos);
			return result;
		}
		if (edge.childType() == NodeType::behaviorVariableNames)
		{
			bool result = removeFromContainer(row_start, count, string_data->m_variableNames);
			switch (variant_data->m_variableInfos[row_start].m_type)
			{
			case hkbVariableInfo::VARIABLE_TYPE_BOOL:
			case hkbVariableInfo::VARIABLE_TYPE_INT8:
			case hkbVariableInfo::VARIABLE_TYPE_INT16:
			case hkbVariableInfo::VARIABLE_TYPE_INT32:
			case hkbVariableInfo::VARIABLE_TYPE_REAL:
				result = result && removeFromContainer(row_start, count, variant_data->m_variableInitialValues->m_wordVariableValues);
				break;
			case hkbVariableInfo::VARIABLE_TYPE_POINTER:
			{
				int value_index = variant_data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value;
				result = result && removeFromContainer(value_index, count, variant_data->m_variableInitialValues->m_variantVariableValues);
				result = result && removeFromContainer(row_start, count, variant_data->m_variableInitialValues->m_wordVariableValues);
				break;
			}
			case hkbVariableInfo::VARIABLE_TYPE_VECTOR3:
			case hkbVariableInfo::VARIABLE_TYPE_VECTOR4:
			case hkbVariableInfo::VARIABLE_TYPE_QUATERNION:
			{
				int value_index = variant_data->m_variableInitialValues->m_wordVariableValues[edge.subindex()].m_value;
				result = result && removeFromContainer(value_index, count, variant_data->m_variableInitialValues->m_quadVariableValues);
				result = result && removeFromContainer(row_start, count, variant_data->m_variableInitialValues->m_wordVariableValues);
				break;
			}
			default:
				break;
			}
			result = result && removeFromContainer(row_start, count, variant_data->m_variableInfos);
			return result;
		}
		return false;
	}
	return SupportEnhancedEdge::removeRows(row_start, count, edge, manager);
}

bool BehaviorModel::isArray(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	if (edge.childType() == NodeType::behaviorEventNames ||
		edge.childType() == NodeType::behaviorVariableNames ||
		edge.childType() == NodeType::behaviorCharacterPropertyNames)
	{
		return true;
	}
	return SupportEnhancedEdge::isArray(row, edge, manager);
}
