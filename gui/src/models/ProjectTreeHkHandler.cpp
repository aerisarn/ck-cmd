#include <src/models/ProjectTreeHkHandler.h>
#include <src/hkx/HkxLinkedTableVariant.h>

#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <hkbCharacterData_7.h>
#include <hkbBehaviorGraph_1.h>
#include <hkbBehaviorGraphData_2.h>
#include <hkbBehaviorGraphStringData_1.h>
#include <hkbStateMachine_4.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbStateMachineTransitionInfoArray_0.h>
#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
#include <hkbBehaviorReferenceGenerator_0.h>
#include <hkbNode_1.h>


using namespace ckcmd::HKX;

bool isWordVariable(int type)
{
	return type == hkbVariableInfo::VARIABLE_TYPE_BOOL ||
		type == hkbVariableInfo::VARIABLE_TYPE_INT8 ||
		type == hkbVariableInfo::VARIABLE_TYPE_INT16 ||
		type == hkbVariableInfo::VARIABLE_TYPE_INT32 ||
		type == hkbVariableInfo::VARIABLE_TYPE_REAL;
}

bool isQuadVariable(int type)
{
	return type == hkbVariableInfo::VARIABLE_TYPE_VECTOR3 ||
		type == hkbVariableInfo::VARIABLE_TYPE_VECTOR4 ||
		type == hkbVariableInfo::VARIABLE_TYPE_QUATERNION;
}

bool isRefVariable(int type)
{
	return type == hkbVariableInfo::VARIABLE_TYPE_POINTER;
}

template <typename T>
static bool addToContainer(int row_start, int count, hkArray<T>& container)
{
	if (row_start > container.getSize())
		return false;
	if (row_start == container.getSize())
	{
		for (int i = 0; i < count; i++)
			container.pushBack(T());
		return true;
	}
	for (int i = 0; i < count; i++)
		container.insertAt(row_start, T());
	return false;
}

template<typename T>
int countElementsByType(hkArray<T>& container, std::function<bool(int)> typeClassifier)
{
	int count = 0;
	for (int v = 0; v < container.getSize(); ++v)
	{
		if (typeClassifier(container[v].m_type))
			count++;
	}
	return count;
}

template<typename T>
int findIndexByType(int row, hkArray<T>& container, std::function<bool(int)> typeClassifier)
{
	int count = 0;
	for (int v = 0; v <= row; ++v)
	{
		if (typeClassifier(container[v].m_type))
			count++;
	}
	return count;
}


template<typename T>
T& getElementByTypeIndex(int row_index, hkArray<T>& container, const hkArray<hkbVariableInfo>& type_container, std::function<bool(int)> typeClassifier)
{
	int real_row_index = 0;
	int type_row_index = 0;
	for (int v = 0; v < type_container.getSize(); ++v)
	{
		if (typeClassifier(type_container[v].m_type))
		{
			if (type_row_index == row_index)
				return container[real_row_index];
			type_row_index++;
		}
		real_row_index++;
	}
	return T();
}

struct  HandleCharacterData
{
	static const int DATA_SUPPORTS = 3;
	static const int BEHAVIOR_INDEX = DATA_SUPPORTS;
	static const int RIG_INDEX = BEHAVIOR_INDEX + 1;
	static const int RAGDOLL_INDEX = RIG_INDEX + 1;
	static const int SUPPORT_END = RAGDOLL_INDEX + 1;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::deformableSkinNames:
			return "Deformable Skins";
		case NodeType::animationNames:
			return "Animations";
		case NodeType::characterPropertyNames:
			return "Character Properties";
		default:
			break;
		}
		return "Invalid Character Entry";
	};

