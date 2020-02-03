#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class RetargetCreatureCmd : public CommandBase<RetargetCreatureCmd>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

	REGISTER_COMMAND_HEADER(RetargetCreatureCmd)

private:
	RetargetCreatureCmd();
	virtual ~RetargetCreatureCmd();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

	virtual bool InternalRunCommand(const CommandSettings& settings);
};