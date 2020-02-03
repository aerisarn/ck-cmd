#ifndef ESP_H
#define ESP_H

// Command Base
#include <commands/CommandBase.h>

#include <commands\esp\data\EspFile.h>
#include <filesystem>
namespace fs = std::experimental::filesystem;

class CreateEsp : public CommandBase<CreateEsp>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(CreateEsp)

private:
    CreateEsp();
    virtual ~CreateEsp();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};

class Esp
{
public:
	Esp(std::string name, fs::path path, bool isMaster);
	Esp(std::string name, bool isMaster);
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