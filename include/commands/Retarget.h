#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class Retarget : public CommandBase<Retarget>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

    REGISTER_COMMAND_HEADER(Retarget)

private:
    Retarget();
    virtual ~Retarget();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);
};