#undef max

	static ModelEdge get_child(int index, int column, int subindex, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		if (childType == NodeType::CharacterHkxNode)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::deformableSkinNames);
			case 1:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::animationNames);
			case 2:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::characterPropertyNames);
			default:
				break;
			}
			if (index >= DATA_SUPPORTS)
			{
				auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
				auto string_data = data->m_stringData;

				int behavior_index = manager.behaviorFileIndex(project, variant);
				hkVariant* root = manager.behaviorFileRoot(behavior_index);
				int rig_index = manager.getRigIndex(project, string_data);
				hkVariant* rig_root = manager.getRigRoot(project, rig_index);
				int ragdoll_index = -1;
				if (string_data->m_ragdollName.getLength() > 0)
					ragdoll_index = manager.getRagdollIndex(project, string_data->m_ragdollName.cString());
				else
					ragdoll_index = manager.getRagdollIndex(project, string_data->m_rigName.cString());
				hkVariant* ragdoll_root = manager.getRagdollRoot(project, ragdoll_index);

				if (index == BEHAVIOR_INDEX)
				{
					if (root != NULL)
						return ModelEdge(variant, project, behavior_index, index, 0, root, NodeType::BehaviorHkxNode);

					if (rig_root != NULL)
						return ModelEdge(variant, project, rig_index, 0, 0, rig_root, NodeType::SkeletonHkxNode);

					return ModelEdge(variant, project, ragdoll_index, 0, 0, ragdoll_root, NodeType::RagdollHkxNode);
				}
				else if (index == RIG_INDEX)
				{
					if (rig_root != NULL && root != NULL)
						return ModelEdge(variant, project, rig_index, 0, 0, rig_root, NodeType::SkeletonHkxNode);

					return ModelEdge(variant, project, ragdoll_index, 0, 0, ragdoll_root, NodeType::RagdollHkxNode);
				}
				return ModelEdge(variant, project, ragdoll_index, 0, 0, ragdoll_root, NodeType::RagdollHkxNode);
			}
			return ModelEdge();
		}
		else if (childType == NodeType::deformableSkinNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::deformableSkinName);
		}
		else if (childType == NodeType::animationNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::animationName);
		}
		else if (childType == NodeType::characterPropertyNames)
		{
			return ModelEdge(variant, project, file, index, column, index, variant, NodeType::characterProperty);
		}
		else if (childType == NodeType::characterProperty)
		{
			return ModelEdge(variant, project, file, index, column, subindex, variant, NodeType::characterProperty);
		}
		return ModelEdge();
	}

	static const int getColumns(int row, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		if (data == NULL)
			return 1;
		auto string_data = data->m_stringData;
		if (string_data == NULL)
			return 0;
		auto mirror_data = data->m_mirroredSkeletonInfo;
		if (mirror_data == NULL)
			return 0;
		if (childType == NodeType::deformableSkinNames)
		{
			return 1;
		}
		else if (childType == NodeType::animationNames)
		{
			return 1;
		}
		else if (childType == NodeType::characterPropertyNames)
		{
			return 1;
		}
		else if (childType == NodeType::characterProperty)
		{

			// 3 x 1
			//hkbVariableInfo.m_role.m_role
			//hkbVariableInfo.m_role.m_type 
			//hkbVariableInfo.m_type

			auto type = data->m_characterPropertyInfos[row].m_type;
			if (isWordVariable(type))
			{
				return 1;
			}
			else if (isQuadVariable(type))
			{
				return 4;
			}
			
			//ref, must be calculated
			int ref_index = data->m_characterPropertyValues->m_wordVariableValues[row].m_value;
			auto* value = data->m_characterPropertyValues->m_variantVariableValues[ref_index];
			int value_index = manager.findIndex(file, &*value);
			auto* value_variant = manager.at(file, value_index);
			return HkxTableVariant(*value_variant).columns();
			
		}

		int string_data_index = manager.findIndex(file, &*string_data);
		int mirror_data_index = manager.findIndex(file, &*mirror_data);
		int additional = std::max({
			HkxTableVariant(*variant).columns(),
			HkxTableVariant(*manager.at(file, string_data_index)).columns(),
			HkxTableVariant(*manager.at(file, mirror_data_index)).columns()
		});
		return 1 + additional;
	}

	static int getChildCount(int row, int project, hkVariant* variant, NodeType childType, ResourceManager& manager)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		if (data == NULL)
			return 0;
		auto string_data = data->m_stringData;
		if (string_data == NULL)
			return 0;
		if (childType == NodeType::CharacterHkxNode)
		{
			int count = DATA_SUPPORTS;
			count += manager.hasBehavior(project, string_data);
			count += manager.hasRigAndRagdoll(project, string_data);
			if (row < count)
				return count;
			return 0;
		}
		else if (childType == NodeType::deformableSkinNames)
		{
			return string_data->m_deformableSkinNames.getSize();
		}
		else if (childType == NodeType::animationNames)
		{
			return string_data->m_animationNames.getSize();
		}
		else if (childType == NodeType::characterPropertyNames)
		{
			return string_data->m_characterPropertyNames.getSize();
		}
		else if (childType == NodeType::characterProperty)
		{
			return 0;
		}
		return 0;
	}

	static const int getRows(int project, int file, int row, int column, hkVariant* variant, NodeType childType, ResourceManager& manager)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		if (data == NULL)
			return 1;
		auto string_data = data->m_stringData;
		if (string_data == NULL)
			return 0;
		auto mirror_data = data->m_mirroredSkeletonInfo;
		if (mirror_data == NULL)
			return 0;
		if (childType == NodeType::deformableSkinNames)
		{
			if (column == 0)
			{
				return getChildCount(row, project, variant, childType, manager);
			}
			return 0;
		}
		else if (childType == NodeType::animationNames)
		{
			if (column == 0)
			{
				return getChildCount(row, project, variant, childType, manager);
			}
			return 0;
		}
		else if (childType == NodeType::characterPropertyNames)
		{
			if (column == 0)
			{
				return getChildCount(row, project, variant, childType, manager);
			}
			return 0;
		}
		else if (childType == NodeType::characterProperty)
		{
			// 3 x 1
			int rows = 4;
			//name
			//hkbVariableInfo.m_role.m_role
			//hkbVariableInfo.m_role.m_type 
			//hkbVariableInfo.m_type

			auto type = data->m_characterPropertyInfos[row].m_type;
			if (isWordVariable(type))
			{
				return rows + 1;
			}
			else if (isQuadVariable(type))
			{
				return rows + 1;
			}

			//ref, must be calculated
			int ref_index = data->m_characterPropertyValues->m_wordVariableValues[row].m_value;
			auto* value = data->m_characterPropertyValues->m_variantVariableValues[ref_index];
			int value_index = manager.findIndex(file, &*value);
			auto* value_variant = manager.at(file, value_index);
			return rows + HkxTableVariant(*value_variant).rows();
		}

		int string_data_index = manager.findIndex(file, &*string_data);
		int mirror_data_index = manager.findIndex(file, &*mirror_data);
		return SUPPORT_END +
			HkxTableVariant(*variant).rows() +
			HkxTableVariant(*manager.at(file, string_data_index)).rows() +
			HkxTableVariant(*manager.at(file, mirror_data_index)).rows();
	}

	static int getRowColumns(int file, int row, int column, hkVariant* variant, NodeType childType, ResourceManager& manager)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		if (data == NULL)
			return 0;
		auto string_data = data->m_stringData;
		if (string_data == NULL)
			return 0;
		auto mirror_data = data->m_mirroredSkeletonInfo;
		if (mirror_data == NULL)
			return 0;
		if (childType == NodeType::CharacterHkxNode)
		{
			if (row < SUPPORT_END)
				return 1;
			HkxTableVariant data_variant(*variant);
			int data_rows = data_variant.rows();
			if (row < SUPPORT_END + data_rows)
				return data_variant.columns(row - SUPPORT_END);
			int string_data_index = manager.findIndex(file, &*string_data);
			HkxTableVariant stringdata_variant(*manager.at(file, string_data_index));
			int stringdata_rows = stringdata_variant.rows();
			if (row < SUPPORT_END + data_rows + stringdata_rows)
				return stringdata_variant.columns(row - data_rows - SUPPORT_END);
			int mirroring_data_index = manager.findIndex(file, &*mirror_data);
			HkxTableVariant mirroring_data_variant(*manager.at(file, mirroring_data_index));
			int mirroring_data_rows = mirroring_data_variant.rows();
			if (row < SUPPORT_END + data_rows + stringdata_rows + mirroring_data_rows)
				return mirroring_data_variant.columns(row - stringdata_rows - data_rows - SUPPORT_END);
		}
		else if (childType == NodeType::characterProperty) {
			return getColumns(row, file, variant, manager, childType);
		}
		return 0;
	}

	static QVariant data(int file, int row, int column, int subindex, hkVariant* variant, NodeType childType, ResourceManager& manager)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		auto string_data = data->m_stringData;
		auto mirror_data = data->m_mirroredSkeletonInfo;
		if (column == 0)
		{
			if (childType == NodeType::CharacterHkxNode) {
				if (row == 0)
					return string_data->m_name.cString();
				if (row < SUPPORT_END)
					return QVariant();
				HkxTableVariant data_variant(*variant);
				int data_rows = data_variant.rows();
				if (row < SUPPORT_END + data_rows)
					return data_variant.rowName(row - SUPPORT_END);
				int string_data_index = manager.findIndex(file, &*string_data);
				HkxTableVariant stringdata_variant(*manager.at(file, string_data_index));
				int stringdata_rows = stringdata_variant.rows();
				if (row < SUPPORT_END + data_rows + stringdata_rows)
					return stringdata_variant.rowName(row - data_rows - SUPPORT_END);
				int mirroring_data_index = manager.findIndex(file, &*mirror_data);
				HkxTableVariant mirroring_data_variant(*manager.at(file, mirroring_data_index));
				int mirroring_data_rows = mirroring_data_variant.rows();
				if (row < SUPPORT_END + data_rows + stringdata_rows + mirroring_data_rows)
					return mirroring_data_variant.rowName(row - stringdata_rows - data_rows - SUPPORT_END);
				return QVariant();
			}
			if (childType == NodeType::deformableSkinNames ||
				childType == NodeType::animationNames ||
				childType == NodeType::characterPropertyNames)
			{
				return DataListsName((int)childType);
			}
			if (childType == NodeType::deformableSkinName)
			{
				return string_data->m_deformableSkinNames[row].cString();
			}
			if (childType == NodeType::animationName)
			{
				return string_data->m_animationNames[row].cString();
			}
			if (childType == NodeType::characterProperty)
			{
				if (column == 0)
				{
					return string_data->m_characterPropertyNames[subindex].cString();
				}
				if (column == 1 && row < 4)
				{
					hkVariant fake_variant; 
					fake_variant.m_class = &hkbVariableInfoClass;
					fake_variant.m_object = &data->m_characterPropertyInfos[subindex];
					return HkxTableVariant(fake_variant).data(row - 1, column - 1);
				}
				else {
					auto variable_type = data->m_characterPropertyInfos[subindex];
					//if (isWordVariable())
				}


				return "InvalidColumn";
			}
			return QVariant();
		}
		if (row < SUPPORT_END)
			return QVariant();
		HkxTableVariant data_variant(*variant);
		int data_rows = data_variant.rows();
		if (row < SUPPORT_END + data_rows)
			return data_variant.data(row - SUPPORT_END, column - 1);
		int string_data_index = manager.findIndex(file, &*string_data);
		HkxTableVariant stringdata_variant(*manager.at(file, string_data_index));
		int stringdata_rows = stringdata_variant.rows();
		if (row < SUPPORT_END + data_rows + stringdata_rows)
			return stringdata_variant.data(row - data_rows - SUPPORT_END, column - 1);
		int mirroring_data_index = manager.findIndex(file, &*mirror_data);
		HkxTableVariant mirroring_data_variant(*manager.at(file, mirroring_data_index));
		int mirroring_data_rows = mirroring_data_variant.rows();
		if (row < SUPPORT_END + data_rows + stringdata_rows + mirroring_data_rows)
			return mirroring_data_variant.data(row - stringdata_rows - data_rows - SUPPORT_END, column - 1);
		return QVariant();
	}

	static bool setData(int row, int column, int project, int file, hkVariant* variant, NodeType childType, const QVariant& value, ResourceManager& manager)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		auto string_data = data->m_stringData;
		auto mirror_data = data->m_mirroredSkeletonInfo;
		if (childType == NodeType::animationName)
		{
			string_data->m_animationNames[row] = value.toString().toUtf8().constData();
			return true;
		}

		if (row < SUPPORT_END)
			return false;
		HkxTableVariant data_variant(*variant);
		int data_rows = data_variant.rows();
		if (row < SUPPORT_END + data_rows)
			return data_variant.setData(row - SUPPORT_END, column - 1, value);
		int string_data_index = manager.findIndex(file, &*string_data);
		HkxTableVariant stringdata_variant(*manager.at(file, string_data_index));
		int stringdata_rows = stringdata_variant.rows();
		if (row < SUPPORT_END + data_rows + stringdata_rows)
			return stringdata_variant.setData(row - data_rows - SUPPORT_END, column - 1, value);
		int mirroring_data_index = manager.findIndex(file, &*mirror_data);
		HkxTableVariant mirroring_data_variant(*manager.at(file, mirroring_data_index));
		int mirroring_data_rows = mirroring_data_variant.rows();
		if (row < SUPPORT_END + data_rows + stringdata_rows + mirroring_data_rows)
			return mirroring_data_variant.setData(row - stringdata_rows - data_rows - SUPPORT_END, column - 1, value);
		return false;
	}

	static bool addRows(int row_start, int count, int project, int file, hkVariant* variant, NodeType childType, ResourceManager& manager)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		auto string_data = data->m_stringData;
		auto mirror_data = data->m_mirroredSkeletonInfo;

		if (childType == NodeType::animationNames)
		{
			return addToContainer(row_start, count, string_data->m_animationNames);
		}
		return false;
	}

	static bool removeRows(int row_start, int count, int project, int file, hkVariant* variant, NodeType childType, ResourceManager& manager)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		auto string_data = data->m_stringData;
		auto mirror_data = data->m_mirroredSkeletonInfo;

		if (childType == NodeType::animationNames)
		{
			for (int i=0; i< count; ++i)
				string_data->m_animationNames.removeAt(row_start);
			return true;
		}
		return false;
	}
};

