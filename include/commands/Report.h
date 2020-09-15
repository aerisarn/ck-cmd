#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class Report : public Command<Report>
{
    REGISTER_COMMAND_HEADER(Report)

private:
    Report();
    virtual ~Report();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};