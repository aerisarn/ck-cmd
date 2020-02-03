#ifndef EXPANIM_H
#define EXPANIM_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ExportAnimation : public CommandBase<ExportAnimation>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

	REGISTER_COMMAND_HEADER(ExportAnimation)

private:
	ExportAnimation();
	virtual ~ExportAnimation();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

	virtual bool InternalRunCommand(const CommandSettings& settings);
};

#endif //EXPANIM_H