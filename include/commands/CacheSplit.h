#ifndef EXPANIM_H
#define EXPANIM_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class CacheSplit : public Command<CacheSplit>
{
	REGISTER_COMMAND_HEADER(CacheSplit)

private:
	CacheSplit() = default;
	virtual ~CacheSplit() = default;

public:
	virtual string GetName() const;
	virtual string GetHelp() const;
	virtual string GetHelpShort() const;

protected:
	virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};

#endif //EXPANIM_H