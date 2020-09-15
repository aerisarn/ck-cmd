#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class Retarget : public Command<Retarget>
{
    REGISTER_COMMAND_HEADER(Retarget)

private:
    Retarget();
    virtual ~Retarget();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};