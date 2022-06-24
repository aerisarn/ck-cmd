#include <src/edges/CharacterModel.h>
#include <src/hkx/HkxLinkedTableVariant.h>

using namespace ckcmd::HKX;

const std::set<QString> CharacterModel::children_rows
{ 
	"deformableSkinNames",
	"animationNames",
	"characterPropertyNames",
	"rigName",
	"ragdollName",
	"behaviorFilename"
};

int variantChildRowColumns(int file, int row, int column, hkVariant* variant, ResourceManager& manager, bool rows)
{
	auto links = HkxLinkedTableVariant(*variant).links();
	for (const auto& link : links)
	{
		if (link._ref != nullptr && link._row == row && link._column == column)
		{
			auto child_index = manager.findIndex(file, link._ref);
			if (child_index == HK_INVALID_REF)
				throw std::runtime_error("Invalid reference");
			if (rows)
				return HkxTableVariant(*manager.at(file, child_index)).rows();
			else
				return HkxTableVariant(*manager.at(file, child_index)).columns();
		}
	}
	return 0;
}

int CharacterModel::childRows(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
	if (data == NULL)
		return 0;
	auto string_data = data->m_stringData;
	if (string_data == NULL)
		return 0;
	auto mirror_data = data->m_mirroredSkeletonInfo;
	if (mirror_data == NULL)
		return 0;

	switch (edge.childType())
	{
	case NodeType::deformableSkinNames:
	{
		if (column == 0)
			return string_data->m_deformableSkinNames.getSize();
		return 0;
	}
	case NodeType::animationNames:
	{
		if (column == 0)
			return string_data->m_animationNames.getSize();
		return 0;
	}
	case NodeType::characterPropertyNames:
	{
		if (column == 0)
			return string_data->m_characterPropertyNames.getSize();
		return 0;
	}
	case NodeType::deformableSkinName:
	case NodeType::animationName:
	case NodeType::characterProperty:
		return 0;
	default:
		break;
	}
	int data_rows = HkxTableVariant(*variant).rows();
	int string_data_index = manager.findIndex(edge.file(), &*string_data);
	int string_data_rows = HkxTableVariant(*manager.at(edge.file(), string_data_index)).rows();
	int mirror_data_index = manager.findIndex(edge.file(), &*mirror_data);
	int mirror_data_rows = HkxTableVariant(*manager.at(edge.file(), mirror_data_index)).rows();
	if (edge.childType() == NodeType::HavokNative)
	{
		if (row < data_rows && 
			children_rows.find(HkxTableVariant(*variant).rowName(row)) != children_rows.end())
		{
			return 1;
		}
		if (row - data_rows < string_data_rows &&
				children_rows.find(
					HkxTableVariant(*manager.at(edge.file(), string_data_index)).rowName(row - data_rows)
				) != children_rows.end()
			) 
		{
			return 1;
		}
		if (row - data_rows - string_data_rows < mirror_data_rows &&
				children_rows.find(
					HkxTableVariant(*manager.at(edge.file(), mirror_data_index)).rowName(row - data_rows - string_data_rows)
				) != children_rows.end()
			)
		{
			return 1;
		}
		return 0;
	}
	return data_rows + string_data_rows + mirror_data_rows;

	//if (edge.childType() == )
	//{
	//	if (column == 0)
	//	{
	//		return getChildCount(row, project, variant, childType, manager);
	//	}
	//	return 0;
	//}
	//else if (childType == NodeType::animationNames)
	//{
	//	if (column == 0)
	//	{
	//		return getChildCount(row, project, variant, childType, manager);
	//	}
	//	return 0;
	//}
	//else if (childType == NodeType::characterPropertyNames)
	//{
	//	if (column == 0)
	//	{
	//		return getChildCount(row, project, variant, childType, manager);
	//	}
	//	return 0;
	//}
	//else if (childType == NodeType::characterProperty)
	//{
	//	// 3 x 1
	//	int rows = 4;
	//	//name
	//	//hkbVariableInfo.m_role.m_role
	//	//hkbVariableInfo.m_role.m_type 
	//	//hkbVariableInfo.m_type

	//	auto type = data->m_characterPropertyInfos[row].m_type;
	//	if (isWordVariable(type))
	//	{
	//		return rows + 1;
	//	}
	//	else if (isQuadVariable(type))
	//	{
	//		return rows + 1;
	//	}

	//	//ref, must be calculated
	//	int ref_index = data->m_characterPropertyValues->m_wordVariableValues[row].m_value;
	//	auto* value = data->m_characterPropertyValues->m_variantVariableValues[ref_index];
	//	int value_index = manager.findIndex(file, &*value);
	//	auto* value_variant = manager.at(file, value_index);
	//	return rows + HkxTableVariant(*value_variant).rows();
	//}

	//int childCount = getChildCount(row, project, variant, childType, manager);

	//int string_data_index = manager.findIndex(file, &*string_data);
	//int mirror_data_index = manager.findIndex(file, &*mirror_data);
	//return childCount +
	//	HkxTableVariant(*variant).rows() +
	//	HkxTableVariant(*manager.at(file, string_data_index)).rows() +
	//	HkxTableVariant(*manager.at(file, mirror_data_index)).rows();
}

