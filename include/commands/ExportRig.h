#ifndef EXPORTRIG_H
#define EXPORTRIG_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ExportRig : public CommandBase<ExportRig>
{
	COMMAND_PARAMETERS_LIST
	{
		//COMMAND_PARAMETER(bool, a);
	};

	REGISTER_COMMAND_HEADER(ExportRig)

private:
	ExportRig();
	virtual ~ExportRig();

public:
	static string GetName();
	static string GetHelp();
	static string GetHelpShort();

	virtual bool InternalRunCommand(const CommandSettings& settings);
};

#endif //EXPORTRIG_H