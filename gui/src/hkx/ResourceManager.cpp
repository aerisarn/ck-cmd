#include "ResourceManager.h"

using namespace ckcmd::HKX;

ResourceManager::ResourceManager(const fs::path& workspace_folder) :
	_workspace_folder(workspace_folder)
{

}

size_t ResourceManager::index(const fs::path& file) const {
	auto index = std::distance(
		_files.begin(),
		std::find(_files.begin(), _files.end(), file)
	);
	return index;
}

int ResourceManager::findIndex(int file_index, const void* object) const
{
	auto& _objects = _contents.at(file_index).second;
	for (int i = 0; i < _objects.getSize(); i++) {
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
	return &_contents[index(file)].second[_index];
}

fs::path ResourceManager::open(const std::string& project)
{
	std::string xml_name = fs::path(project).replace_extension(".xml").string();
	std::string hkx_name = fs::path(project).replace_extension(".hkx").string();
	for (auto& p : fs::recursive_directory_iterator(_workspace_folder))
	{
		if (p.path().filename() == xml_name) {
			if (p.path().string().find("characters") != string::npos)
				continue;
			return p.path();
		}
		else if (p.path().filename() == hkx_name) {
			if (p.path().string().find("characters") != string::npos)
				continue;
			return p.path();
		}
	}
	return fs::path();
}

hkx_file_t& ResourceManager::get(const fs::path& file)
{
	if (std::find(_files.begin(), _files.end(), file) == _files.end()) {
		HKXWrapper wrap;
		fs::path xml_path = file;  xml_path.replace_extension(".xml");
		fs::path hkx_path = file;  hkx_path.replace_extension(".hkx");
		
		hkRootLevelContainer* root;
		hkx_file_t new_file;
		if (fs::exists(xml_path))
		{
			root = wrap.read(xml_path, new_file.second);
		}
		else if (fs::exists(hkx_path))
			root = wrap.read(hkx_path, new_file.second);
		else
			throw std::runtime_error("Unable to find: " + file.string());

		for (int i = 0; i < new_file.second.getSize(); i++) {
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