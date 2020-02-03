#ifndef IMPFBX_H
#define IMPFBX_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ImportFBX : public CommandBase<ImportFBX>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(ImportFBX)

private:
	ImportFBX();
    virtual ~ImportFBX();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};

#endif //IMPFBX_H