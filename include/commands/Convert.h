#include "stdafx.h"

// Command Base
#include <commands\CommandBase.h>

class Convert : public Command<Convert>
{
    REGISTER_COMMAND_HEADER(Convert)

private:
    Convert();
    virtual ~Convert();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};