struct  HandleBehaviorData
{

	static const size_t DATA_SUPPORTS = 3;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::behaviorEventNames:
			return "Events";
		case NodeType::behaviorVariableNames:
			return "Variables";
		case NodeType::behaviorCharacterPropertyNames:
			return "Properties";
		default:
			break;
		}
		return "Invalid Behavior Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* graph = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		if (graph == NULL)
			return 0;
		auto data = graph->m_data;
		if (data == NULL)
			return 0;
		auto string_data = data->m_stringData;
		if (string_data == NULL)
			return 0;
		if (childType == NodeType::BehaviorHkxNode)
		{
			return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).links().size();
		}
		else if (childType == NodeType::behaviorEventNames)
		{
			return string_data->m_eventNames.getSize();
		}
		else if (childType == NodeType::behaviorVariableNames)
		{
			return string_data->m_variableNames.getSize();
		}
		else if (childType == NodeType::behaviorCharacterPropertyNames)
		{
			return string_data->m_characterPropertyNames.getSize();
		}
		else if (childType == NodeType::behaviorEventName)
		{
			return 0;
		}
		else if (childType == NodeType::behaviorVariable)
		{
			return 0;
		}
		/*Character Properties*/

		else if (childType == NodeType::behaviorCharacterProperty)
		{
			return 0;
		}
		return HkxLinkedTableVariant(*variant).links().size();
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		auto string_data = data->m_data->m_stringData;
		if (childType == NodeType::behaviorEventNames ||
			childType == NodeType::behaviorVariableNames ||
			childType == NodeType::behaviorCharacterPropertyNames)
		{
			return DataListsName((int)childType);
		}
		if (childType == NodeType::behaviorEventName)
		{
			if (column == 0)
				return string_data->m_eventNames[row].cString();
			return "InvalidColumn";
		}
		if (childType == NodeType::behaviorVariable)
		{
			if (column == 0)
			{
				return string_data->m_variableNames[row].cString();
			}
			return "InvalidColumn";
		}
		if (childType == NodeType::behaviorCharacterProperty)
		{
			if (column == 0)
			{
				return string_data->m_characterPropertyNames[row].cString();
			}
			return "InvalidColumn";
		}
		if (column == 0)
		{
			return HkxVariant(*variant).name();
		}
		HkxLinkedTableVariant v(*variant);
		return v.data(row - DATA_SUPPORTS, column - 1);
	}

	static bool setData(int row, int column, int project, int file, hkVariant* variant, NodeType childType, const QVariant& value, ResourceManager& manager)
	{
		auto* data = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		auto string_data = data->m_data->m_stringData;
		if (childType == NodeType::behaviorEventName)
		{
			string_data->m_eventNames[row] = value.toString().toUtf8().constData();
			return true;
		}
		//else if (childType == NodeType::behaviorVariableName)
		//{
		//	string_data->m_variableNames[row] = value.toString().toUtf8().constData();
		//}
		//else if (childType == NodeType::characterPropertyName)
		//{
		//	string_data->m_characterPropertyNames[row] = value.toString().toUtf8().constData();
		//}
		return false;
	}

	static bool addRows(int row_start, int count, int project, int file, hkVariant* variant, NodeType childType, ResourceManager& manager)
	{
		auto* data = reinterpret_cast<hkbBehaviorGraph*>(variant->m_object);
		auto string_data = data->m_data->m_stringData;
		if (childType == NodeType::behaviorEventNames)
		{
			bool result = addToContainer(row_start, count, string_data->m_eventNames);
			if (!result)
				return false;
			result = addToContainer(row_start, count, data->m_data->m_eventInfos);
			if (!result)
				return false;
			return true;
		}
		if (childType == NodeType::behaviorCharacterProperty)
		{
			bool result = addToContainer(row_start, count, string_data->m_variableNames);
			if (!result)
				return false;
			result = addToContainer(row_start, count, data->m_data->m_variableInfos);
			if (HK_NULL == data->m_data->m_variableInitialValues)
			{
				data->m_data->m_variableInitialValues = manager.createObject<hkbVariableValueSet>(file, &hkbVariableValueSetClass);
			}
			result = addToContainer(row_start, count, data->m_data->m_variableInitialValues->m_wordVariableValues);
			if (!result)
				return false;
			return true;
		}
		//else if (childType == NodeType::behaviorVariableNames)
		//{
		//	addToContainer(row_start, count, string_data->m_variableNames);
		//	data->m_data->m_variableInitialValues->m
		//	//string_data->m_variableNames[row] = value.toString().toUtf8().constData();
		//}
		//else if (childType == NodeType::characterPropertyName)
		//{
		//	//string_data->m_characterPropertyNames[row] = value.toString().toUtf8().constData();
		//}
		return false;
	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		if (childType == NodeType::BehaviorHkxNode)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorEventNames);
			case 1:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorVariableNames);
			case 2:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorCharacterPropertyNames);
			default:
				break;
			}
		}
		else if (childType == NodeType::behaviorEventNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorEventName);
		}
		else if (childType == NodeType::behaviorVariableNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorVariable);
		}
		else if (childType == NodeType::behaviorCharacterPropertyNames)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::behaviorCharacterProperty);
		}
		HkxLinkedTableVariant v(*variant);
		auto& links = v.links();
		auto& link = links.at(index - DATA_SUPPORTS);
		int file_index = manager.findIndex(file, link._ref);
		if (file_index == -1)
			__debugbreak();
		return ModelEdge((hkVariant*)nullptr, project, file, link._row, link._column, manager.at(file, file_index));
	}
};

