#include "stdafx.h"

// Command Base
#include <commands\CommandBase.h>

class About : public CommandBase
{
    REGISTER_COMMAND_HEADER(About)

private:
    About();
    virtual ~About();

    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};