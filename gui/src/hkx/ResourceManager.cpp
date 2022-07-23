#include <src/Skyrim/TES5File.h>
#include <src/Collection.h>
#include <src/ModFile.h>
#include <src/hkx/HkxTableVariant.h>
#include <src/hkx/ReferenceSetter.h>
#include <src/utility/Objects.h>
#include <src/items/HkxItemBone.h>
#include <src/items/HkxItemEvent.h>
#include <src/items/HkxItemRagdollBone.h>
#include <src/items/HkxItemVar.h>

#include <src/config.h>

#include "ResourceManager.h"

#include <hkbProjectData_2.h>
#include <hkbCharacterData_7.h>
#include <hkbBehaviorReferenceGenerator_0.h>
#include <hkbBehaviorGraph_1.h>
#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
#include <Animation/Animation/hkaAnimationContainer.h>
#include <hkbClipGenerator_2.h>
#include <hkbStateMachine_4.h>

#include <QSet>
#include <atomic>

//#include <iostream>
//#include <fstream>

#include <src/hkx/HkxSpecialMembersMaps.h>

using namespace ckcmd::HKX;

static std::atomic<size_t> runtime_file_id = 0;

ResourceManager::ResourceManager(WorkspaceConfig& workspace) :
	_workspace(workspace),
	_cache(_workspace.getFolder())
{

	LOG << "Opening " << _workspace.getFolder() << log_endl;

	if (_workspace.empty())
	{
		LOG << "Workspace has no projects, scanning ... " << _workspace.getFolder() << log_endl;
		scanWorkspace();
	}

	_charactersNames = _workspace.getCharacterProjects();
	for (auto& character : _charactersNames)
	{
		_characters.push_back(character.toUtf8().constData());
	}

	_miscellaneousNames = _workspace.getMiscellaneousProjects();
	for (auto& misc : _miscellaneousNames)
	{
		_miscellaneous.push_back(misc.toUtf8().constData());
	}

	_esp = new Collection((char* const)_workspace.getFolder().string().c_str(), 3);

	ModFlags masterFlags = ModFlags(0xA);
	ModFile* skyrimMod = _esp->AddMod("creatures.esp", masterFlags);

	char* argvv[4];
	argvv[0] = new char();
	argvv[1] = new char();
	argvv[2] = new char();
	argvv[3] = new char();
	logger.init(4, argvv);

	_esp->Load();

	//find children classes;
	hkDefaultClassNameRegistry& defaultRegistry = hkDefaultClassNameRegistry::getInstance();
	hkArray<const hkClass*> classes;
	defaultRegistry.getClasses(classes);
	for (int i = 0; i < classes.getSize(); ++i)
	{
		bool isParent = false;
		for (int j = 0; j < classes.getSize(); ++j)
		{
			if (classes[j]->getParent() == classes[i])
			{
				isParent = true;
				break;
			}
		}
		if (!isParent)
			_concreate_classes.push_back(classes[i]);
	}

	//used to generate the Special members map
	//std::set<std::array<std::string, 3>> interesting_members;
	//std::set<std::string> interesting_classes;
	//for (int c = 0; c < _concreate_classes.size(); ++c)
	//{
	//	for (size_t i = 0; i < _concreate_classes[c]->getNumMembers(); ++i) {
	//		const auto& member_declaration = _concreate_classes[c]->getMember(i);
	//		if (member_declaration.getFlags().get() & hkClassMember::SERIALIZE_IGNORED)
	//			continue;
	//		switch (member_declaration.getType()) {
	//		case hkClassMember::TYPE_INT8:
	//		case hkClassMember::TYPE_UINT8:
	//		case hkClassMember::TYPE_INT16:
	//		case hkClassMember::TYPE_UINT16:
	//		case hkClassMember::TYPE_INT32:
	//		case hkClassMember::TYPE_UINT32:
	//		case hkClassMember::TYPE_INT64:
	//		case hkClassMember::TYPE_UINT64:
	//			interesting_classes.insert(std::string(_concreate_classes[c]->getName()) + "Class");
	//			interesting_members.insert(
	//				{
	//				"&" + std::string(_concreate_classes[c]->getName()) + "Class",
	//				std::to_string(i),
	//				member_declaration.getName()
	//				}
	//			);
	//		default:
	//			break;
	//		}
	//	}
	//}
	//ofstream myfile;
	//myfile.open("int_class_members.txt");
	//for (auto& entry : interesting_classes)
	//{
	//	myfile << "extern const hkClass " << entry << ";" << std::endl;
	//}
	//myfile << std::endl;
	//for (auto& entry : interesting_members)
	//{
	//	std::string guessed_type = "MemberIndexType::Invalid";
	//	if (
	//		entry[0].find("ragdoll") != string::npos || entry[0].find("Ragdoll") != string::npos ||
	//		entry[2].find("ragdoll") != string::npos || entry[2].find("Ragdoll") != string::npos
	//		)
	//	{
	//		guessed_type = "MemberIndexType::ragdollBoneIndex";
	//	}
	//	else if (
	//		entry[0].find("bone") != string::npos || entry[0].find("Bone") != string::npos ||
	//		entry[2].find("bone") != string::npos || entry[2].find("Bone") != string::npos
	//		)
	//	{
	//		guessed_type = "MemberIndexType::boneIndex";
	//	}
	//	else if (
	//		entry[0].find("event") != string::npos || entry[0].find("Event") != string::npos ||
	//		entry[2].find("event") != string::npos || entry[2].find("Event") != string::npos
	//		)
	//	{
	//		guessed_type = "MemberIndexType::eventIndex";
	//	}
	//	else if (
	//		entry[0].find("variable") != string::npos || entry[0].find("Variable") != string::npos ||
	//		entry[2].find("variable") != string::npos || entry[2].find("Variable") != string::npos
	//		)
	//	{
	//		guessed_type = "MemberIndexType::variableIndex";
	//	}
	//	myfile << "\t{{" << entry[0] << "," << entry[1] << "}, " << guessed_type <<"}, //" << entry[2] << std::endl;
	//}
	//myfile.close();

}

ResourceManager::~ResourceManager() 
{
	delete _esp;
}

int ResourceManager::index(const fs::path& file) const {
	for (const auto& entry : _files)
	{
		if (fs::equivalent(entry.second,file))
		{
			return entry.first;
		}
	}
	return -1;
}

fs::path ResourceManager::path(int file_index) const
{
	return _files.at(file_index);
}

bool ResourceManager::is_open(const fs::path& file) const {
	return std::find_if(_files.begin(), _files.end(), [file](const auto& entry) {return fs::equivalent(file,entry.second); }) != _files.end();
}

bool ResourceManager::is_open(int file_index) const {
	return _files.find(file_index) != _files.end();
}


hkVariant* ResourceManager::findVariant(int file_index, const void* object)
{
	auto& _objects = _contents.at(file_index).second;
	for (auto& _object : _objects)
	{
		if (_object.m_object == object)
			return &_object;
	}
	return nullptr;
}

hk_object_list_t ResourceManager::findCompatibleNodes(size_t file_index, const hkClassMember* member_class)
{
	return findCompatibleNodes(file_index, member_class->getClass());
}

hk_object_list_t ResourceManager::findCompatibleNodes(size_t file_index, const hkClass* hkclass)
{
	hk_object_list_t result;
	auto& file_contents = _contents.at(file_index).second;
	for (auto& content : file_contents)
	{
		if (hkclass == content.m_class ||
			hkclass->isSuperClass(*content.m_class)) {
			result.push_back(&content);
		}
	}
	return result;
}
std::vector<const hkClass*> ResourceManager::findCompatibleClasses(const hkClass* hkclass) const
{
	std::vector<const hkClass*> out;
	for (size_t c = 0; c < _concreate_classes.size(); ++c)
	{
		auto hclass = _concreate_classes[c];
		if (hclass->getNumDeclaredMembers() > 0 &&
			(hkclass == _concreate_classes[c] || hkclass->isSuperClass(*_concreate_classes[c])))
		{
			out.push_back(hclass);
		}
	}
	return out;
}

