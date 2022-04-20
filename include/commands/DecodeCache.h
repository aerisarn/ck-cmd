#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class DecodeCacheCmd : public Command<DecodeCacheCmd>
{
	REGISTER_COMMAND_HEADER(DecodeCacheCmd)

private:
	DecodeCacheCmd();
	virtual ~DecodeCacheCmd();

public:
	virtual string GetName() const;
	virtual string GetHelp() const;
	virtual string GetHelpShort() const;

protected:
	virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};