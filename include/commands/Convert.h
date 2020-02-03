#include "stdafx.h"

// Command Base
#include <commands\CommandBase.h>

class Convert : public CommandBase<Convert>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(Convert)

private:
    Convert();
    virtual ~Convert();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};