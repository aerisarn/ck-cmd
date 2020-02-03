#include "stdafx.h"

// Command Base
#include <commands\CommandBase.h>


class Calculate1stPerson: public CommandBase<Calculate1stPerson>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(Calculate1stPerson)

private:
    Calculate1stPerson();
    virtual ~Calculate1stPerson();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};