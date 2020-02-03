#include <commands/ListCreatures.h>

#include "stdafx.h"

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/AnimationCache.h>

using namespace std;
using namespace ckcmd::info;
using namespace ckcmd::BSA;

ListCreaturesCmd::ListCreaturesCmd()
{
}

ListCreaturesCmd::~ListCreaturesCmd()
{
}

string ListCreaturesCmd::GetName() 
{
	return "ListCreatures";
}

string ListCreaturesCmd::GetHelp() 
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd games
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + "\r\n";

	const char help[] = "Prints the list of havok creature projects on a Skyrim Installation";

	return usage + help;
}

string ListCreaturesCmd::GetHelpShort() 
{
	return "Checks for installed bethesda games and their install path";
}

bool ListCreaturesCmd::InternalRunCommand(const CommandSettings& settings)
{
	Games& games = Games::Instance();
	Games::Game tes5 = Games::TES5;
	
	//if (!games.isGameInstalled(tes5)) {
	//    Log::Error("This command only works on TES5, and doesn't seem to be installed. Be sure to run the game at least once.");
	//    return false;
	//}
	
	string animDataContent;
	string animSetDataContent;
	
	const std::string animDataPath = "I:\\git_ref\\tes4gatekeeper\\meshes\\animationdatasinglefile.txt";
	const std::string animSetDataPath = "I:\\git_ref\\tes4gatekeeper\\meshes\\animationsetdatasinglefile.txt";
	
	loadFileIntoString(animDataPath, animDataContent );
	loadFileIntoString(animSetDataPath, animSetDataContent );

	//const vector<string> bsas = { "Update.bsa", "Skyrim - Animations.bsa" };
	//
	////by priority order, we first check for overrides
	//loadOverrideOrBSA(animDataPath, animDataContent, tes5, bsas);
	//loadOverrideOrBSA(animSetDataPath, animSetDataContent, tes5, bsas);
	
	AnimationCache cache(animDataContent, animSetDataContent);
	cache.printInfo();

	return true;
}