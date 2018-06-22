#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class Project : public CommandBase
{
    REGISTER_COMMAND_HEADER(Project)

private:
    Project();
    virtual ~Project();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};