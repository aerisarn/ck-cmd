#ifndef EXPFBX_H
#define EXPFBX_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ImportFBX : public CommandBase
{
    REGISTER_COMMAND_HEADER(ImportFBX)

private:
	ImportFBX();
    virtual ~ImportFBX();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};

#endif //EXPFBX_H