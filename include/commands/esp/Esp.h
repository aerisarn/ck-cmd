#ifndef ESP_H
#define ESP_H

// Command Base
#include <commands/CommandBase.h>

#include <commands\esp\data\EspFile.h>
#include <filesystem>

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

class CreateEsp : public Command<CreateEsp>
{
    REGISTER_COMMAND_HEADER(CreateEsp)

private:
    CreateEsp();
    virtual ~CreateEsp();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
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