#ifndef IMPFBX_H
#define IMPFBX_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ImportFBX : public Command<ImportFBX>
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

#endif //IMPFBX_H