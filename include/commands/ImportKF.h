#pragma once
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ImportKF : public Command<ImportKF>
{
    REGISTER_COMMAND_HEADER(ImportKF)

private:
    ImportKF();
    virtual ~ImportKF();

public:
    virtual string GetName() const;
    virtual string GetHelp() const;
    virtual string GetHelpShort() const;

protected:
    virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};