#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class RetargetCreatureCmd : public CommandBase
{
	REGISTER_COMMAND_HEADER(RetargetCreatureCmd)

private:
	RetargetCreatureCmd();
	virtual ~RetargetCreatureCmd();

public:
	virtual string GetName() const;
	virtual string GetHelp() const;
	virtual string GetHelpShort() const;

protected:
	virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};