int CharacterModel::childColumns(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
	if (data == NULL)
		return 0;
	auto string_data = data->m_stringData;
	if (string_data == NULL)
		return 0;
	auto mirror_data = data->m_mirroredSkeletonInfo;
	if (mirror_data == NULL)
		return 0;

	switch (edge.childType())
	{
	case NodeType::deformableSkinNames:
	case NodeType::animationNames:
	case NodeType::characterPropertyNames:
		return 1;
	case NodeType::deformableSkinName:
	case NodeType::animationName:
	case NodeType::characterProperty:
		return 0;
	default:
		break;
	}
	int data_rows = HkxTableVariant(*variant).rows();
	int string_data_index = manager.findIndex(edge.file(), &*string_data);
	int string_data_rows = HkxTableVariant(*manager.at(edge.file(), string_data_index)).rows();
	int mirror_data_index = manager.findIndex(edge.file(), &*mirror_data);
	int mirror_data_rows = HkxTableVariant(*manager.at(edge.file(), mirror_data_index)).rows();
	if (edge.childType() == NodeType::HavokNative)
	{
		if (row < data_rows &&
			children_rows.find(HkxTableVariant(*variant).rowName(row)) != children_rows.end())
		{
			return 1;
		}
		if (row - data_rows < string_data_rows &&
			children_rows.find(
				HkxTableVariant(*manager.at(edge.file(), string_data_index)).rowName(row - data_rows)
			) != children_rows.end()
			)
		{
			return 1;
		}
		if (row - data_rows - string_data_rows < mirror_data_rows &&
			children_rows.find(
				HkxTableVariant(*manager.at(edge.file(), mirror_data_index)).rowName(row - data_rows - string_data_rows)
			) != children_rows.end()
			)
		{
			return 1;
		}
		//if (row < data_rows)
		//{
		//	return variantChildRowColumns(edge.file(), row, column, variant, manager, false);
		//}
		//if (row - data_rows < string_data_rows) {
		//	return variantChildRowColumns(
		//		edge.file(),
		//		row - data_rows,
		//		column,
		//		manager.at(edge.file(), string_data_index),
		//		manager,
		//		false
		//	);
		//}
		//if (row - data_rows - string_data_rows < mirror_data_rows) {
		//	return variantChildRowColumns(
		//		edge.file(),
		//		row - data_rows - string_data_rows,
		//		column,
		//		manager.at(edge.file(), mirror_data_index),
		//		manager,
		//		false
		//	);
		//}
		//return 0;
	}
	if (row < data_rows) {
		return HkxTableVariant(*variant).columns(row);
	}
	if (row - data_rows < string_data_rows) {
		return HkxTableVariant(*manager.at(edge.file(), string_data_index)).columns(row - data_rows);
	}
	if (row - data_rows - string_data_rows < mirror_data_rows) {
		return HkxTableVariant(*manager.at(edge.file(), mirror_data_index)).columns(row - data_rows - string_data_rows);
	}
	return 0;
}

int CharacterModel::childCount(const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
	if (data == NULL)
		return 0;
	auto string_data = data->m_stringData;
	if (string_data == NULL)
		return 0;
	switch (edge.childType())
	{
	case NodeType::deformableSkinNames:
	case NodeType::animationNames:
	case NodeType::characterPropertyNames:
		return childRows(0, 0, edge, manager);
	case NodeType::deformableSkinName:
	case NodeType::animationName:
	case NodeType::characterProperty:
		return 0;
	default:
		break;
	}

	if (edge.childType() == NodeType::CharacterHkxNode)
	{
		return children_rows.size();
	}
	return 0;
}

