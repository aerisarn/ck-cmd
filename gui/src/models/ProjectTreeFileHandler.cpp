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
	case NodeType::MiscNode:
		ProjectType res_type = childType == NodeType::CharacterNode ? ProjectType::character : ProjectType::misc;
		if (file_index != -1) {
			return _manager.projectCharacters(file_index);
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
	case NodeType::BehaviorHkxNode:		
		return _manager.path(file_index).string().c_str();
	}
	return QVariant();
}

bool ProjectTreeFileHandler::hasChild(int row, int _project, int _file, NodeType childType, ResourceManager& manager)
{
	return getChildCount(_project, childType, manager) > 0;
}

ModelEdge ProjectTreeFileHandler::getChild(int index, int project, int file, NodeType childType, ResourceManager& _manager)
{
	switch (childType) {
	case NodeType::CharactersNode:
	case NodeType::MiscsNode:
	{
		ProjectType res_type = childType == NodeType::CharactersNode ? ProjectType::character : ProjectType::misc;
		NodeType index_type = childType == NodeType::CharactersNode ? NodeType::CharacterNode : NodeType::MiscNode;
		int project_file = -1;
		//if (_manager.isProjectFileOpen(index, res_type)) {
		//	project_file = _manager.projectFileIndex(index, res_type);
		//}
		return ModelEdge(nullptr, project, -1, index, 0, nullptr, index_type);
	}
	case NodeType::CharacterNode:
	case NodeType::MiscNode:
	{
		ProjectType res_type = childType == NodeType::CharacterNode ? ProjectType::character : ProjectType::misc;
		int character_file = _manager.characterFileIndex(index, project, res_type);
		auto* variant = _manager.characterFileRoot(character_file);
		return ModelEdge(variant, project, character_file, index, 0, variant, NodeType::CharacterHkxNode);
	}
	default:
		break;
	}
	return ModelEdge();
}