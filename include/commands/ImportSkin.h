#ifndef IMPFBX_H
#define IMPFBX_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ImportSkin : public CommandBase<ImportSkin>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(ImportSkin)

private:
	ImportSkin();
    virtual ~ImportSkin();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};

#endif //IMPFBX_H