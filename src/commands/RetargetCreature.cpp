#include <commands/RetargetCreature.h>

#include "stdafx.h"

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/AnimationCache.h>

using namespace ckcmd::info;
using namespace ckcmd::BSA;

REGISTER_COMMAND_CPP(RetargetCreatureCmd)

RetargetCreatureCmd::RetargetCreatureCmd()
{
}

RetargetCreatureCmd::~RetargetCreatureCmd()
{
}

string RetargetCreatureCmd::GetName() const
{
	return "RetargetCreature";
}

string RetargetCreatureCmd::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd games
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <source_havok_project> <output_havok_project_name>\r\n";

	//will need to check this help in console/
	const char help[] =
	R"(Creates a new havok project (to be used in a separate CK racial type) starting from an existing one.
	  The new project will be directly created into your Skyrim installation folder and the necessary 
	  files, like the animation cache merged files, will be amended too.
		
	  Arguments:
	  <source_havok_project> a valid havok project already loaded. 
							 Use ListCreatures command to find one"
	  <output_havok_project_name> the name of the havok project to be created. 
                                  Must not be laready present into the Animation Cache;

	  arguments are mandatory)";

	return usage + help;
}

string RetargetCreatureCmd::GetHelpShort() const
{
	return "Checks for installed bethesda games and their install path";
}


bool RetargetCreatureCmd::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string source_havok_project = parsedArgs["<source_havok_project>"].asString();
	string output_havok_project_name = parsedArgs["<output_havok_project_name>"].asString();

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
	
	fs::path project_path;
	bool in_override = false;

	if (cache.hasCreatureProject(source_havok_project)) {
		Log::Info("Retargeting %s into %s", source_havok_project.c_str(), output_havok_project_name.c_str());
		//search into override
		string to_find = fs::path(source_havok_project).replace_extension(".hkx").string();
		transform(to_find.begin(), to_find.end(), to_find.begin(), ::tolower);
		for (auto& p : fs::recursive_directory_iterator(games.data(tes5))) {
			string filename = p.path().filename().string();
			transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
			if (filename == to_find) {
				project_path = p.path();
				in_override = true;
				break;
			}
		}
		if (!in_override) {
			for (const auto& bsa_path : games.bsas(tes5)) {
				BSAFile b(bsa_path);
				std::vector<std::string> results = b.assets(".*" + to_find);
				if (results.size()>0)
					Log::Info("here");
			}
		}

	}

	return true;
}