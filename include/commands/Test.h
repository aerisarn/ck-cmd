#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class Test : public CommandBase<Test>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(Test)

private:
    Test();
    virtual ~Test();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};