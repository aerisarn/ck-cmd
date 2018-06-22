#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class DumpList : public CommandBase
{
    REGISTER_COMMAND_HEADER(DumpList)

private:

    DumpList();
    virtual ~DumpList();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};