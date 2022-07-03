#include <src/edges/CharacterModel.h>
#include <src/hkx/HkxLinkedTableVariant.h>

using namespace ckcmd::HKX;

hkbCharacterData* CharacterModel::variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		return data;
	}
	return nullptr;
}

hkbCharacterStringData* CharacterModel::string_variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		if (data != nullptr)
		{
			return data->m_stringData;
		}
	}
	return nullptr;
}

hkbMirroredSkeletonInfo* CharacterModel::mirror_variant(const ModelEdge& edge) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
		if (data != nullptr)
		{
			return data->m_mirroredSkeletonInfo;
		}
	}
	return nullptr;
}

hkVariant* CharacterStringData(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
	if (nullptr != data)
	{
		auto string_data = data->m_stringData;
		if (data != NULL)
		{
			int data_index = manager.findIndex(edge.file(), &*string_data);
			return manager.at(edge.file(), data_index);
		}
	}
	return nullptr;
}

hkVariant* CharacterMirrorInfo(const ModelEdge& edge, ResourceManager& manager, hkVariant* variant)
{
	auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
	if (nullptr != data)
	{
		auto mirror_info = data->m_mirroredSkeletonInfo;
		if (mirror_info != NULL)
		{
			int string_data_index = manager.findIndex(edge.file(), &*mirror_info);
			return manager.at(edge.file(), string_data_index);
		}
	}
	return nullptr;
}

std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> CharacterModel::additional_variants() const
{
	return {
		CharacterStringData,
		CharacterMirrorInfo
	};
}

int CharacterModel::supports() const
{
	return 6;
}

const char* CharacterModel::supportName(int support_index) const
{
	switch (support_index)
	{
	case 0:
		return "Skins";
	case 1:
		return "Animations";
	case 2:
		return "Character Properties";
	case 3:
		return "Skeleton";
	case 4:
		return "Ragdoll";
	case 5:
		return "Master Behavior";
	default:
		break;
	}
	return "Invalid Support";
}

NodeType CharacterModel::supportType(int support_index) const
{
	switch (support_index)
	{
	case 0:
		return NodeType::deformableSkinNames;
	case 1:
		return NodeType::animationNames;
	case 2:
		return NodeType::characterPropertyNames;
	case 3:
		return NodeType::SkeletonHkxNode;
	case 4:
		return NodeType::RagdollHkxNode;
	case 5:
		return NodeType::BehaviorHkxNode;
	default:
		break;
	}
	return NodeType::Invalid;
}

int CharacterModel::rows(const ModelEdge& edge, ResourceManager& manager) const
{
	auto string_data = string_variant(edge);
	if (string_data != nullptr)
	{
		if (edge.childType() == NodeType::deformableSkinNames)
		{
			return string_data->m_deformableSkinNames.getSize();
		}
		if (edge.childType() == NodeType::animationNames)
		{
			return string_data->m_animationNames.getSize();
		}
		if (edge.childType() == NodeType::characterPropertyNames)
		{
			return string_data->m_characterPropertyNames.getSize();
		}
		if (edge.childType() == NodeType::deformableSkinName)
		{
			return 0;
		}
		if (edge.childType() == NodeType::animationName)
		{
			return 0;
		}
		if (edge.childType() == NodeType::characterProperty)
		{
			return 0;
		}
		return SupportEnhancedEdge::rows(edge, manager);
	}
	return 0;
}

int CharacterModel::columns(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	if (edge.childType() == NodeType::deformableSkinName)
	{
		return 0;
	}
	if (edge.childType() == NodeType::animationName)
	{
		return 0;
	}
	if (edge.childType() == NodeType::characterProperty)
	{
		return 0;
	}
	return SupportEnhancedEdge::columns(row, edge, manager);
}

int CharacterModel::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	if (
		edge.childType() == NodeType::deformableSkinNames ||
		edge.childType() == NodeType::animationNames ||
		edge.childType() == NodeType::characterPropertyNames ||
		edge.childType() == NodeType::deformableSkinName ||
		edge.childType() == NodeType::animationName ||
		edge.childType() == NodeType::characterProperty
		)
	{
		return rows(edge, manager);
	}
	return supports();
}

std::pair<int, int> CharacterModel::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType())
	{
	case NodeType::deformableSkinNames:
	case NodeType::animationNames:
	case NodeType::characterPropertyNames:
	case NodeType::SkeletonHkxNode:
	case NodeType::RagdollHkxNode:
	case NodeType::BehaviorHkxNode:
		return { index, 0 };
	case NodeType::deformableSkinName:
	case NodeType::animationName:
	case NodeType::characterProperty:
		return { -1, -1 };
	default:
		break;
	}
	return SupportEnhancedEdge::child(index, edge, manager);
}

int CharacterModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType())
	{
	case NodeType::deformableSkinNames:
	case NodeType::animationNames:
	case NodeType::characterPropertyNames:
	case NodeType::SkeletonHkxNode:
	case NodeType::RagdollHkxNode:
	case NodeType::BehaviorHkxNode:
		return row;
	default:
		break;
	}
	return SupportEnhancedEdge::childIndex(row, column, edge, manager);
}