struct  HandleStateMachineData
{

	static const size_t DATA_SUPPORTS = 2;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::FSMWildcardTransitions:
			return "Wildcard Transitions";
		case NodeType::FSMStateTransitions:
			return "Transitions";
		default:
			break;
		}
		return "Invalid FSM Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
		if (FSM == NULL)
			return 0;

		if (childType == NodeType::HavokNative)
		{
			return DATA_SUPPORTS + HkxLinkedTableVariant(*variant).links().size();
		}
		else if (childType == NodeType::FSMWildcardTransitions)
		{
			if (FSM->m_wildcardTransitions == NULL)
				return 0;
			return FSM->m_wildcardTransitions->m_transitions.getSize();
		}
		else if (childType == NodeType::FSMWildcardTransition)
		{
			return 0;
		}
		else if (childType == NodeType::FSMStateTransitions)
		{
			int count = 0;
			for (int i = 0; i < FSM->m_states.getSize(); i++)
			{
				if (NULL != FSM->m_states[i]->m_transitions)
					count += FSM->m_states[i]->m_transitions->m_transitions.getSize();
			}
			return count;
		}
		else if (childType == NodeType::FSMStateTransition)
		{
			return 0;
		}
		
		return HkxLinkedTableVariant(*variant).links().size();
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* FSM = reinterpret_cast<hkbStateMachine*>(variant->m_object);
		if (FSM == NULL)
			return 0;

		if (childType == NodeType::FSMWildcardTransitions || childType == NodeType::FSMStateTransitions)
		{
			return DataListsName((int)childType);
		}
		else if (childType == NodeType::FSMWildcardTransition)
		{
			auto& transition = FSM->m_wildcardTransitions->m_transitions[row];
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
		else if (childType == NodeType::FSMStateTransition)
		{
			int index = row;
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
		if (column == 0)
		{
			return HkxVariant(*variant).name();
		}
		HkxLinkedTableVariant v(*variant);
		if (FSM->m_wildcardTransitions != NULL)
			return v.data(row-DATA_SUPPORTS, column - 1);
		return v.data(row, column - 1);
	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		if (childType == NodeType::HavokNative)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::FSMWildcardTransitions);
			case 1:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::FSMStateTransitions);
			default:
				break;
			}
		}
		else if (childType == NodeType::FSMWildcardTransitions)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::FSMWildcardTransition);
		}
		else if (childType == NodeType::FSMStateTransitions)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::FSMStateTransition);
		}
		HkxLinkedTableVariant v(*variant);
		auto& links = v.links();
		auto& link = links.at(index - DATA_SUPPORTS);
		int file_index = manager.findIndex(file, link._ref);
		if (file_index == -1)
			__debugbreak();
		return ModelEdge((hkVariant*)nullptr, project, file, link._row, link._column, manager.at(file, file_index));
	}
};

