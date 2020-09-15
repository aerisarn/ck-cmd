#ifndef IMPANIM_H
#define IMPANIM_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ImportAnimation : public Command<ImportAnimation>
{
	REGISTER_COMMAND_HEADER(ImportAnimation)

private:
	ImportAnimation();
	virtual ~ImportAnimation();

public:
	virtual string GetName() const;
	virtual string GetHelp() const;
	virtual string GetHelpShort() const;

protected:
	virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};

#endif //IMPANIM_H