bool CharacterModel::hasChild(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	int rows = variantChildRowColumns(edge.file(), row, column, edge.childItem<hkVariant>(), manager, true);
	int columns = variantChildRowColumns(edge.file(), row, column, edge.childItem<hkVariant>(), manager, false);
	return rows > 0 && columns > 0;
}

ModelEdge CharacterModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
	if (data == NULL)
		return ModelEdge();
	auto string_data = data->m_stringData;
	if (string_data == NULL)
		return ModelEdge();
	auto mirror_data = data->m_mirroredSkeletonInfo;
	if (mirror_data == NULL)
		return ModelEdge();

	switch (edge.childType())
	{
	case NodeType::deformableSkinNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), variant, NodeType::deformableSkinName);
	case NodeType::animationNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), variant, NodeType::animationName);
	case NodeType::characterPropertyNames:
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), variant, NodeType::characterProperty);
	case NodeType::deformableSkinName:
	case NodeType::animationName:
	case NodeType::characterProperty:
		throw std::runtime_error("Child from leaf node requested!");
	default:
		break;
	}
	int data_rows = HkxTableVariant(*variant).rows();
	int string_data_index = manager.findIndex(edge.file(), &*string_data);
	int string_data_rows = HkxTableVariant(*manager.at(edge.file(), string_data_index)).rows();
	int mirror_data_index = manager.findIndex(edge.file(), &*mirror_data);
	int mirror_data_rows = HkxTableVariant(*manager.at(edge.file(), mirror_data_index)).rows();

	std::set<QString>::iterator children_it = children_rows.end();
	if (row < data_rows) {
		children_it = children_rows.find(HkxTableVariant(*variant).rowName(row));
	}
	else if (row - data_rows < string_data_rows) {
		children_it = children_rows.find(HkxTableVariant(*manager.at(edge.file(), string_data_index)).rowName(row - data_rows));
	}
	else if (row - data_rows - string_data_rows < mirror_data_rows) {
		children_it = children_rows.find(HkxTableVariant(*manager.at(edge.file(), mirror_data_index)).rowName(row - data_rows - string_data_rows));
	}
	if (children_it == children_rows.end())
		return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::HavokNative);
	if (*children_it == "deformableSkinNames")
	{
		return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::deformableSkinNames);
	}
	if (*children_it == "animationNames")
	{
		return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::animationNames);
	}
	if (*children_it == "characterPropertyNames")
	{
		return ModelEdge(edge, edge.project(), edge.file(), row, column, edge.subindex(), variant, NodeType::characterPropertyNames);
	}
	if (*children_it == "rigName")
	{
		int rig_index = manager.getRigIndex(edge.project(), string_data);
		auto* rig = manager.getRigRoot(edge.project(), rig_index);
		return ModelEdge(edge, edge.project(), rig_index, row, column, edge.subindex(), rig, NodeType::SkeletonHkxNode);
	}
	if (*children_it == "ragdollName")
	{
		int ragdoll_index = manager.getRagdollIndex(edge.project(), string_data->m_ragdollName.cString());
		auto* ragdoll = manager.getRagdollRoot(edge.project(), ragdoll_index);
		return ModelEdge(edge, edge.project(), ragdoll_index, row, column, edge.subindex(), ragdoll, NodeType::SkeletonHkxNode);
	}
	if (*children_it == "behaviorFilename")
	{
		int behavior_index = manager.behaviorFileIndex(edge.project(), variant);
		auto* behavior = manager.behaviorFileRoot(behavior_index);
		return ModelEdge(edge, edge.project(), behavior_index, row, column, edge.subindex(), behavior, NodeType::SkeletonHkxNode);
	}
	return ModelEdge();
}