hkx_file_t& ResourceManager::get(size_t index)
{
	return _contents[index];
}

hkx_file_t& ResourceManager::get(const fs::path& file)
{
	if (!is_open(file)) {
		HKXWrapper wrap;
		fs::path xml_path = file;  xml_path.replace_extension(".xml");
		fs::path hkx_path = file;  hkx_path.replace_extension(".hkx");
		
		hkRootLevelContainer* root;
		hkx_file_t new_file;
		hkArray<hkVariant> to_read;
		if (fs::exists(xml_path))
		{
			root = wrap.read(xml_path, to_read);
		}
		else if (fs::exists(hkx_path))
			root = wrap.read(hkx_path, to_read);
		else
		{
			LOG << "Unable to find: " + file.string() << log_endl;
			//throw std::runtime_error("Unable to find: " + file.string());
		}
		for (int i = 0; i < to_read.getSize(); i++) {
			new_file.second.push_back(to_read[i]);
		}

		for (int i = 0; i < to_read.getSize(); i++) {
			if (to_read[i].m_object == root)
			{
				new_file.first = to_read[i];
				break;
			}
		}	
		_contents[runtime_file_id] = new_file;

		_files[runtime_file_id++] = file;
	}
	return _contents[index(file)];
}

void ResourceManager::save(size_t index)
{
	const fs::path file = _files.at(index);
	auto& contents = _contents[index];
	HKXWrapper wrap;
	try {
		wrap.write_le_se((hkRootLevelContainer*)contents.first.m_object, file);
	} catch (...)
	{ }
}

bool ResourceManager::isHavokProject(const fs::path& file)
{
	HKXWrapper wrap;
	try {
		auto root = wrap.read(file);
		if (root)
			return root->findObjectByType(hkbProjectDataClass.getName()) != NULL;
		return false;
	}
	catch (...) {
		return false;
	}
}

bool ResourceManager::isHavokAnimation(const fs::path& file)
{
	HKXWrapper wrap;
	try {
		auto root = wrap.read(file);
		if (root)
			return root->findObjectByType(hkaAnimationContainerClass.getName()) != NULL;
		return false;
	}
	catch (...) {
		return false;
	}
}

std::string __inline internal_get_sanitized_name(const fs::path& path) {
	return path.filename().replace_extension("").string();
}

const std::string& ResourceManager::get_sanitized_name(int file_index) {
	if (_sanitized_names.find(file_index) == _sanitized_names.end())
	{
		auto p = _files.at(file_index);
		_sanitized_names[file_index] = internal_get_sanitized_name(p);
	}
	return _sanitized_names.at(file_index);
}

bool ResourceManager::isCreatureProject(int file_index) {
	auto entry = findCacheEntry(file_index);
	if (entry->hasCache())
		return true;
	return false;
}

CacheEntry* ResourceManager::findCacheEntry(const std::string& sanitized_name)
{
	return _cache.find(sanitized_name);
}

CacheEntry* ResourceManager::findCacheEntry(size_t file_index)
{
	return _cache.find(get_sanitized_name(file_index));
}

void ResourceManager::save_cache(int project_index)
{
	string project = get_sanitized_name(project_index);
	CacheEntry* project_entry = findCacheEntry(project);
	fs::path animationDataPath = "animationdatasinglefile.txt";
	fs::path animationSetDataPath = "animationsetdatasinglefile.txt";
	fs::create_directories(_workspace.getFolder());
	_cache.save_creature(
		project, project_entry, animationDataPath, animationSetDataPath, _workspace.getFolder()
	);
}


std::set<Sk::AACTRecord*> ResourceManager::actions()
{
	std::set<Sk::AACTRecord*> out;
	for (auto idle_record_it = _esp->FormID_ModFile_Record.begin(); idle_record_it != _esp->FormID_ModFile_Record.end(); idle_record_it++)
	{
		Record* record = idle_record_it->second;
		if (record->GetType() == REV32(AACT)) {
			out.insert(dynamic_cast<Sk::AACTRecord*>(record));
		}
	}
	return out;
}

static inline bool iequals(const string& a, const string& b)
{
	return std::equal(a.begin(), a.end(),
		b.begin(), b.end(),
		[](char a, char b) {
			return tolower(a) == tolower(b);
		});
}

std::set<Sk::IDLERecord*> ResourceManager::idles(size_t index)
{
	std::set<Sk::IDLERecord*> out;
	std::string this_behavior = fs::relative(_files.at(index), _workspace.getFolder()).string();
	for (auto idle_record_it = _esp->FormID_ModFile_Record.begin(); idle_record_it != _esp->FormID_ModFile_Record.end(); idle_record_it++)
	{
		Record* record = idle_record_it->second;
		if (record->GetType() == REV32(IDLE)) {
			auto idle = dynamic_cast<Sk::IDLERecord*>(record);
			if (iequals(this_behavior,idle->DNAM.value))
				out.insert(idle);
		}
	}
	return out;
}

long long crc_32_ll(std::string& to_crc)
{
	transform(to_crc.begin(), to_crc.end(), to_crc.begin(), ::tolower);
	long long crc = stoll(HkCRC::compute(to_crc), NULL, 16);
	return crc;
}

std::string crc_32(std::string& to_crc)
{
	transform(to_crc.begin(), to_crc.end(), to_crc.begin(), ::tolower);
	long long crc = stoll(HkCRC::compute(to_crc), NULL, 16);
	return to_string(crc);
}

std::array<std::string, 4>  ResourceManager::animationCrc32(const fs::path& path)
{
	fs::path absolute_folder = path.parent_path();
	std::string clip_name = path.filename().replace_extension("").string();
	std::string relative_path = "meshes\\" + fs::relative(absolute_folder, _workspace.getFolder()).generic_string();
	string::size_type n = 0;
	while ((n = relative_path.find('/', n)) != string::npos)
	{
		relative_path.replace(n, 1, "\\");
		n += 1;
	}
	if (relative_path.at(relative_path.size() - 1) == '\\')
		relative_path = relative_path.substr(0, relative_path.size() - 1);
	return { relative_path, crc_32(relative_path), clip_name, crc_32(clip_name) };
}

void ResourceManager::scanWorkspace()
{
	for (auto& p : fs::recursive_directory_iterator(_workspace.getFolder()))
	{
		if (fs::is_regular_file(p.path())
			&& (p.path().extension() == ".hkx" || p.path().extension() == ".xml")
			)
		{
			LOG << "Analyzing file ... " << p.path().string() << log_endl;
			if (isHavokProject(p.path())) {
				string sanitized_project_name = internal_get_sanitized_name(p.path());
				LOG << "Found project " << sanitized_project_name << log_endl;
				CacheEntry* entry = findCacheEntry(sanitized_project_name);
				if (NULL == entry)
				{
					LOG << " WARNING: " << sanitized_project_name << " was not found into the animation cache. The project won't be loaded by the game" << log_endl;
				}
				else {
					if (entry->hasCache()) {
						//LOG << "Project is a creature" << sanitized_project_name << log_endl;
						_workspace.addCharacterProject(p.path().string().c_str(), sanitized_project_name.c_str());

					}
					else {
						//LOG << "Project is miscellaneous" << sanitized_project_name << log_endl;
						_workspace.addMiscellaneousProject(p.path().string().c_str(), sanitized_project_name.c_str());
					}
				}
			}
			//else if (isHavokAnimation(p.path()))
			//{
			//	LOG << "Found animation " << p.path() << log_endl;
			//	auto crc32strings = animationCrc32(p.path());
			//	LOG << "Path: " << crc32strings[0] << " CRC32: " << crc32strings[1] << log_endl;
			//	_workspace.addCrc32(crc32strings[1].c_str(), crc32strings[0].c_str()); //path
			//	LOG << "Name: " << crc32strings[2] << " CRC32: " << crc32strings[3] << log_endl;
			//	_workspace.addCrc32(crc32strings[3].c_str(), crc32strings[2].c_str()); //name
			//}
		}
	}
}