struct  HandleSkeletonData
{
	static const size_t DATA_SUPPORTS = 2;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::SkeletonBones:
			return "Bones";
		case NodeType::SkeletonFloats:
			return "Floats";
		default:
			break;
		}
		return "Invalid Character Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkaSkeleton*>(variant->m_object);
		if (data == NULL)
			return 0;
		if (childType == NodeType::SkeletonHkxNode)
		{
			return DATA_SUPPORTS;
		}
		else if (childType == NodeType::SkeletonBones)
		{
			return data->m_bones.getSize();
		}
		else if (childType == NodeType::SkeletonFloats)
		{
			return data->m_floatSlots.getSize();
		}
		else {
			return 0;
		}
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkaSkeleton*>(variant->m_object);
		if (childType == NodeType::SkeletonBones ||
			childType == NodeType::SkeletonFloats)
		{
			return DataListsName((int)childType);
		}
		else if (childType == NodeType::SkeletonBone)
		{
			return data->m_bones[row].m_name.cString();
		}
		else if (childType == NodeType::SkeletonFloat)
		{
			return data->m_floatSlots[row].cString();
		}
		else if (childType == NodeType::SkeletonHkxNode) {
			return "Skeleton";
		}
		return 0;

	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		if (childType == NodeType::SkeletonHkxNode)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonBones);
			case 1:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonFloats);
			default:
				break;
			}
			return ModelEdge();
		}
		else if (childType == NodeType::SkeletonBones)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonBone);
		}
		else if (childType == NodeType::SkeletonFloats)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::SkeletonFloat);
		}
		else {
			return ModelEdge();
		}
	}
};

