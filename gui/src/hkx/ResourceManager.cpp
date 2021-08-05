#include "ResourceManager.h"

using namespace ckcmd::HKX;

ResourceManager::ResourceManager(const fs::path& workspace_folder) :
	_workspace_folder(workspace_folder)
{

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
	if (_files.find(file) == _files.end()) {
		HKXWrapper wrap;
		fs::path xml_path = file;  xml_path.replace_extension(".xml");
		fs::path hkx_path = file;  hkx_path.replace_extension(".hkx");
		
		hkx_file_t new_file;
		if (fs::exists(xml_path))
			new_file.first = wrap.read(xml_path, new_file.second);
		else if (fs::exists(hkx_path))
			new_file.first = wrap.read(hkx_path, new_file.second);
		else
			throw std::runtime_error("Unable to find: " + file.string());
		
		_files[file] = new_file;
	}
	return _files[file];
}