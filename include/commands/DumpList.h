#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class DumpList : public CommandBase<DumpList>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(DumpList)

private:

    DumpList();
    virtual ~DumpList();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};