#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ListCreaturesCmd : public CommandBase<ListCreaturesCmd>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

	REGISTER_COMMAND_HEADER(ListCreaturesCmd)

private:
	ListCreaturesCmd();
	virtual ~ListCreaturesCmd();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

	virtual bool InternalRunCommand(const CommandSettings& settings);
};