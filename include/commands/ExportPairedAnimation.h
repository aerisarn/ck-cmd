#ifndef EXPANIM_H
#define EXPANIM_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ExportPairedAnimation : public Command<ExportPairedAnimation>
{
	REGISTER_COMMAND_HEADER(ExportPairedAnimation)

private:
	ExportPairedAnimation();
	virtual ~ExportPairedAnimation();

public:
	virtual string GetName() const;
	virtual string GetHelp() const;
	virtual string GetHelpShort() const;

protected:
	virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};

#endif //EXPANIM_H