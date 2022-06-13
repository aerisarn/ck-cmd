#include <src/Skyrim/TES5File.h>
#include <src/Collection.h>
#include <src/ModFile.h>

#include <src/config.h>

#include "ResourceManager.h"

#include <hkbProjectData_2.h>
#include <hkbCharacterData_7.h>
#include <hkbBehaviorReferenceGenerator_0.h>
#include <hkbBehaviorGraph_1.h>
#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
#include <Animation/Animation/hkaAnimationContainer.h>
#include <hkbClipGenerator_2.h>

#include <atomic>

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

	_characters = _workspace.getCharacterProjects();
	_miscellaneous = _workspace.getMiscellaneousProjects();

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
}

ResourceManager::~ResourceManager() 
{
	delete _esp;
}

size_t ResourceManager::index(const fs::path& file) const {
	for (const auto& entry : _files)
	{
		if (entry.second == file)
		{
			return entry.first;
		}
	}
	return (size_t)-1;
}

fs::path ResourceManager::path(int file_index) const
{
	return _files.at(file_index);
}

bool ResourceManager::is_open(const fs::path& file) const {
	return std::find_if(_files.begin(), _files.end(), [file](const auto& entry) {return file == entry.second; }) != _files.end();
}

int ResourceManager::findIndex(int file_index, const void* object) const
{
	auto& _objects = _contents.at(file_index).second;
	for (int i = 0; i < _objects.size(); i++) {
		if (_objects[i].m_object == object)
			return i;
	}
	return -1;
}

int ResourceManager::findIndex(const fs::path& file, const void* object) const
{
	return findIndex(index(file), object);
}

hkVariant* ResourceManager::at(const fs::path& file, size_t _index) {
	return const_cast<hkVariant*>(static_cast<const ResourceManager&>(*this).at(file, _index));
}

const hkVariant* ResourceManager::at(const fs::path& file, size_t _index) const {
	return &_contents.at(index(file)).second[_index];
}

hkVariant* ResourceManager::at(size_t file_index, size_t _index) {
	return const_cast<hkVariant*>(static_cast<const ResourceManager&>(*this).at(file_index, _index));
}

const hkVariant* ResourceManager::at(size_t file_index, size_t _index) const {
	return &_contents.at(file_index).second[_index];
}

