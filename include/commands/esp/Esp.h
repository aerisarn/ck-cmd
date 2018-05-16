#ifndef ESP_H
#define ESP_H

#include <commands\esp\data\EspFile.h>
#include <filesystem>
namespace fs = std::experimental::filesystem;

class Esp
{
public:
	Esp(std::string name, fs::path path, bool isMaster);
	~Esp();

	void Save();

	void SetDescription(std::string desc);
	void SetAuthor(std::string author);

private:
	void CreatePlugin();
	void CreateMaster();

	EspFile* esp;
	std::string name;
	fs::path path;
	bool isMaster;
};

#endif //ESP_H