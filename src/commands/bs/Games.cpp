#include "stdafx.h"

#include <commands/Games.h>

#include <core/hkxcmd.h>
#include <core/log.h>

#include <core/games.h>
#include <core/bsa.h>

using namespace std;
using namespace ckcmd::info;

REGISTER_COMMAND_CPP(GamesCmd)

GamesCmd::GamesCmd()
{
}

GamesCmd::~GamesCmd()
{
}

string GamesCmd::GetName() const
{
    return "Games";
}

string GamesCmd::GetHelp() const
{
    string name = GetName();
    transform(name.begin(), name.end(), name.begin(), ::tolower);

    // Usage: ck-cmd games
    string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + "\r\n";

    const char help[] = "Prints the list of Bethesda games with their installation path";

    return usage + help;
}

string GamesCmd::GetHelpShort() const
{
    return "Checks for installed bethesda games and their install path";
}

bool GamesCmd::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	Games& games = Games::Instance();
	const Games::GamesPathMapT& pathMap = games.getGames();

	Games::GamesPathMapT::const_iterator installed = pathMap.begin();
	for (; installed != games.getGames().end(); installed++) {
		std::cout << Games::string(installed->first) << ": " << games.data(installed->first) << std::endl;
		std::cout << "BSAs:" << std::endl;
		for (const auto& bsa : games.bsas(installed->first)) {
			std::cout << "\t" << bsa.filename() << std::endl;
		}
		std::cout << "ESMs:" << std::endl;
		for (const auto& esm : games.esms(installed->first)) {
			std::cout << "\t" << esm.filename() << std::endl;
		}
		std::cout << "ESPs:" << std::endl;
		for (const auto& esp : games.esps(installed->first)) {
			std::cout << "\t" << esp.filename() << std::endl;
		}
	}

	return true;
}