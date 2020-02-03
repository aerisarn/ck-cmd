#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class GamesCmd : public CommandBase<GamesCmd>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(GamesCmd)

private:
    GamesCmd();
    virtual ~GamesCmd();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};