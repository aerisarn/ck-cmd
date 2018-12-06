#ifndef EXPORTRIG_H
#define EXPORTRIG_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ExportRig : public CommandBase
{
	REGISTER_COMMAND_HEADER(ExportRig)

private:
	ExportRig();
	virtual ~ExportRig();

public:
	virtual string GetName() const;
	virtual string GetHelp() const;
	virtual string GetHelpShort() const;

protected:
	virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};

#endif //EXPORTRIG_H