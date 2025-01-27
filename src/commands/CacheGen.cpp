#include <commands/CacheGen.h>
#include <core/MathHelper.h>

#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <core/games.h>
#include <core/bsa.h>
#include <bs/AnimDataFile.h>
#include <bs/AnimSetDataFile.h>
#include <core/AnimationCache.h>


using namespace ckcmd::info;
using namespace ckcmd::BSA;

static bool BeginConversion(const string& cachePath, const string& exportPath);

string CacheGen::GetName() const
{
	return "cachegen";
}

string CacheGen::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd importanimation
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_projects> <output_path>\r\n";

	const char help[] =
		R"(Generate animation caches from individual projects
		
		Arguments:
			<path_to_projects> path containing animationdata and animationsetdata folders
			<output_path> path to the output directory

		)";
	return usage + help;
}

string CacheGen::GetHelpShort() const
{
	return "Generate animation cache files";
}

bool CacheGen::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string cacheLocation = parsedArgs["<path_to_projects>"].asString();
	string exportPath = parsedArgs["<output_path>"].asString();
	
	BeginConversion(cacheLocation, exportPath);
	return true;
}

bool BeginConversion(const string& cachePath, const string& exportPath) {

	fs::path animDataDir = fs::path(cachePath) / "animationdata";
	fs::path boundAnims = animDataDir / "boundanims";
	fs::path animSetDir = fs::path(cachePath) / "animationsetdata";
	fs::path outPath{ exportPath };

	if (!fs::exists(animDataDir) || !fs::exists(animSetDir) || !fs::exists(boundAnims))
	{
		Log::Error("Cannot locate project directories: %s", cachePath.c_str());
		return false;
	}

	if (!fs::exists(outPath) || !fs::is_directory(outPath)) {
		Log::Error("Invalid export path locations: %s", exportPath.c_str());
		return false;
	}

	Log::Info("Reading Projects from %s and %s", animDataDir.string().c_str(), animSetDir.string().c_str());
	
	AnimData::AnimDataFile animationData{};
	AnimData::AnimSetDataFile animationSetData;

	for (auto& dirEntry : fs::directory_iterator(animDataDir))
	{
		if (fs::is_directory(dirEntry.path()))
			continue;

		std::string entry_extension = dirEntry.path().extension().string();
		transform(entry_extension.begin(), entry_extension.end(), entry_extension.begin(), ::tolower);
		if (entry_extension == ".txt") {
			StaticCacheEntry cacheEntry{};
			AnimationCache::get_entries(cacheEntry, dirEntry.path().string());

			string pName = dirEntry.path().filename().string();
			if (cacheEntry.hasCache()) {
				animationData.putProject(pName, cacheEntry.block, cacheEntry.movements);
			}
			else {
				animationData.putProject(pName, cacheEntry.block);
			}
		}
	}

	// read dirlist instead
	fs::path dirList = animSetDir / "dirList.txt";
	if (!fs::exists(dirList) || !fs::is_regular_file(dirList))
	{
		Log::Error("Can't find animsetdata directory listing: %s", dirList.c_str());
		return false;
	}

	std::ifstream ifs{ dirList };
	for (std::string line; std::getline(ifs, line); )
	{
		fs::path projectFile = animSetDir / line;
		if (!fs::exists(projectFile))
			continue;

		AnimData::ProjectAttackListBlock attackBlock{};
		AnimationCache::get_attack_entries(attackBlock, projectFile);

		if (attackBlock.getProjectAttackBlocks().size() > 0)
		{
			animationSetData.putProjectAttackBlock(line, attackBlock);
		}
	}

	std::ofstream outstream;
	outstream.open(outPath / "animationdatasinglefile.txt"); // append instead of overwrite
	outstream << animationData.toString();
	outstream.close();
	outstream.open(outPath / "animationsetdatasinglefile.txt");
	outstream << animationSetData.toString();
	outstream.close();

	return true;
}
