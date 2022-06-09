#include <src/models/ProjectTreeFileHandler.h>
#include <src/hkx/HkxLinkedTableVariant.h>

using namespace ckcmd::HKX;


int ProjectTreeFileHandler::getChildCount(int file_index, NodeType childType, ResourceManager& _manager)
{
	switch (childType) {
	case NodeType::CharactersNode:
		return _manager.character_project_files();
	case NodeType::MiscsNode:
		return _manager.miscellaneous_project_files();
	case NodeType::CharacterNode:
		if (_manager.isCharacterFileOpen(file_index)) {
			return 1;
		}
		return 0;
	}
	return 0;
}

QVariant ProjectTreeFileHandler::data(int row, int column, int file_index, NodeType childType, ResourceManager& _manager)
{
	switch (childType) {
	case NodeType::CharactersNode:
		return "Character Projects";
	case NodeType::MiscsNode:
		return "Misc Projects";
	case NodeType::CharacterNode:
		return _manager.character_project_file(row);
	case NodeType::MiscNode:
		return _manager.miscellaneous_project_file(row);
	}
	return QVariant();
}

ModelEdge ProjectTreeFileHandler::getChild(int index, int project, int file, NodeType childType, ResourceManager& _manager)
{
	switch (childType) {
	case NodeType::CharactersNode:
	{
		return ModelEdge(nullptr, -1, -1, index, -1, nullptr, NodeType::CharacterNode);
	}
	case NodeType::MiscsNode:
		return ModelEdge(nullptr, -1, -1, index, -1, nullptr, NodeType::MiscNode);
	case NodeType::CharacterNode:
	{
		auto file_index = _manager.characterFileIndex(index);
		auto* variant = &_manager.get(file_index).first;
		return ModelEdge(variant, file, file, index, -1, variant);
	}
	default:
		break;
	}
	return ModelEdge();
}