struct  HandleRagdollData
{
	static const size_t DATA_SUPPORTS = 1;

	static const char* DataListsName(int row)
	{
		switch ((NodeType)row) {
		case NodeType::RagdollBones:
			return "Bones";
		default:
			break;
		}
		return "Invalid Character Entry";
	};

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkaRagdollInstance*>(variant->m_object);
		if (data == NULL)
			return 0;
		if (childType == NodeType::RagdollHkxNode)
		{
			return DATA_SUPPORTS;
		}
		else if (childType == NodeType::RagdollBones)
		{
			if (NULL == data->m_skeleton)
				return 0;
			return data->m_skeleton->m_bones.getSize();
		}
		else {
			return 0;
		}
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		auto* data = reinterpret_cast<hkaRagdollInstance*>(variant->m_object);
		if (childType == NodeType::RagdollBones)
		{
			return DataListsName((int)childType);
		}
		else if (childType == NodeType::RagdollBone)
		{
			return data->m_skeleton->m_bones[row].m_name.cString();
		}
		else if (childType == NodeType::RagdollHkxNode)
		{
			return "Ragdoll";
		}
		else {
			return 0;
		}
	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		if (childType == NodeType::RagdollHkxNode)
		{
			switch (index) {
			case 0:
				return ModelEdge(variant, project, file, index, 0, variant, NodeType::RagdollBones);
			default:
				break;
			}
			return ModelEdge();
		}
		else if (childType == NodeType::RagdollBones)
		{
			return ModelEdge(variant, project, file, index, 0, variant, NodeType::RagdollBone);
		}
		else {
			return ModelEdge();
		}
	}
};

