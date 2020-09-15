#ifndef IMPFBX_H
#define IMPFBX_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ImportSkin : public Command<ImportSkin>
{
    REGISTER_COMMAND_HEADER(ImportSkin)

private:
	ImportSkin();
    virtual ~ImportSkin();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};

#endif //IMPFBX_H