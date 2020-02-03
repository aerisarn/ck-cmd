#include "stdafx.h"

// Command Base
#include <commands\CommandBase.h>



struct About : public CommandBase<About>
{

	COMMAND_PARAMETERS_LIST 
	{
		//COMMAND_PARAMETER(bool, a);
		bool a;
	};


	struct CommandSettingsHelp
	{

	};

    About();
    virtual ~About();



public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

    virtual bool InternalRunCommand(const CommandSettings& settings);

	REGISTER_COMMAND_HEADER(About);
};