QVariant CharacterModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
	if (data == NULL)
		return "";
	auto string_data = data->m_stringData;
	if (string_data == NULL)
		return "";
	auto mirror_data = data->m_mirroredSkeletonInfo;
	if (mirror_data == NULL)
		return "";

	int data_rows = HkxTableVariant(*variant).rows();
	int string_data_index = manager.findIndex(edge.file(), &*string_data);
	int string_data_rows = HkxTableVariant(*manager.at(edge.file(), string_data_index)).rows();
	int mirror_data_index = manager.findIndex(edge.file(), &*mirror_data);
	int mirror_data_rows = HkxTableVariant(*manager.at(edge.file(), mirror_data_index)).rows();

	if (column == 0)
	{
		//names
		if (edge.childType() == NodeType::CharacterHkxNode) {
			if (row == 0)
				return string_data->m_name.cString();

			if (row < data_rows) {
				return HkxTableVariant(*variant).rowName(row);
			}
			if (row - data_rows < string_data_rows) {
				return HkxTableVariant(*manager.at(edge.file(), string_data_index)).rowName(row - data_rows);
			}
			if (row - data_rows - string_data_rows < mirror_data_rows) {
				return HkxTableVariant(*manager.at(edge.file(), mirror_data_index)).rowName(row - data_rows - string_data_rows);
			}
			return QVariant();
		}
		if (edge.childType() == NodeType::deformableSkinNames ||
			edge.childType() == NodeType::animationNames ||
			edge.childType() == NodeType::characterPropertyNames)
		{
			return DataListsName((int)edge.childType());
		}
		if (edge.childType() == NodeType::deformableSkinName)
		{
			return string_data->m_deformableSkinNames[row].cString();
		}
		if (edge.childType() == NodeType::animationName)
		{
			return string_data->m_animationNames[row].cString();
		}
		if (edge.childType() == NodeType::characterProperty)
		{
			if (column == 0)
			{
				return string_data->m_characterPropertyNames[edge.subindex()].cString();
			}
			if (column == 1 && row < 4)
			{
				hkVariant fake_variant;
				fake_variant.m_class = &hkbVariableInfoClass;
				fake_variant.m_object = &data->m_characterPropertyInfos[edge.subindex()];
				return HkxTableVariant(fake_variant).data(row - 1, column - 1);
			}
			else {
				auto variable_type = data->m_characterPropertyInfos[edge.subindex()];
				//if (isWordVariable())
			}
			return "InvalidColumn";
		}
		return QVariant();
	}
	if (row < data_rows) {
		return HkxTableVariant(*variant).data(row, column -1);
	}
	if (row - data_rows < string_data_rows) {
		return HkxTableVariant(*manager.at(edge.file(), string_data_index)).data(row - data_rows, column - 1);
	}
	if (row - data_rows - string_data_rows < mirror_data_rows) {
		return HkxTableVariant(*manager.at(edge.file(), mirror_data_index)).data(row - data_rows - string_data_rows, column - 1);
	}
	return QVariant();
}

bool CharacterModel::setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager)
{
	//auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
	//auto string_data = data->m_stringData;
	//auto mirror_data = data->m_mirroredSkeletonInfo;
	//if (childType == NodeType::animationName)
	//{
	//	string_data->m_animationNames[row] = value.toString().toUtf8().constData();
	//	return true;
	//}

	//if (row < SUPPORT_END)
	//	return false;
	//HkxTableVariant data_variant(*variant);
	//int data_rows = data_variant.rows();
	//if (row < SUPPORT_END + data_rows)
	//	return data_variant.setData(row - SUPPORT_END, column - 1, value);
	//int string_data_index = manager.findIndex(file, &*string_data);
	//HkxTableVariant stringdata_variant(*manager.at(file, string_data_index));
	//int stringdata_rows = stringdata_variant.rows();
	//if (row < SUPPORT_END + data_rows + stringdata_rows)
	//	return stringdata_variant.setData(row - data_rows - SUPPORT_END, column - 1, value);
	//int mirroring_data_index = manager.findIndex(file, &*mirror_data);
	//HkxTableVariant mirroring_data_variant(*manager.at(file, mirroring_data_index));
	//int mirroring_data_rows = mirroring_data_variant.rows();
	//if (row < SUPPORT_END + data_rows + stringdata_rows + mirroring_data_rows)
	//	return mirroring_data_variant.setData(row - stringdata_rows - data_rows - SUPPORT_END, column - 1, value);
	return false;
}

bool CharacterModel::addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	//auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
	//auto string_data = data->m_stringData;
	//auto mirror_data = data->m_mirroredSkeletonInfo;

	//if (childType == NodeType::animationNames)
	//{
	//	return addToContainer(row_start, count, string_data->m_animationNames);
	//}
	return false;
}

bool CharacterModel::removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	//auto* data = reinterpret_cast<hkbCharacterData*>(variant->m_object);
	//auto string_data = data->m_stringData;
	//auto mirror_data = data->m_mirroredSkeletonInfo;

	//if (childType == NodeType::animationNames)
	//{
	//	for (int i = 0; i < count; ++i)
	//		string_data->m_animationNames.removeAt(row_start);
	//	return true;
	//}
	return false;
}

bool CharacterModel::changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager)
{
	return false;
}