#ifndef EXPORTRIG_H
#define EXPORTRIG_H
#include "stdafx.h"

// Command Base
#include <commands/CommandBase.h>

class Skeleton : public Command<Skeleton>
{
	REGISTER_COMMAND_HEADER(Skeleton)

private:
	Skeleton();
	virtual ~Skeleton();

public:
	virtual string GetName() const;
	virtual string GetHelp() const;
	virtual string GetHelpShort() const;

	static bool Convert(const string& inpath, const string& animations_path, const string& outpath);

protected:
	virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
};

#endif //EXPORTRIG_H