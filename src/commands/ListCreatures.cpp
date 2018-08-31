#include <commands/ListCreatures.h>

#include "stdafx.h"

#include <core/hkxcmd.h>
#include <core/AnimationCache.h>

#include <core/games.h>
#include <core/bsa.h>



using namespace std;
using namespace ckcmd::info;
using namespace ckcmd::BSA;

REGISTER_COMMAND_CPP(ListCreaturesCmd)

ListCreaturesCmd::ListCreaturesCmd()
{
}

ListCreaturesCmd::~ListCreaturesCmd()
{
}

string ListCreaturesCmd::GetName() const
{
	return "ListCreatures";
}

string ListCreaturesCmd::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd games
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + "\r\n";

	const char help[] = "Prints the list of havok creature projects on a Skyrim Installation";

	return usage + help;
}

string ListCreaturesCmd::GetHelpShort() const
{
	return "Checks for installed bethesda games and their install path";
}



void loadFileIntoString(const fs::path& path, string& content) {
	std::ifstream fss(path.c_str());
	content.clear();
	//allocate
	fss.seekg(0, std::ios::end);
	content.reserve(fss.tellg());
	//reset and assign
	fss.seekg(0, std::ios::beg);
	content.assign((std::istreambuf_iterator<char>(fss)),
		std::istreambuf_iterator<char>());
}

void loadOverrideOrBSA(const string& path, string& content, const Games::Game& game, const vector<string>& preferredBsas) {
	//search in override
	Games& games = Games::Instance();
	fs::path override_path = games.data(game) / path;
	if (fs::exists(override_path) && fs::is_regular_file(override_path))
		loadFileIntoString(override_path, content);
	else {
		for (string bsa_name : preferredBsas) {
			BSAFile bsa_file(games.data(game) / bsa_name);
			if (bsa_file.find(path)) {
				size_t size = -1;
				const uint8_t* data = bsa_file.extract(path, size);
				content.assign((char*)data, size);
				break;
			}
		}
	}
}


bool ListCreaturesCmd::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	Games& games = Games::Instance();
	Games::Game tes5 = Games::TES5;
	
	if (!games.isGameInstalled(tes5)) {
	    Log::Error("This command only works on TES5, and doesn't seem to be installed. Be sure to run the game at least once.");
	    return false;
	}
	
	string animDataContent;
	string animSetDataContent;
	
	const std::string animDataPath = "meshes\\animationdatasinglefile.txt";
	const std::string animSetDataPath = "meshes\\animationsetdatasinglefile.txt";
	
	const vector<string> bsas = { "Update.bsa", "Skyrim - Animations.bsa" };
	
	//by priority order, we first check for overrides
	loadOverrideOrBSA(animDataPath, animDataContent, tes5, bsas);
	loadOverrideOrBSA(animSetDataPath, animSetDataContent, tes5, bsas);
	
	AnimationCache cache(animDataContent, animSetDataContent);
	cache.printInfo();

	return true;
}