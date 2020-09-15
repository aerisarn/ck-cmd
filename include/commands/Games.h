#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class GamesCmd : public Command<GamesCmd>
{
    REGISTER_COMMAND_HEADER(GamesCmd)

private:
    GamesCmd();
    virtual ~GamesCmd();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};