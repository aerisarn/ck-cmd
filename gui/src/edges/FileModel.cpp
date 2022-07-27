#include <src/edges/FileModel.h>

using namespace ckcmd::HKX;

std::vector<NodeType> FileModel::handled_types() const
{
	return {
		NodeType::CharactersNode,
		NodeType::MiscsNode,
		NodeType::CharacterNode,
		NodeType::MiscNode
	};
}

int FileModel::rows(const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType()) {
	case NodeType::CharactersNode:
		return manager.character_project_files();
	case NodeType::MiscsNode:
		return manager.miscellaneous_project_files();
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
		ProjectType res_type = edge.childType() == NodeType::CharacterNode ? ProjectType::character : ProjectType::misc;
		if (edge.project() != -1) {
			return manager.projectCharacters(edge.project());
		}
		return 0;
	}
	return 0;
}

int FileModel::columns(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	return 1;
}

int FileModel::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	return rows(edge, manager);
}

std::pair<int, int> FileModel::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	return { index, 0 };
}

int FileModel::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType()) {
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
		return row;
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
	{
		if (edge.project() != -1)
			return row;
		return -1;
	}
	default:
		break;
	}
	return -1;
}

ModelEdge FileModel::child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType()) {
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	{
		ProjectType res_type = edge.childType() == NodeType::CharactersNode ? ProjectType::character : ProjectType::misc;
		NodeType index_type = edge.childType() == NodeType::CharactersNode ? NodeType::CharacterNode : NodeType::MiscNode;
		int project_file = manager.projectFileIndex(row, res_type);
		return ModelEdge(edge, project_file, -1, row, 0, -1, nullptr, index_type);
	}
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
	{
		ProjectType res_type = edge.childType() == NodeType::CharacterNode ? ProjectType::character : ProjectType::misc;
		int character_file = manager.characterFileIndex(row, edge.project(), res_type);
		auto* variant = manager.characterFileRoot(character_file);
		return ModelEdge(variant, edge.project(), character_file, row, 0, variant, NodeType::CharacterHkxNode);
	}
	default:
		break;
	}
	return ModelEdge();
}

QVariant FileModel::data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	switch (edge.childType()) {
	case NodeType::CharactersNode:
		return "Character Projects";
	case NodeType::MiscsNode:
		return "Misc Projects";
	case NodeType::CharacterNode:
		return manager.character_project_file(edge.row());
	case NodeType::MiscNode:
		return manager.miscellaneous_project_file(edge.row());
	case NodeType::BehaviorHkxNode:
		return manager.path(edge.file()).string().c_str();
	}
	return QVariant();
}

std::pair<int, int> FileModel::dataStart(const ModelEdge& edge) const
{
	return { -1, -1 };
}

bool FileModel::setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager)
{
	return false;
}

bool FileModel::addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	return false;
}

bool FileModel::removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	return false;
}

bool FileModel::changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager)
{
	return false;
}