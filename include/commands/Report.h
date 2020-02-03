#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class Report : public CommandBase<Report>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(Report)

private:
    Report();
    virtual ~Report();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};