const fs::path& ResourceManager::projectPath(int row, ProjectType type)
{
	switch (type)
	{
	case ProjectType::character:
		return _characters.at(row);
	case ProjectType::misc:
		return _miscellaneous.at(row);
	default:
		break;
	}
	return fs::path();
}

bool ResourceManager::isProjectFileOpen(int row, ProjectType type)
{
	if (row < 0)
		return false;
	return is_open(projectPath(row, type));
}

void ResourceManager::openProjectFile(int row, ProjectType type)
{
	auto& path = projectPath(row, type);
	if (fs::exists(path))
	{
		get(path);
	}
}

size_t ResourceManager::projectCharacters(int project_index)
{
	hkbProjectStringData* data = getProjectRoot(project_index);
	if (nullptr != data)
		return data->m_characterFilenames.getSize();
	return 0;
}

hkbProjectStringData* ResourceManager::getProjectRoot(int file_index)
{
	auto& contents = get(file_index);
	for (auto& item : contents.second)
	{
		if (&hkbProjectStringDataClass == item.m_class)
		{
			return  reinterpret_cast<hkbProjectStringData*>(item.m_object);
		}
	}
	return nullptr;
}

hkbCharacterData* ResourceManager::getCharacterData(int character_index)
{
	auto& contents = get(character_index);
	for (auto& item : contents.second)
	{
		if (&hkbCharacterDataClass == item.m_class)
		{
			return  reinterpret_cast<hkbCharacterData*>(item.m_object);
		}
	}
	return nullptr;
}

hkbCharacterStringData* ResourceManager::getCharacterString(int character_index)
{
	auto& contents = get(character_index);
	for (auto& item : contents.second)
	{
		if (&hkbCharacterStringDataClass == item.m_class)
		{
			return  reinterpret_cast<hkbCharacterStringData*>(item.m_object);
		}
	}
	return nullptr;
}

hkbProjectStringData* ResourceManager::getProjectRoot(const fs::path& fs_path)
{
	auto& contents = get(fs_path);
	for (auto& item : contents.second)
	{
		if (&hkbProjectStringDataClass == item.m_class)
		{
			return  reinterpret_cast<hkbProjectStringData*>(item.m_object);
		}
	}
	return nullptr;
}

void ResourceManager::closeProjectFile(int row, ProjectType type)
{
	const fs::path& fs_path = projectPath(row, type);
	int project_index = index(fs_path);
	hkbProjectStringData* project_data = nullptr;
	if (project_index != (size_t)-1)
	{
		project_data = getProjectRoot(project_index);
	}
	//remove animation sets/animations
	if (nullptr != project_data && project_data->m_characterFilenames.getSize())
	{
		fs::path char_path = fs_path.parent_path() / project_data->m_characterFilenames[0].cString();
		int character_index = index(char_path);
		auto* string_data = getCharacterString(character_index);
		if (nullptr != string_data && string_data->m_animationNames.getSize() && isCreatureProject(project_index))
		{
			for (int a = 0; a < string_data->m_animationNames.getSize(); ++a)
			{
				std::string row = string_data->m_animationNames[a].cString();
				long long path_crc32 = crc_32_ll(row);
				auto to_erase = _decoded_loaded_sets.equal_range({ project_index, path_crc32 });
				if (to_erase.first != to_erase.second)
					_decoded_loaded_sets.erase(to_erase.first, to_erase.second);

				auto clip_to_erase = _animations_root_movements.equal_range({ project_index, path_crc32 });
				if (clip_to_erase.first != clip_to_erase.second)
					_animations_root_movements.erase(clip_to_erase.first, clip_to_erase.second);
			}
		}
	}
	close(project_index);
}

size_t ResourceManager::projectFileIndex(int row, ProjectType type)
{
	return index(projectPath(row, type));
}

struct ci_less
{
	// case-independent (ci) compare_less binary function
	struct nocase_compare
	{
		bool operator() (const unsigned char& c1, const unsigned char& c2) const {
			return tolower(c1) < tolower(c2);
		}
	};
	bool operator() (const std::string& s1, const std::string& s2) const {
		return std::lexicographical_compare
		(s1.begin(), s1.end(),   // source range
			s2.begin(), s2.end(),   // dest range
			nocase_compare());  // comparison
	}
};

size_t ResourceManager::characterFileIndex(int row, int project_index, ProjectType type)
{
	size_t result = -1;
	fs::path fs_path = path(project_index);
	hkbProjectStringData* project_data = nullptr;
	hkbCharacterStringData* string_data = nullptr;
	project_data = getProjectRoot(project_index);
	bool already_open = false;
	if (nullptr != project_data && project_data->m_characterFilenames.getSize())
	{
		fs::path char_path = fs_path.parent_path() / project_data->m_characterFilenames[row].cString();
		result = index(char_path);
		if (result != (size_t)-1)
		{
			already_open = true;
		}
		else if (fs::exists(char_path))
		{
			auto& contents = get(char_path);
			for (auto& item : contents.second)
			{
				if (&hkbCharacterStringDataClass == item.m_class)
				{
					string_data = reinterpret_cast<hkbCharacterStringData*>(item.m_object);
					break;
				}
			}
			result = index(char_path);
		}
	}
	if (result != (size_t)-1 && !already_open)
	{
		//build animation sets
		if (nullptr != string_data && string_data->m_animationNames.getSize() && isCreatureProject(project_index))
		{
			CreatureCacheEntry* entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_index));
			size_t sets_size = entry->sets.getProjectFiles().getStrings().size();

			std::map<std::string, std::string> crc32_map;
			std::map<std::string, long long, ci_less> index_map;
			for (int a = 0; a < string_data->m_animationNames.getSize(); ++a)
			{
				std::string row_string = string_data->m_animationNames[a].cString();
				std::string anim_name = fs::path(row_string).filename().replace_extension("").string();
				std::string anim_crc32 = crc_32(anim_name);
				long long path_crc32 = crc_32_ll(row_string);
				crc32_map[anim_crc32] = row_string;
				index_map[string_data->m_animationNames[a].cString()] = path_crc32;
			}
#pragma parallel for
			for (int s = 0; s < sets_size; ++s)
			{
				auto set_name = entry->sets.getProjectFiles()[s];
				long long set_crc32 = crc_32_ll(set_name);
				auto& crcs = entry->sets.getProjectAttackBlocks()[s].getCrc32Data().getStrings();
				for (int c = 0; c < crcs.size(); ++c)
				{
					if (c % 3 == 1)
					{
						_decoded_loaded_sets.insert({ {(size_t)project_index, set_crc32}, crc32_map[crcs.at(c)] });
					}
				}
			}
			//check clip generators
			fs::path behavior_path = fs_path.parent_path() / string_data->m_behaviorFilename.cString(); behavior_path = behavior_path.parent_path();
			for (auto& p : fs::directory_iterator(behavior_path))
			{
				if (fs::is_regular_file(p.path()))
				{
					HKXWrapper wrap;
					hkArray<hkVariant> objects;
					auto* rlc = wrap.read(p, objects);
					if (&hkbBehaviorGraphClass == rlc->m_namedVariants[0].getClass())
					{
						for (const auto& obj : objects) {
							if (obj.m_class == &hkbClipGeneratorClass) {
								hkbClipGenerator* clip = (hkbClipGenerator*)obj.m_object;
								if (clip->m_name == nullptr)
									continue;
								auto movement = entry->findMovement(clip->m_name.cString());
								if (clip->m_animationName == nullptr)
									continue;
								auto it = index_map.find(clip->m_animationName.cString());
								if (it != index_map.end())
									_animations_root_movements[{project_index, it->second}] = movement;
								else
									LOGINFO << "Clip: " << clip->m_animationName.cString() << " Was found in behavior clips but not into Character string data" << endl;
							}
						}
					}
				}
			}
		}
	}
	return result;
}

