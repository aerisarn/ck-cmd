#include "stdafx.h"

// Command Base
#include <commands\CommandBase.h>


class Calculate1stPerson: public Command<Calculate1stPerson>
{
    REGISTER_COMMAND_HEADER(Calculate1stPerson)

private:
    Calculate1stPerson();
    virtual ~Calculate1stPerson();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};