struct  HandleBehaviorReference {

	static int getChildCount(hkVariant* variant, NodeType childType)
	{
		return 1;
	}

	static QVariant data(int row, int column, hkVariant* variant, NodeType childType)
	{
		hkbBehaviorReferenceGenerator* bfg = reinterpret_cast<hkbBehaviorReferenceGenerator*>(variant->m_object);
		return bfg->m_behaviorName.cString();
	}

	static ModelEdge get_child(int index, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
	{
		int behavior_file = manager.behaviorFileIndex(project, variant);
		hkVariant* root = manager.behaviorFileRoot(behavior_file);
		return ModelEdge(variant, project, behavior_file, index, 0, root, NodeType::BehaviorHkxNode);
	}

};

int ProjectTreeHkHandler::getChildCount(int project, hkVariant* variant, NodeType childType, ResourceManager& manager)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::getChildCount(0, project, variant, childType, manager);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::getChildCount(variant, childType);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return HandleStateMachineData::getChildCount(variant, childType);
	}
	if (&hkaSkeletonClass == variant->m_class)
	{
		return HandleSkeletonData::getChildCount(variant, childType);
	}
	if (&hkaRagdollInstanceClass == variant->m_class)
	{
		return HandleRagdollData::getChildCount(variant, childType);
	}
	if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	{
		return HandleBehaviorReference::getChildCount(variant, childType);
	}
	return HkxLinkedTableVariant(*variant).links().size();
}

int ProjectTreeHkHandler::childRows(int project, int file, int row, int column, hkVariant* variant, NodeType childType, ResourceManager& manager)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::getRows(project, file, row, column, variant, childType, manager);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::getChildCount(variant, childType);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return HandleStateMachineData::getChildCount(variant, childType);
	}
	if (&hkaSkeletonClass == variant->m_class)
	{
		return HandleSkeletonData::getChildCount(variant, childType);
	}
	if (&hkaRagdollInstanceClass == variant->m_class)
	{
		return HandleRagdollData::getChildCount(variant, childType);
	}
	if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	{
		return HandleBehaviorReference::getChildCount(variant, childType);
	}

	HkxLinkedTableVariant v(*variant);
	auto& links = v.links();
	for (const auto& link : links)
	{
		if (link._row == row && link._column == column)
		{
			int file_index = manager.findIndex(file, link._ref);
			if (file_index == -1)
				__debugbreak();
			return HkxTableVariant(*manager.at(file, file_index)).rows();
		}
	}
	return 0;
}

int ProjectTreeHkHandler::childRowColumns(int project, int file, int row, int column, hkVariant* variant, NodeType childType, ResourceManager& manager)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::getRowColumns(file, row, column, variant, childType, manager);
	}
	return HkxTableVariant(*variant).columns(row);;
}

int ProjectTreeHkHandler::childColumns(int project, int file, int row, int column, hkVariant* variant, NodeType childType, ResourceManager& manager)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::getColumns(row, file, variant, manager, childType);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return 1;
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return 1;
	}
	if (&hkaSkeletonClass == variant->m_class)
	{
		return 1;
	}
	if (&hkaRagdollInstanceClass == variant->m_class)
	{
		return 1;
	}
	if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	{
		return 1;
	}
	HkxLinkedTableVariant v(*variant);
	auto& links = v.links();
	for (const auto& link : links)
	{
		if (link._row == row && link._column == column)
		{
			int file_index = manager.findIndex(file, link._ref);
			if (file_index == -1)
				__debugbreak();
			return HkxTableVariant(*manager.at(file, file_index)).columns();
		}
	}
	return 0;
}