ModelEdge CharacterModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.type())
	{
	case NodeType::deformableSkinNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::deformableSkinName);
	case NodeType::animationNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::animationName);
	case NodeType::characterPropertyNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), NodeType::characterProperty);
	default:
		break;
	}
	auto string_data = string_variant(edge);
	switch (row)
	{
		case 4:
		{
			int rig_index = manager.getRigIndex(edge.project(), string_data);
			auto* rig = manager.getRigRoot(edge.project(), rig_index);
			return ModelEdge(edge, edge.project(), rig_index, row, column, edge.subindex(), rig, NodeType::SkeletonHkxNode);
		}
		case 5:
		{
			if (NULL != string_data->m_ragdollName)
			{
				int ragdoll_index = manager.getRagdollIndex(edge.project(), string_data->m_ragdollName.cString());
				auto* ragdoll = manager.getRagdollRoot(edge.project(), ragdoll_index);
				return ModelEdge(edge, edge.project(), ragdoll_index, row, column, edge.subindex(), ragdoll, NodeType::RagdollHkxNode);
			}
			return ModelEdge(edge, edge.project(), -1, row, column, edge.subindex(), NULL, NodeType::RagdollHkxNode);
		}
		case 6:
		{
			int behavior_index = manager.behaviorFileIndex(edge.project(), edge.childItem<hkVariant>());
			auto* behavior = manager.behaviorFileRoot(behavior_index);
			return ModelEdge(edge, edge.project(), behavior_index, row, column, edge.subindex(), behavior, NodeType::BehaviorHkxNode);
		}
	}
	return SupportEnhancedEdge::child(row, column, edge, manager);
}

QVariant CharacterModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	auto* string_data = string_variant(edge);
	if (edge.childType() == NodeType::deformableSkinNames)
	{
		if (column == 0)
		{
			return supportName(0);
		}
		return "InvalidColumn";
	}
	if (edge.childType() == NodeType::animationNames)
	{
		if (column == 0)
		{
			return supportName(1);
		}
		return "InvalidColumn";
	}
	if (edge.childType() == NodeType::characterPropertyNames)
	{
		if (column == 0)
		{
			return supportName(2);
		}
		return "InvalidColumn";
	}
	if (edge.childType() == NodeType::deformableSkinName)
	{
		if (column == 0)
		{
			return string_data->m_deformableSkinNames[edge.row()].cString();
		}
		return "InvalidColumn";
	}
	if (edge.childType() == NodeType::animationName)
	{
		if (column == 0)
		{
			return string_data->m_animationNames[edge.row()].cString();
		}
		return "InvalidColumn";
	}
	if (edge.childType() == NodeType::characterProperty)
	{
		if (column == 0)
		{
			return string_data->m_characterPropertyNames[edge.row()].cString();
		}
		return "InvalidColumn";
	}
	return SupportEnhancedEdge::data(row, column, edge, manager);
}

//bool CharacterModel::setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager)
//{
//	//auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
//	//auto string_data = data->m_stringData;
//	//auto mirror_data = data->m_mirroredSkeletonInfo;
//	//if (childType == NodeType::animationName)
//	//{
//	//	string_data->m_animationNames[row] = value.toString().toUtf8().constData();
//	//	return true;
//	//}
//
//	//if (row < SUPPORT_END)
//	//	return false;
//	//HkxTableVariant data_variant(*variant);
//	//int data_rows = data_variant.rows();
//	//if (row < SUPPORT_END + data_rows)
//	//	return data_variant.setData(row - SUPPORT_END, column - 1, value);
//	//int string_data_index = manager.findIndex(file, &*string_data);
//	//HkxTableVariant stringdata_variant(*manager.at(file, string_data_index));
//	//int stringdata_rows = stringdata_variant.rows();
//	//if (row < SUPPORT_END + data_rows + stringdata_rows)
//	//	return stringdata_variant.setData(row - data_rows - SUPPORT_END, column - 1, value);
//	//int mirroring_data_index = manager.findIndex(file, &*mirror_data);
//	//HkxTableVariant mirroring_data_variant(*manager.at(file, mirroring_data_index));
//	//int mirroring_data_rows = mirroring_data_variant.rows();
//	//if (row < SUPPORT_END + data_rows + stringdata_rows + mirroring_data_rows)
//	//	return mirroring_data_variant.setData(row - stringdata_rows - data_rows - SUPPORT_END, column - 1, value);
//	return false;
//}
//
//bool CharacterModel::addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
//{
//	//auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
//	//auto string_data = data->m_stringData;
//	//auto mirror_data = data->m_mirroredSkeletonInfo;
//
//	//if (childType == NodeType::animationNames)
//	//{
//	//	return addToContainer(row_start, count, string_data->m_animationNames);
//	//}
//	return false;
//}
//
//bool CharacterModel::removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
//{
//	//auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
//	//auto string_data = data->m_stringData;
//	//auto mirror_data = data->m_mirroredSkeletonInfo;
//
//	//if (childType == NodeType::animationNames)
//	//{
//	//	for (int i = 0; i < count; ++i)
//	//		string_data->m_animationNames.removeAt(row_start);
//	//	return true;
//	//}
//	return false;
//}
//
//bool CharacterModel::changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager)
//{
//	return false;
//}