void ResourceManager::saveProject(int row, ProjectType type)
{
	fs::path fs_path = projectPath(row, type);
	int project_index = index(fs_path);
	saveProject(project_index);
}

void ResourceManager::saveProject(int project_index)
{
	fs::path fs_path = path(project_index);
	fs::path base_path = fs_path.parent_path();
	auto& project_contents = get(fs_path);
	HKXWrapper wrap; wrap.write_le_se((hkRootLevelContainer*)project_contents.first.m_object, fs_path);
	hkbProjectStringData* project_data = getProjectRoot(project_index);
	if (nullptr != project_data && project_data->m_characterFilenames.getSize())
	{
		fs::path char_path = fs_path.parent_path() / project_data->m_characterFilenames[0].cString();
		int character_index = index(char_path);
		if (character_index == -1)
		{
			get(char_path);
			character_index = index(char_path);
		}
		else {
			wrap.write_le_se((hkRootLevelContainer*)get(char_path).first.m_object, char_path);
		}
		auto* string_data = getCharacterString(character_index);
		if (nullptr != string_data)
		{
			CacheEntry* entry = _cache.find(get_sanitized_name(project_index));
			CreatureCacheEntry* creature_entry = dynamic_cast<CreatureCacheEntry*>(entry);
			bool creature = (creature_entry != nullptr);
			entry->block.clear();
			entry->movements.clear();
			auto& clips_cache = entry->block.getClips();
			auto& movements_cache = entry->movements.getMovementData();

			AnimData::StringListBlock project_hkx_files;
			fs::path behavior_path = fs_path.parent_path() / string_data->m_behaviorFilename.cString(); behavior_path = behavior_path.parent_path();
			std::vector<AnimData::root_movement_t*> movement_serialization_map;
			std::map<std::string, long long> crc32_map;
			std::map<long long, std::string> inverse_crc32_map;
			std::map<long long, std::multimap<float, std::string>> anim_triggers;

			for (auto& p : fs::directory_iterator(behavior_path))
			{
				if (fs::is_regular_file(p.path()))
				{
					int behavior_index = index(p.path());
					hkRootLevelContainer* behavior_root = nullptr;
					if (behavior_index == -1)
					{
						auto& behavior_contents = get(p.path());
						behavior_index = index(p.path());
						behavior_root = reinterpret_cast<hkRootLevelContainer*>(behavior_contents.first.m_object);
					}
					else {
						behavior_root = reinterpret_cast<hkRootLevelContainer*>(get(behavior_index).first.m_object);
						wrap.write_le_se(behavior_root, p.path());
					}
					if (&hkbBehaviorGraphClass == behavior_root->m_namedVariants[0].getClass())
					{
						fs::path file = fs::relative(p.path(), base_path);
						project_hkx_files.append(file.string());


						if (creature)
						{
							auto& behavior_contents = get(behavior_index);
							hkbBehaviorGraphStringData* behavior_data = nullptr; // reinterpret_cast<hkbBehaviorGraphStringData*>(reinterpret_cast<hkRootLevelContainer*>(behavior_contents.first.m_object)->findObjectByType(hkbBehaviorGraphStringDataClass.getName()));
							for (const auto& content : behavior_contents.second)
							{
								if (&hkbBehaviorGraphStringDataClass == content.m_class)
								{
									behavior_data = reinterpret_cast<hkbBehaviorGraphStringData*>(content.m_object);
									break;
								}
							}
							if (nullptr == behavior_data)
								continue;
							auto& events = behavior_data->m_eventNames;
							for (const auto& content : behavior_contents.second)
							{
								if (&hkbClipGeneratorClass == content.m_class)
								{
									AnimData::ClipGeneratorBlock clip_block;
									hkbClipGenerator* clip = (hkbClipGenerator*)content.m_object;
									clip_block.setName(clip->m_name.cString());
									if (clip->m_animationName == nullptr)
										continue;
									std::string row = clip->m_animationName.cString();

									auto it = crc32_map.find(row);
									if (it == crc32_map.end())
									{
										long long path_crc32 = crc_32_ll(row);
										it = crc32_map.insert({ row, path_crc32 }).first;
										inverse_crc32_map[path_crc32] = row;
									}
									auto movement_it = _animations_root_movements.find({ project_index, it->second });
									if (movement_it == _animations_root_movements.end())
										continue;
									float duration = 0.0;
									if (movement_it != _animations_root_movements.end())
									{
										auto get = std::find(movement_serialization_map.begin(), movement_serialization_map.end(), &movement_it->second);
										if (get == movement_serialization_map.end())
										{
											clip_block.setCacheIndex(movements_cache.size());
											AnimData::ClipMovementData data(movement_it->second);
											data.setCacheIndex(movements_cache.size());
											movements_cache.push_back(data);
											movement_serialization_map.push_back(&movement_it->second);
										}
										else {
											clip_block.setCacheIndex(std::distance(movement_serialization_map.begin(), get));
										}
										duration = movement_it->second.duration;
									}
									auto triggers_it = anim_triggers.find(it->second);
									if (triggers_it == anim_triggers.end())
									{
										fs::path animation_path = fs_path.parent_path() / clip->m_animationName.cString();
										if (fs::exists(animation_path))
										{
											hkArray<hkVariant> anim_objects;
											auto* anim_rlc = wrap.read(animation_path, anim_objects);
											for (const auto& anim_content : anim_objects)
											{
												if (hkaAnimationClass.isSuperClass(*anim_content.m_class))
												{
													hkaAnimation* anim = reinterpret_cast<hkaAnimation*>(anim_content.m_object);
													if (nullptr != anim && anim->m_annotationTracks.getSize() > 0)
													{
														auto root_annotation = anim->m_annotationTracks[0];
														for (int ann = 0; ann < root_annotation.m_annotations.getSize(); ann++)
														{
															anim_triggers[it->second].insert(
																{
																	root_annotation.m_annotations[ann].m_time,
																	root_annotation.m_annotations[ann].m_text.cString()
																}
															);
														}
													}
													break;
												}
											}
											triggers_it = anim_triggers.find(it->second);
										}
									}
									clip_block.setPlaybackSpeed(AnimData::ClipMovementData::tes_float_cache_to_string(clip->m_playbackSpeed));
									clip_block.setCropStartTime(AnimData::ClipMovementData::tes_float_cache_to_string(clip->m_cropStartAmountLocalTime));
									clip_block.setCropEndTime(AnimData::ClipMovementData::tes_float_cache_to_string(clip->m_cropEndAmountLocalTime));
									std::multimap<float, std::string> triggers;
									if (triggers_it != anim_triggers.end())
										triggers = triggers_it->second;
									if (nullptr != clip->m_triggers)
									{

										for (int t = 0; t < clip->m_triggers->m_triggers.getSize(); ++t)
										{
											auto& trigger = clip->m_triggers->m_triggers[t];
											float time;
											if (trigger.m_relativeToEndOfClip == true)
											{
												time = duration + trigger.m_localTime;
											}
											else {
												time = trigger.m_localTime;
											}
											triggers.insert({ time, events[trigger.m_event.m_id].cString() });
										}
									}
									//load triggers inside anim
									AnimData::StringListBlock eventList; eventList.reserve(triggers.size());
									for (const auto& trigger : triggers)
									{
										eventList.append(trigger.second + ":" + AnimData::ClipMovementData::tes_float_cache_to_string(trigger.first));
									}
									clip_block.setEvents(eventList);
									clips_cache.push_back(clip_block);
								}
							}
							
							creature_entry->block.setHasAnimationCache(clips_cache.size() > 0);

							auto& set_names = creature_entry->sets.getProjectFiles();
							map<std::string, std::string> folders_crcs;
							map<std::string, std::string> animations_crcs;
							auto& blocks = creature_entry->sets.getProjectAttackBlocks();

							for (int s = 0; s < set_names.size(); ++s)
							{
								auto set_name = set_names[s];
								long long set_name_crc32 = crc_32_ll(set_name);
								AnimData::ClipFilesCRC32Block crc32Data;

								auto animations_its = _decoded_loaded_sets.equal_range({ project_index, set_name_crc32 });
								int size = std::distance(animations_its.first, animations_its.second);
								if (size > 0)
									crc32Data.reserve(size);
								for (auto animation_it = animations_its.first; animation_it != animations_its.second; animation_it++)
								{
									auto animation_path = /*inverse_crc32_map[*/animation_it->second;/*];*/
									auto full_path = "meshes" / fs::relative(base_path / animation_path, _workspace.getFolder());
									auto sanitized_name = full_path.filename().replace_extension("").string();
									transform(sanitized_name.begin(), sanitized_name.end(), sanitized_name.begin(), ::tolower);
									std::string sanitized_name_crc32; 
									auto sanitized_name_crc32_it = animations_crcs.find(sanitized_name);
									if (sanitized_name_crc32_it == animations_crcs.end())
									{
										sanitized_name_crc32 = crc_32(sanitized_name);
										animations_crcs[sanitized_name] = sanitized_name_crc32;
									}
									else {
										sanitized_name_crc32 = sanitized_name_crc32_it->second;
									}
									auto meshes_path = full_path.parent_path().string();
									transform(meshes_path.begin(), meshes_path.end(), meshes_path.begin(), ::tolower);
									std::string meshes_path_crc32;
									auto meshes_path_crc32_it = folders_crcs.find(sanitized_name);
									if (meshes_path_crc32_it == folders_crcs.end())
									{
										meshes_path_crc32 = crc_32(meshes_path);
										folders_crcs[meshes_path] = meshes_path_crc32;
									}
									else {
										meshes_path_crc32 = meshes_path_crc32_it->second;
									}
									crc32Data.append(meshes_path_crc32, sanitized_name_crc32);
								}
								blocks[s].setCrc32Data(crc32Data);
							}
						
						}					
					}
				}
			}

			project_hkx_files.append(fs::relative(char_path, base_path).string());
			fs::path rig_path = base_path / string_data->m_rigName.cString();
			if (fs::exists(rig_path))
				project_hkx_files.append(fs::relative(rig_path, base_path).string());

			entry->block.setProjectFiles(project_hkx_files);
			entry->block.setHasProjectFiles(project_hkx_files.size() > 0);

			save_cache(project_index);

		}
	}
}

