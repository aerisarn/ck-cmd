#include "stdafx.h"

#include <core/hkxcmd.h>
#include <core/log.h>

#include <core/games.h>
#include <core/bsa.h>

using namespace std;

static void HelpString(hkxcmd::HelpType type) {
	switch (type)
	{
	case hkxcmd::htShort: Log::Info("Games - Checks for installed bethesda games and their install path."); break;
	case hkxcmd::htLong:
	{
		char fullName[MAX_PATH], exeName[MAX_PATH];
		GetModuleFileName(NULL, fullName, MAX_PATH);
		_splitpath(fullName, NULL, NULL, exeName, NULL);
		Log::Info("Usage: %s games", exeName);
		Log::Info("  Prints the list of Bethesda games with their installation path.");
	}
	break;
	}
}

using namespace ckcmd::info;

static bool ExecuteCmd(hkxcmdLine &cmdLine)
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

REGISTER_COMMAND(Games, HelpString, ExecuteCmd);