hk_object_list_t ResourceManager::findCompatibleNodes(size_t file_index, const hkClassMember* member_class) const
{
	hk_object_list_t result;
	auto& file_contents = _contents.at(file_index).second;
	for (int i = 0; i < file_contents.size(); i++) {
		const auto& object = file_contents[i];
		if (member_class->getClass() == object.m_class ||
			member_class->getClass()->isSuperClass(*object.m_class)) {
			result.push_back({ i, &object });
		}
	}
	return result;
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
		new_file.second.resize(to_read.getSize());
		for (int i = 0; i < to_read.getSize(); i++) {
			new_file.second[i] = to_read[i];
		}

		for (int i = 0; i < new_file.second.size(); i++) {
			if (new_file.second[i].m_object == root)
			{
				new_file.first = new_file.second[i];
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

//CacheEntry* ResourceManager::findOrCreateCacheEntry(size_t file_index, bool character)
//{
//	return _cache.findOrCreate(get_sanitized_name(file_index), character);
//}
//

//
//const AnimData::ClipMovementData& ResourceManager::getMovement(size_t file_index, string clip) {
//	return _cache.getMovement(get_sanitized_name(file_index), clip );
//}
//
//vector<AnimationCache::event_info_t> ResourceManager::getEventsInfo(size_t file_index, string anim_event) 
//{
//	return _cache.getEventsInfo(get_sanitized_name(file_index), anim_event);
//}

void ResourceManager::save_cache(int file_index)
{
	string project = get_sanitized_name(file_index);
	CacheEntry* project_entry = findCacheEntry(project);
	fs::path animationDataPath = "animationdatasinglefile.txt";
	fs::path animationSetDataPath = "animationsetdatasinglefile.txt";
	fs::create_directories(_workspace.getFolder() / "test");
	_cache.save_creature(
		project, project_entry, animationDataPath, animationSetDataPath, _workspace.getFolder() / "test"
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

const QString& ResourceManager::projectPath(int row, ProjectType type)
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
	return QString();
}

bool ResourceManager::isProjectFileOpen(int row, ProjectType type)
{
	QString path = projectPath(row, type);
	fs::path fs_path = path.toUtf8().constData();
	return is_open(fs_path);
}

void ResourceManager::openProjectFile(int row, ProjectType type)
{
	QString path = projectPath(row, type);
	fs::path fs_path = path.toUtf8().constData();
	if (fs::exists(fs_path))
	{
		get(fs_path);
	}
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
	QString path = projectPath(row, type);
	fs::path fs_path = path.toUtf8().constData();
	int project_index = index(fs_path);
	//remove animation sets
	_animation_sets.erase(project_index);
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
				auto to_erase = _decoded_loaded_sets.equal_range({ project_index, a });
				if (to_erase.first != to_erase.second)
					_decoded_loaded_sets.erase(to_erase.first, to_erase.second);

				auto clip_to_erase = _animations_root_movements.equal_range({ project_index, a });
				if (clip_to_erase.first != clip_to_erase.second)
					_animations_root_movements.erase(clip_to_erase.first, clip_to_erase.second);
			}
		}
	}
	close(project_index);
}

size_t ResourceManager::projectFileIndex(int row, ProjectType type)
{
	QString path = projectPath(row, type);
	fs::path fs_path = path.toUtf8().constData();
	int project_index = index(fs_path);
	if (_animation_sets.find(project_index) == _animation_sets.end())
	{
		string sanitized_project_name = get_sanitized_name(project_index);
		CacheEntry* entry = findCacheEntry(sanitized_project_name);
		CreatureCacheEntry* creature_entry = dynamic_cast<CreatureCacheEntry*>(entry);
		if (nullptr != creature_entry)
		{
			_animation_sets.insert({ project_index, &creature_entry->sets });
		}
	}
	return index(fs_path);
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


size_t ResourceManager::characterFileIndex(int row, ProjectType type)
{
	size_t result = -1;
	QString path = projectPath(row, type);
	fs::path fs_path = path.toUtf8().constData();
	int project_index = index(fs_path);
	hkbProjectStringData* project_data = nullptr;
	hkbCharacterStringData* string_data = nullptr;
	if (project_index != (size_t)-1)
	{
		project_data = getProjectRoot(project_index);
	}
	else if (fs::exists(fs_path))
	{
		project_data = getProjectRoot(fs_path);
	}
	bool already_open = false;
	if (nullptr != project_data && project_data->m_characterFilenames.getSize())
	{
		fs::path char_path = fs_path.parent_path() / project_data->m_characterFilenames[0].cString();
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
			auto sets = _animation_sets[project_index];
			size_t sets_size = sets->getProjectFiles().getStrings().size();
			CacheEntry* entry = findCacheEntry(project_index);
			std::map<std::string, int> crc32_map;
			std::map<std::string, int, ci_less> index_map;
			for (int a = 0; a < string_data->m_animationNames.getSize(); ++a)
			{
				std::string anim_name = fs::path(string_data->m_animationNames[a].cString()).filename().replace_extension("").string();
				std::string anim_crc32 = crc_32(anim_name);
				crc32_map[anim_crc32] = a;
				index_map[string_data->m_animationNames[a].cString()] = a;
			}
#pragma parallel for
			for (int s = 0; s < sets_size; ++s)
			{
				auto& crcs = sets->getProjectAttackBlocks()[s].getCrc32Data().getStrings();
				for (int c = 0; c < crcs.size(); ++c)
				{
					if (c % 3 == 1)
					{
						_decoded_loaded_sets.insert({ {(size_t)project_index, crc32_map[crcs.at(c)]}, (size_t)s });
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
									auto movement = entry->findMovement(clip->m_name.cString());
									auto it = index_map.find(clip->m_animationName.cString());
									if (!movement.empty() && it != index_map.end())
										_animations_root_movements[{project_index, it->second}] = movement;
							}
						}
					}
				}
			}
		}
	}
	return result;
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

size_t ResourceManager::behaviorFileIndex(int project_file, hkVariant* data)
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
	get(behavior_path);
	return index(behavior_path);
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
}

std::pair< hkRefPtr<const hkaSkeleton>, bool> hasRagdoll(const std::vector<hkVariant>& contents)
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


size_t ResourceManager::hasRigAndRagdoll(int project_file, hkbCharacterStringData* string_data)
{
	size_t count = 0;
	fs::path behavior_path = path(project_file).parent_path();
	fs::path rig_path = behavior_path / string_data->m_rigName.cString();
	if (fs::exists(rig_path))
		count += 1;
	if (string_data->m_ragdollName.getLength() > 0)
	{
		fs::path ragdoll_path = behavior_path / string_data->m_ragdollName.cString();
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