hkVariant* ResourceManager::characterFileRoot(int character_index)
{
	auto& contents = get(character_index);
	for (auto& item : contents.second)
	{
		if (&hkbCharacterDataClass == item.m_class)
		{
			return &item;
		}
	}
	return nullptr;
}

void ResourceManager::close(int file_index)
{
	if (_contents.find(file_index) != _contents.end())
	{
		_contents.erase(file_index);
		_files.erase(file_index);
	}
}

int ResourceManager::behaviorFileIndex(int project_file, hkVariant* data)
{
	fs::path behavior_path = path(project_file).parent_path();
	if (data->m_class == &hkbCharacterDataClass)
	{
		hkbCharacterData* char_data = reinterpret_cast<hkbCharacterData*>(data->m_object);
		behavior_path /= char_data->m_stringData->m_behaviorFilename.cString();
	}
	else if (data->m_class == &hkbBehaviorReferenceGeneratorClass)
	{
		hkbBehaviorReferenceGenerator* brg = reinterpret_cast<hkbBehaviorReferenceGenerator*>(data->m_object);
		behavior_path /= brg->m_behaviorName.cString();
	}
	if (fs::exists(behavior_path))
	{
		get(behavior_path);
		return index(behavior_path);
	}
	return -1;
}

hkVariant* ResourceManager::behaviorFileRoot(int behavior_file)
{
	auto& contents = get(behavior_file);
	for (auto& item : contents.second)
	{
		if (&hkbBehaviorGraphClass == item.m_class)
		{
			return &item;
		}
	}
	return nullptr;
}

std::pair< hkRefPtr<const hkaSkeleton>, bool> hasRagdoll(const std::list<hkVariant>& contents)
{
	for (const auto& content : contents)
	{
		if (content.m_class == &hkaRagdollInstanceClass)
		{
			return
			{
				reinterpret_cast<hkaRagdollInstance*>(content.m_object)->m_skeleton,
				true
			};
		}
	}
	return { nullptr, false };
}

size_t ResourceManager::hasBehavior(int project_file, hkbCharacterStringData* string_data)
{
	fs::path project_path = path(project_file).parent_path();
	fs::path behavior_path = project_path / string_data->m_behaviorFilename.cString();
	if (fs::exists(behavior_path) && fs::is_regular_file(behavior_path))
		return 1;
	return 0;
}

size_t ResourceManager::hasRigAndRagdoll(int project_file, hkbCharacterStringData* string_data)
{
	size_t count = 0;
	fs::path project_path = path(project_file).parent_path();
	fs::path rig_path = project_path / string_data->m_rigName.cString();
	if (fs::exists(rig_path))
		count += 1;
	if (string_data->m_ragdollName.getLength() > 0)
	{
		fs::path ragdoll_path = project_path / string_data->m_ragdollName.cString();
		if (fs::exists(ragdoll_path))
			count += 1;
	}
	else if (fs::exists(rig_path))
	{
		//could have ragdoll inside the rig
		auto contents = get(rig_path);
		if (hasRagdoll(contents.second).second)
			count += 1;
	}
	return count;
}

size_t ResourceManager::getRigIndex(int project_file, hkbCharacterStringData* string_data)
{
	fs::path behavior_path = path(project_file).parent_path();
	behavior_path /= string_data->m_rigName.cString();
	get(behavior_path);
	return index(behavior_path);
}

hkVariant* ResourceManager::getRigRoot(int project_file, int rig_index)
{
	auto& rig_contents = get(rig_index);
	auto ragdoll_info = hasRagdoll(rig_contents.second);
	for (auto& entry : rig_contents.second)
	{
		if (entry.m_class == &hkaSkeletonClass)
		{
			if (ragdoll_info.second)
			{
				if (ragdoll_info.first.val() != entry.m_object)
					return &entry;
			}
			else {
				return &entry;
			}
		}
	}
	return nullptr;
}

size_t ResourceManager::getRagdollIndex(int project_file, const std::string& file)
{
	fs::path ragdoll_path = path(project_file).parent_path() / file;
	get(ragdoll_path);
	return index(ragdoll_path);
}

hkVariant* ResourceManager::getRagdollRoot(int project_file, int ragdoll_index)
{
	auto& ragdoll_contents = get(ragdoll_index);
	for (auto& content : ragdoll_contents.second)
	{
		if (content.m_class == &hkaRagdollInstanceClass)
		{
			return &content;
		}
	}
	return nullptr;
}



std::vector<fs::path> ResourceManager::importAssets(int project_file, const fs::path& sourcePath, AssetType type)
{
	std::vector<fs::path> out;
	return out;
}

size_t ResourceManager::assetsCount(int project_file, AssetType type)
{
	return 0;
}

void ResourceManager::clearAssetList(int project_file, AssetType type)
{

}

void ResourceManager::refreshAssetList(int project_file, AssetType type)
{

}

fs::path ResourceManager::assetFolder(int project_index, AssetType type)
{
	fs::path project_file_path = path(project_index);
	fs::path project_path = project_file_path.parent_path();
	switch (type)
	{
	case AssetType::behavior:
		return project_path / "Behaviors";
	case AssetType::animation:
		return project_path / "Animations";
	case AssetType::character_assets:
		return project_path / "Character Assets";
	default:
		break;
	}
	return project_path;
}

