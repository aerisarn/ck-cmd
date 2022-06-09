#include <src/Skyrim/TES5File.h>
#include <src/Collection.h>
#include <src/ModFile.h>

#include <src/config.h>

#include "ResourceManager.h"

#include <hkbProjectData_2.h>

using namespace ckcmd::HKX;

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
	auto index = std::distance(
		_files.begin(),
		std::find(_files.begin(), _files.end(), file)
	);
	return index;
}

fs::path ResourceManager::path(int file_index) const
{
	return _files.at(file_index);
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
	if (std::find(_files.begin(), _files.end(), file) == _files.end()) {
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
		_contents[_files.size()] = new_file;
		_files.push_back(file);
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

const std::string& ResourceManager::get_sanitized_name(int file_index) {
	if (_sanitized_names.find(file_index) == _sanitized_names.end())
	{
		auto p = _files.at(file_index);
		string sanitized_project_name = p.filename().replace_extension("").string();
		_sanitized_names[file_index] = sanitized_project_name;
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

CacheEntry* ResourceManager::findOrCreateCacheEntry(size_t file_index, bool character)
{
	return _cache.findOrCreate(get_sanitized_name(file_index), character);
}

CacheEntry* ResourceManager::findCacheEntry(size_t file_index)
{
	return _cache.find(get_sanitized_name(file_index));
}

const AnimData::ClipMovementData& ResourceManager::getMovement(size_t file_index, string clip) {
	return _cache.getMovement(get_sanitized_name(file_index), clip );
}

vector<AnimationCache::event_info_t> ResourceManager::getEventsInfo(size_t file_index, string anim_event) 
{
	return _cache.getEventsInfo(get_sanitized_name(file_index), anim_event);
}

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

void ResourceManager::scanWorkspace()
{

	for (auto& p : fs::recursive_directory_iterator(_workspace.getFolder()))
	{
		if (fs::is_regular_file(p.path())
			&& (p.path().extension() == ".hkx" || p.path().extension() == ".xml")
			)
		{
			//LOG << "Analyzing file ... " << p.path().string() << log_endl;
			if (isHavokProject(p.path())) {
				string sanitized_project_name = p.path().filename().replace_extension("").string();
				//LOG << "Found project " << sanitized_project_name << log_endl;
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
		}
	}
}

bool ResourceManager::isCharacterFileOpen(int index)
{
	QString path = _characters.at(index);
	fs::path fs_path = path.toUtf8().constData();
	return std::find(_files.begin(), _files.end(), fs_path) != _files.end();
}

bool ResourceManager::isMiscFileOpen(int index)
{
	QString path = _miscellaneous.at(index);
	fs::path fs_path = path.toUtf8().constData();
	return std::find(_files.begin(), _files.end(), fs_path) != _files.end();
}

void ResourceManager::openCharacterFile(int index)
{
	QString path = _characters.at(index);
	fs::path fs_path = path.toUtf8().constData();
	get(fs_path);
}

size_t ResourceManager::characterFileIndex(int row)
{
	QString path = _characters.at(row);
	fs::path fs_path = path.toUtf8().constData();
	return index(fs_path);
}

void ResourceManager::closeCharacterFile(int string_index)
{
	QString path = _characters.at(string_index);
	fs::path fs_path = path.toUtf8().constData();
	int internal_index = index(fs_path);
	_contents.erase(internal_index);
	_files.erase(_files.begin() + internal_index);
}

void ResourceManager::openMiscFile(int index) 
{
	QString path = _miscellaneous.at(index);
	fs::path fs_path = path.toUtf8().constData();
	get(fs_path);
}

void ResourceManager::closeMiscFile(int index) {

}

size_t ResourceManager::miscFileIndex(int row)
{
	QString path = _miscellaneous.at(row);
	fs::path fs_path = path.toUtf8().constData();
	return index(fs_path);
}

