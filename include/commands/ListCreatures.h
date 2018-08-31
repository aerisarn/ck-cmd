#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class ListCreaturesCmd : public CommandBase
{
	REGISTER_COMMAND_HEADER(ListCreaturesCmd)

private:
	ListCreaturesCmd();
	virtual ~ListCreaturesCmd();

public:
	virtual string GetName() const;
	virtual string GetHelp() const;
	virtual string GetHelpShort() const;

protected:
	virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};