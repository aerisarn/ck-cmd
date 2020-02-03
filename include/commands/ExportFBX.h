#ifndef EXPFBX_H
#define EXPFBX_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ExportFBX : public CommandBase<ExportFBX>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(ExportFBX)

private:
    ExportFBX();
    virtual ~ExportFBX();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};

#endif //EXPFBX_H