QStringList ResourceManager::assetsList(int project_index, AssetType type)
{
	QStringList out; out << "";
	fs::path project_file_path = path(project_index);
	fs::path project_path = project_file_path.parent_path();
	auto* project_root = getProjectRoot(project_file_path);
	fs::path char_file_path = project_path / project_root->m_characterFilenames[0].cString();
	get(char_file_path);
	auto string_data = getCharacterString(index(char_file_path));
	fs::path asset_subfolder = fs::path(string_data->m_rigName.cString()).parent_path();
	switch (type)
	{
	case AssetType::skeleton:
		break;
	case AssetType::behavior:
		asset_subfolder = "Behaviors";
		break;
	case AssetType::animation:
		asset_subfolder = "Animations";
		break;
	default:
		return {};
	}
	auto asset_folder = project_path / asset_subfolder;
	for (auto& entry : fs::recursive_directory_iterator(asset_folder))
	{
		if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".hkx")
		{
			fs::path relative = fs::relative(entry.path(), project_path);
			out << relative.string().c_str();
		}
	}
	return out;
}

QStringList ResourceManager::clipList(int project_index)
{
	HKXWrapper wrap;
	QSet<QString> out;
	fs::path project_file_path = path(project_index);
	fs::path project_path = project_file_path.parent_path();
	auto* project_root = getProjectRoot(project_file_path);
	fs::path char_file_path = project_path / project_root->m_characterFilenames[0].cString();
	get(char_file_path);
	auto string_data = getCharacterString(index(char_file_path));
	fs::path behavior_subfolder = project_path / fs::path(string_data->m_behaviorFilename.cString()).parent_path();
	for (auto& entry : fs::directory_iterator(behavior_subfolder))
	{
		if (fs::is_regular_file(entry.path()))
		{
			auto& contents = get(entry.path());
			for (const auto& content : contents.second)
			{
				if (content.m_class == &hkbClipGeneratorClass)
				{
					out << reinterpret_cast<hkbClipGenerator*>(content.m_object)->m_name.cString();
				}
			}
		}
	}
	return out.toList();
}

QStringList ResourceManager::clipAnimationsList(int project_index)
{
	HKXWrapper wrap;
	QSet<QString> out;
	fs::path project_file_path = path(project_index);
	fs::path project_path = project_file_path.parent_path();
	auto* project_root = getProjectRoot(project_file_path);
	fs::path char_file_path = project_path / project_root->m_characterFilenames[0].cString();
	get(char_file_path);
	auto string_data = getCharacterString(index(char_file_path));
	fs::path behavior_subfolder = project_path / fs::path(string_data->m_behaviorFilename.cString()).parent_path();
	for (auto& entry : fs::directory_iterator(behavior_subfolder))
	{
		if (fs::is_regular_file(entry.path()))
		{
			auto& contents = get(entry.path());
			for (const auto& content : contents.second)
			{
				if (content.m_class == &hkbClipGeneratorClass)
				{
					out << reinterpret_cast<hkbClipGenerator*>(content.m_object)->m_animationName.cString();
				}
			}
		}
	}
	return out.toList();
}

QStringList ResourceManager::attackEventList(int project_index)
{
	HKXWrapper wrap;
	QSet<QString> out;
	fs::path project_file_path = path(project_index);
	fs::path project_path = project_file_path.parent_path();
	auto* project_root = getProjectRoot(project_file_path);
	fs::path char_file_path = project_path / project_root->m_characterFilenames[0].cString();
	get(char_file_path);
	auto string_data = getCharacterString(index(char_file_path));
	fs::path behavior_subfolder = project_path / fs::path(string_data->m_behaviorFilename.cString()).parent_path();
	for (auto& entry : fs::directory_iterator(behavior_subfolder))
	{
		if (fs::is_regular_file(entry.path()))
		{
			auto& contents = get(entry.path());
			for (const auto& content : contents.second)
			{
				if (content.m_class == &hkbBehaviorGraphStringDataClass)
				{
					hkbBehaviorGraphStringData* string_data = reinterpret_cast<hkbBehaviorGraphStringData*>(content.m_object);
					for (int e = 0; e < string_data->m_eventNames.getSize(); ++e)
					{
						QString eventName = string_data->m_eventNames[e].cString();
						if (
							eventName.startsWith("attackStart") ||
							eventName.startsWith("attackPowerStart") ||
							eventName.startsWith("bashStart") ||
							eventName.startsWith("bashPowerStart")
							)
						{
							out << eventName;
						}
					}
					break;
				}
			}
		}
	}
	return out.toList();
}

QStringList ResourceManager::wordVariableList(int project_index)
{
	HKXWrapper wrap;
	QSet<QString> out;
	fs::path project_file_path = path(project_index);
	fs::path project_path = project_file_path.parent_path();
	auto* project_root = getProjectRoot(project_file_path);
	fs::path char_file_path = project_path / project_root->m_characterFilenames[0].cString();
	get(char_file_path);
	auto string_data = getCharacterString(index(char_file_path));
	fs::path behavior_subfolder = project_path / fs::path(string_data->m_behaviorFilename.cString()).parent_path();
	for (auto& entry : fs::directory_iterator(behavior_subfolder))
	{
		if (fs::is_regular_file(entry.path()))
		{
			auto& contents = get(entry.path());
			for (const auto& content : contents.second)
			{
				if (content.m_class == &hkbBehaviorGraphDataClass)
				{
					hkbBehaviorGraphData* data = reinterpret_cast<hkbBehaviorGraphData*>(content.m_object);
					hkbBehaviorGraphStringData* string_data = data->m_stringData;
					for (int v = 0; v < data->m_variableInfos.getSize(); ++v)
					{
						if (data->m_variableInfos[v].m_type == hkbVariableInfo::VARIABLE_TYPE_BOOL ||
							data->m_variableInfos[v].m_type == hkbVariableInfo::VARIABLE_TYPE_INT8 ||
							data->m_variableInfos[v].m_type == hkbVariableInfo::VARIABLE_TYPE_INT16 ||
							data->m_variableInfos[v].m_type == hkbVariableInfo::VARIABLE_TYPE_INT32 ||
							data->m_variableInfos[v].m_type == hkbVariableInfo::VARIABLE_TYPE_REAL)
						{
							out << string_data->m_variableNames[v].cString();
						}
					}
					break;
				}
			}
		}
	}
	return out.toList();
}

/* CACHE SETS */

size_t ResourceManager::getAnimationSetsFiles(int project_file)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return entry->sets.getProjectFiles().size();
	}
	return 0;
}
QString ResourceManager::getAnimationSetsFile(int project_file, int index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return QString::fromStdString(entry->sets.getProjectFiles()[index]);
	}
	return "";
}

void ResourceManager::createAnimationSet(int project_file, const QString& name) //add .txt
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		std::string std_name = name.toUtf8().constData();
		entry->sets.putProjectAttack(std_name, AnimData::ProjectAttackBlock());
	}
}

void ResourceManager::deleteAnimationSet(int project_file, int index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.removeProjectAttack(index);
	}
}

size_t ResourceManager::getAnimationSetEvents(int project_file, int set_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return entry->sets.getBlockEvents(set_index);
	}
	return 0;
}

QString ResourceManager::getAnimationSetEvent(int project_file, int set_index, int event_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return QString::fromStdString(entry->sets.getBlockEvent(set_index, event_index));
	}
	return "";
}

void ResourceManager::addAnimationSetEvent(int project_file, int set_index, const QString& event_name)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.addBlockEvent(set_index, event_name.toUtf8().constData());
	}
}

void ResourceManager::deleteAnimationSetEvent(int project_file, int set_index, int event_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.removeBlockEvent(set_index, event_index);
	}
}

size_t ResourceManager::getAnimationSetVariables(int project_file, int set_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return entry->sets.getBlockVariables(set_index);
	}
	return 0;
}

QString ResourceManager::getAnimationSetVariable(int project_file, int set_index, int variable_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return QString::fromStdString(entry->sets.getBlockVariable(set_index, variable_index));
	}
	return "";
}

int ResourceManager::getAnimationSetVariableMin(int project_file, int set_index, int variable_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return entry->sets.getBlockVariableMin(set_index, variable_index);
	}
	return 0;
}

