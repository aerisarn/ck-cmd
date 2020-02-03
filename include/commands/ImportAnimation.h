#ifndef IMPANIM_H
#define IMPANIM_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ImportAnimation : public CommandBase<ImportAnimation>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

	REGISTER_COMMAND_HEADER(ImportAnimation)

private:
	ImportAnimation();
	virtual ~ImportAnimation();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

	virtual bool InternalRunCommand(const CommandSettings& settings);
};

#endif //IMPANIM_H