QVariant ProjectTreeHkHandler::data(int file, int row, int column, int subindex, hkVariant* variant, NodeType childType, ResourceManager& manager)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::data(file, row, column, subindex, variant, childType, manager);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::data(row, column, variant, childType);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return HandleStateMachineData::data(row, column, variant, childType);
	}
	if (&hkaSkeletonClass == variant->m_class)
	{
		return HandleSkeletonData::data(row, column, variant, childType);
	}
	if (&hkaRagdollInstanceClass == variant->m_class)
	{
		return HandleRagdollData::data(row, column, variant, childType);
	}
	if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	{
		return HandleBehaviorReference::data(row, column, variant, childType);
	}
	if (column == 0)
	{
		return HkxVariant(*variant).name();
	}
	HkxLinkedTableVariant v(*variant);
	return v.data(row, column - 1);
}

bool ProjectTreeHkHandler::hasChild(hkVariant*, int row, int column, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::getChildCount(row, project, variant, childType, manager) > 0;
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return getChildCount(project, variant, childType, manager) > 0;
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return getChildCount(project, variant, childType, manager) > 0;
	}
	if (&hkaSkeletonClass == variant->m_class)
	{
		return getChildCount(project, variant, childType, manager) > 0;
	}
	if (&hkaRagdollInstanceClass == variant->m_class)
	{
		return getChildCount(project, variant, childType, manager) > 0;
	}
	if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	{
		return getChildCount(project, variant, childType, manager) > 0;
	}
	if (column == 0)
	{
		return getChildCount(project, variant, childType, manager) > 0;
	}
	HkxLinkedTableVariant v(*variant);
	auto& links = v.links();
	for (const auto& link : links)
	{
		if (link._row == row && link._column == column)
		{
			return true;
		}
	}
	return false;
}

ModelEdge ProjectTreeHkHandler::getChild(hkVariant*, int row, int column, int subindex, int project, int file, hkVariant* variant, ResourceManager& manager, NodeType childType)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::get_child(row, column, subindex, project, file, variant, manager, childType);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::get_child(row, project, file, variant, manager, childType);
	}
	if (&hkbStateMachineClass == variant->m_class)
	{
		return HandleStateMachineData::get_child(row, project, file, variant, manager, childType);
	}
	if (&hkaSkeletonClass == variant->m_class)
	{
		return HandleSkeletonData::get_child(row, project, file, variant, manager, childType);
	}
	if (&hkaRagdollInstanceClass == variant->m_class)
	{
		return HandleRagdollData::get_child(row, project, file, variant, manager, childType);
	}
	if (&hkbBehaviorReferenceGeneratorClass == variant->m_class)
	{
		return HandleBehaviorReference::get_child(row, project, file, variant, manager, childType);
	}
	HkxLinkedTableVariant v(*variant);
	auto& links = v.links();
	for (const auto& link : links)
	{
		if (link._row == row && link._column == column)
		{
			int file_index = manager.findIndex(file, link._ref);
			if (file_index == -1)
				__debugbreak();
			return ModelEdge((hkVariant*)nullptr, project, file, link._row, link._column, manager.at(file, file_index));
		}
	}
	__debugbreak();
	return ModelEdge();
}

bool ProjectTreeHkHandler::setData(int row, int column, int project, int file, hkVariant* variant, NodeType childType, const QVariant& value, ResourceManager& manager)
{
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::setData(row, column, project, file, variant, childType, value, manager);
	}
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::setData(row, column, project, file, variant, childType, value, manager);
	}
	if (column == 0)
	{
		//setting name
		return HkxVariant(*variant).setName(value.toString());
	}
	return false;
}

bool ProjectTreeHkHandler::addRows(int row_start, int count, int project, int file, hkVariant* variant, NodeType childType, ResourceManager& manager)
{
	if (&hkbBehaviorGraphClass == variant->m_class)
	{
		return HandleBehaviorData::addRows(row_start, count, project, file, variant, childType, manager);
	}
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::addRows(row_start, count, project, file, variant, childType, manager);
	}
	return false;
}

bool ProjectTreeHkHandler::removeRows(int row_start, int count, int project, int file, hkVariant* variant, NodeType childType, ResourceManager& manager)
{
	//if (&hkbBehaviorGraphClass == variant->m_class)
	//{
	//	return HandleBehaviorData::addRows(row_start, count, project, file, variant, childType, manager);
	//}
	if (&hkbCharacterDataClass == variant->m_class)
	{
		return HandleCharacterData::removeRows(row_start, count, project, file, variant, childType, manager);
	}
	return false;
}

bool ProjectTreeHkHandler::changeColumns(int row, int column_start, int delta, int _project, int _file, hkVariant* variant, NodeType childType, ResourceManager& manager)
{
	return HkxTableVariant(*variant).resizeColumns(row, column_start, delta);
}