int ResourceManager::getAnimationSetVariableMax(int project_file, int set_index, int variable_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return entry->sets.getBlockVariableMax(set_index, variable_index);
	}
	return 0;
}

void ResourceManager::setAnimationSetVariableMin(int project_file, int set_index, int variable_index, int min_value)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.setBlockVariableMin(set_index, variable_index, min_value);
	}
}

void ResourceManager::setAnimationSetVariableMax(int project_file, int set_index, int variable_index, int max_value)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.setBlockVariableMax(set_index, variable_index, max_value);
	}
}

void ResourceManager::addAnimationSetVariable(int project_file, int set_index, const QString& variable_name, int min_value, int max_value)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.addBlockVariable(set_index, variable_name.toUtf8().constData(), min_value, max_value);
	}
}

void ResourceManager::deleteAnimationSetVariable(int project_file, int set_index, int variable_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.removeBlockVariable(set_index, variable_index);
	}
}

QStringList ResourceManager::getAnimationSetAnimation(int project_file, int set_index)
{
	QStringList out;
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		auto set_name = entry->sets.getProjectFiles()[set_index];
		auto crc32_ll = crc_32_ll(set_name);
		auto its = _decoded_loaded_sets.equal_range({ project_file, crc32_ll });
		for (auto it = its.first; it != its.second; it++)
		{
			out << QString::fromStdString(it->second);
		}
	}
	return out;
}

void ResourceManager::addAnimationSetAnimation(int project_file, int set_index, const QString& animation_path)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		auto set_name = entry->sets.getProjectFiles()[set_index];
		auto crc32_ll = crc_32_ll(set_name);
		_decoded_loaded_sets.insert({ { project_file, crc32_ll }, animation_path.toUtf8().constData() });
	}
}

void ResourceManager::deleteAnimationSetAnimation(int project_file, int set_index, const QString& animation_path)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		std::string to_delete = animation_path.toUtf8().constData();
		auto set_name = entry->sets.getProjectFiles()[set_index];
		auto crc32_ll = crc_32_ll(set_name);
		auto its = _decoded_loaded_sets.equal_range({ project_file, crc32_ll });
		for (auto it = its.first; it != its.second; it)
		{
			if (to_delete == it->second)
			{
				it = _decoded_loaded_sets.erase(it);
			}
			else {
				it++;
			}
		}
	}
}

size_t ResourceManager::getAnimationSetAttacks(int project_file, int set_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return entry->sets.getBlockAttacks(set_index);
	}
	return 0;
}

QString ResourceManager::getAnimationSetAttackEvent(int project_file, int set_index, int attack_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return QString::fromStdString(entry->sets.getBlockAttackEvent(set_index, attack_index));
	}
	return "";
}

void ResourceManager::setAnimationSetAttackEvent(int project_file, int set_index, int attack_index, const QString& attack_event)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.setBlockAttackEvent(set_index, attack_index, attack_event.toUtf8().constData());
	}
}

void ResourceManager::addAnimationSetAttack(int project_file, int set_index, const QString& attack_event)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.addBlockAttack(set_index, attack_event.toUtf8().constData());
	}
}

void ResourceManager::deleteAnimationSetAttack(int project_file, int set_index, int attack_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.removeBlockAttack(set_index, attack_index);
	}
}

size_t ResourceManager::getAnimationSetAttackClips(int project_file, int set_index, int attack_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return entry->sets.getBlockAttackClips(set_index, attack_index);
	}
	return 0;
}

QString ResourceManager::getAnimationSetAttackClip(int project_file, int set_index, int attack_index, int clip_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		return QString::fromStdString(entry->sets.getBlockAttackClip(set_index, attack_index, clip_index));
	}
	return "";
}

void ResourceManager::addAnimationSetAttackClip(int project_file, int set_index, int attack_index, const QString& clip_generator_name)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.addBlockAttackClip(set_index, attack_index, clip_generator_name.toUtf8().constData());
	}
}

void ResourceManager::deleteAnimationSetAttackClip(int project_file, int set_index, int attack_index, int clip_index)
{
	auto entry = dynamic_cast<CreatureCacheEntry*>(findCacheEntry(project_file));
	if (entry != nullptr)
	{
		entry->sets.removeBlockAttackClip(set_index, attack_index, clip_index);
	}
}

void ResourceManager::setAnimationMovementData(int project_file, const std::string& animation_row, const AnimData::root_movement_t& movement)
{
	auto _animation_row = animation_row;
	long long path_crc32 = crc_32_ll(_animation_row);
	std::pair<size_t, long long> ins = { project_file, path_crc32 };
	_animations_root_movements[ins] = movement;
}

const AnimData::root_movement_t& ResourceManager::getAnimationMovementData(int project_file, const std::string& animation_name)
{
	auto _animation_row = animation_name;
	long long path_crc32 = crc_32_ll(_animation_row);
	std::pair<size_t, long long> find = { project_file, path_crc32 };
	if (_animations_root_movements.find(find) != _animations_root_movements.end())
		return _animations_root_movements.at(find);
	return AnimData::root_movement_t();
}

//hkTypeInfoRegistry::getInstance()
void* ResourceManager::createObject(int file, const hkClass* hkclass, const std::string& name)
{
	hkVariant v;
	v.m_object = malloc(hkclass->getObjectSize());
	if (v.m_object != nullptr)
	{
		memset(v.m_object, 0, hkclass->getObjectSize());
		v.m_class = hkclass;
		auto info = hkTypeInfoRegistry::getInstance().finishLoadedObject(v.m_object, hkclass->getName());
		HkxTableVariant vv(v);
		vv.setName(name.c_str());
		int rows = vv.rows();
		for (int i = 0; i < rows; i++)
		{
			int columns = vv.columns(i);
			for (int j = 0; j < columns; j++)
			{
				auto value = vv.data(i, j);
				if (value.canConvert<HkxItemEvent>() ||
					value.canConvert<HkxItemVar>() ||
					value.canConvert<HkxItemBone>() ||
					value.canConvert<HkxItemRagdollBone>())
				{
					vv.setData(i, j, -1);
				}
			}
		}
		initializeValues(v.m_object, hkclass);
		_contents[file].second.push_back(v);
	}
	return v.m_object;
}

QStringList ResourceManager::getStates(hkbStateMachine* fsm)
{
	QStringList out;
	std::map<int, QString> ordered_states;
	for (int s = 0; s < fsm->m_states.getSize(); ++s)
	{
		auto state = fsm->m_states[s];
		ordered_states[fsm->m_states[s]->m_stateId] = fsm->m_states[s]->m_name.cString();
	}
	if (!ordered_states.empty())
	{
		int max_states = ordered_states.rbegin()->first + 1;
		for (int i = 0; i < max_states; i++)
		{
			if (ordered_states.find(i) != ordered_states.end())
			{
				out << ordered_states.at(i);
			}
			else
			{
				out << "Invalid State";
			}
		}
	}
	return out;
}

bool ResourceManager::removeAsset(int file_index, AssetType type, int asset_index)
{
	MemberIndexType indexType;
	if (type == AssetType::events)
	{
		indexType = MemberIndexType::eventIndex;
	}
	else if (type == AssetType::variables)
	{
		indexType = MemberIndexType::variableIndex;
	}
	else {
		return false;
	}
	auto& contents = get(file_index);
	for (auto& object : contents.second)
	{
		HkxVariant v(object);
		ReferenceSetter s(asset_index, -1, indexType);
		v.accept(s);
	}
	return true;
}

fs::path ResourceManager::makeExportPath(const fs::path& absolute_file, const fs::path& export_folder)
{
	fs::path relative_project_path = fs::relative(absolute_file, _workspace.getFolder());
	fs::path dest = export_folder / relative_project_path;
	fs::create_directories(dest.parent_path());
	return dest;
}

void ResourceManager::exportProject(int row, ProjectType type, ExportType export_to)
{
	fs::path fs_path = projectPath(row, type);
	int project_index = index(fs_path);
	exportProject(project_index, export_to);
}

