#ifndef IMPANIM_H
#define IMPANIM_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ImportPairedAnimation : public Command<ImportPairedAnimation>
{
	REGISTER_COMMAND_HEADER(ImportPairedAnimation)

private:
	ImportPairedAnimation();
	virtual ~ImportPairedAnimation();

public:
	virtual string GetName() const;
	virtual string GetHelp() const;
	virtual string GetHelpShort() const;

protected:
	virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};

#endif //IMPANIM_H