void copyAsset(const fs::path& _From, const fs::path& _To, ExportType export_to)
{
	if (export_to == ExportType::SE)
	{
		HKXWrapper wrap;
		auto root = wrap.read(_From);
		wrap.write_se_only(root, _To);
	}
	else {
		fs::copy(_From, _To, fs::copy_options::overwrite_existing);
	}
}

void ResourceManager::exportProject(int project_index, ExportType export_to)
{
	fs::path export_path = fs::path(Settings.get<std::string>("/general/skyrim_le_folder")) / "meshes";
	if (export_to == ExportType::SE)
	{
		export_path = Games::Instance().data(Games::TES5SE);
		if (!fs::exists(export_path))
			return;
		export_path = export_path / "meshes";
	}
	fs::create_directories(export_path);
	if (fs::exists(export_path))
	{
		fs::path project_path = path(project_index);
		fs::path base_path = project_path.parent_path();
		fs::path relative_project_path = fs::relative(project_path, base_path);

		//Project
		copyAsset(project_path, makeExportPath(project_path, export_path), export_to);

		//Character
		auto& project_contents = get(project_path);
		hkbProjectStringData* project_data = getProjectRoot(project_index);
		if (nullptr != project_data && project_data->m_characterFilenames.getSize())
		{
			//copy character
			fs::path char_path = base_path / project_data->m_characterFilenames[0].cString();
			copyAsset(char_path, makeExportPath(char_path, export_path), export_to);

			int character_index = index(char_path);
			if (character_index == -1)
			{
				get(char_path);
			}
			auto* string_data = getCharacterString(character_index);
			if (nullptr != string_data)
			{
				fs::path behavior_path = base_path / string_data->m_behaviorFilename.cString();
				fs::path behavior_folder = behavior_path.parent_path();
				for (auto const& dir_entry : std::filesystem::directory_iterator{ behavior_folder })
				{
					if (fs::is_regular_file(dir_entry.path()))
					{
						//copy all behaviors, just in case
						copyAsset(dir_entry.path(), makeExportPath(dir_entry.path(), export_path), export_to);
					}
				}
				//copy animation
				for (int i = 0; i < string_data->m_animationNames.getSize(); ++i)
				{
					fs::path animation_path = base_path / string_data->m_animationNames[i].cString();
					if (fs::exists(animation_path))
						copyAsset(animation_path, makeExportPath(animation_path, export_path), export_to);
				}
				//copy rig
				fs::path rig_path = base_path / string_data->m_rigName.cString();
				if (fs::exists(rig_path))
				{
					copyAsset(rig_path, makeExportPath(rig_path, export_path), export_to);
				}
			}
		}
		//txts
		fs::path animationdatasinglefile = _workspace.getFolder() / "animationdatasinglefile.txt";
		fs::copy(animationdatasinglefile, export_path / "animationdatasinglefile.txt", fs::copy_options::overwrite_existing);
		fs::path animationsetdatasinglefile = _workspace.getFolder() / "animationsetdatasinglefile.txt";
		fs::copy(animationsetdatasinglefile, export_path / "animationsetdatasinglefile.txt", fs::copy_options::overwrite_existing);
	}
}


void createBehaviorFile(const fs::path& out, const std::string& behavior_name)
{
	if (!fs::exists(out))
	{
		hkbBehaviorGraphStringData* string_data = new hkbBehaviorGraphStringData();
		hkbBehaviorGraphData* data = new hkbBehaviorGraphData();
		data->m_stringData = string_data;
		hkbBehaviorGraph* graph = new hkbBehaviorGraph();
		graph->m_data = data;
		graph->m_name = behavior_name.c_str();
		hkRootLevelContainer* root = new hkRootLevelContainer();
		root->m_namedVariants.pushBack(
			{
				hkbBehaviorGraphClass.getName(),
				graph,
				&hkbBehaviorGraphClass
			}
		);
		HKXWrapper wrap;
		wrap.write_le_se(root, out);
		delete string_data;
		delete data;
		delete graph;
		delete root;
	}
}

void ResourceManager::CreateNewBehaviorFile(int project_index, const QString& behavior_name)
{
	fs::path behaviors_path = assetFolder(project_index, AssetType::behavior);
	fs::path out = behaviors_path / behavior_name.toUtf8().constData(); out.replace_extension(".hkx");
	createBehaviorFile(out, behavior_name.toUtf8().constData());
}

void createProjectFile(const fs::path& out, const std::string& character_file)
{
	if (!fs::exists(out))
	{
		hkbProjectStringData* string_data = new hkbProjectStringData();
		string_data->m_characterFilenames.pushBack(character_file.c_str());
		hkbProjectData* data = new hkbProjectData();
		data->m_stringData = string_data;
		hkRootLevelContainer* root = new hkRootLevelContainer();
		root->m_namedVariants.pushBack(
			{
				hkbProjectDataClass.getName(),
				data,
				&hkbProjectDataClass
			}
		);
		HKXWrapper wrap;
		wrap.write_le_se(root, out);
		delete string_data;
		delete data;
		delete root;
	}
}

void createCharacterFile(const fs::path& out, const std::string& behavior_file, const std::string& character_name)
{
	if (!fs::exists(out))
	{
		hkbMirroredSkeletonInfo* minfo = new hkbMirroredSkeletonInfo();
		hkbCharacterStringData* string_data = new hkbCharacterStringData();
		string_data->m_name = character_name.c_str();
		string_data->m_behaviorFilename - behavior_file.c_str();
		hkbVariableValueSet* properties_values = new hkbVariableValueSet();
		//string_data->m_characterFilenames.pushBack(character_file.c_str());
		hkbCharacterData* data = new hkbCharacterData();
		data->m_modelUpMS = hkVector4( 0., 0., 1., 0. );
		data->m_modelForwardMS = hkVector4(0., 1., 0., 0.);
		data->m_modelRightMS = hkVector4(1., 0., 0., 0.);
		data->m_characterPropertyValues = properties_values;
		data->m_stringData = string_data;
		data->m_mirroredSkeletonInfo = minfo;
		hkRootLevelContainer* root = new hkRootLevelContainer();
		root->m_namedVariants.pushBack(
			{
				hkbCharacterDataClass.getName(),
				data,
				& hkbCharacterDataClass
			}
		);
		HKXWrapper wrap;
		wrap.write_le_se(root, out);
		delete minfo;
		delete string_data;
		delete properties_values;
		delete data;
		delete root;
	}
}

void ResourceManager::CreateNewProject(const QString& project_name, const QString& sub_folder, ProjectType type)
{
	std::string name = project_name.toUtf8().constData();
	std::string folder = sub_folder.toUtf8().constData();
	fs::path project_dir = _workspace.getFolder() / folder / name;
	if (!fs::exists(project_dir))
	{
		fs::create_directories(project_dir);
		fs::create_directories(project_dir / "Animations");
		fs::create_directories(project_dir / "Characters");
		fs::create_directories(project_dir / "Character Assets");
		fs::create_directories(project_dir / "Behaviors");

		fs::path project_file = project_dir / (name + ".hkx");
		fs::path character_file = fs::path("Characters") / (name + "Character.hkx");
		fs::path behavior_file = fs::path("Behaviors") / (name + "Behavior.hkx");

		createProjectFile(project_file, character_file.string());
		createCharacterFile(project_dir / character_file, behavior_file.string(), name);
		createBehaviorFile(project_dir / behavior_file, name + "Behavior");

		auto entry = _cache.findOrCreate(name, type == ProjectType::character);
		fs::path animationDataPath = "animationdatasinglefile.txt";
		fs::path animationSetDataPath = "animationsetdatasinglefile.txt";
		_cache.save_creature(
			name, entry, animationDataPath, animationSetDataPath, _workspace.getFolder()
		);
		if (type == ProjectType::character)
			_workspace.addCharacterProject(project_file.string().c_str(), project_name);
		else
			_workspace.addMiscellaneousProject(project_file.string().c_str(), project_name);
	}
}

