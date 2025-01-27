#include <commands/CacheSplit.h>
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

string CacheSplit::GetName() const
{
	return "cachesplit";
}

string CacheSplit::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd importanimation
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_cache> <output_path>\r\n";

	const char help[] =
		R"(Splits animation cache files into individual projects
		
		Arguments:
			<path_to_cache> path containing animationdatasinglefile and animationsetdatasinglefile
			<output_path> path to the output directory

		)";
	return usage + help;
}

string CacheSplit::GetHelpShort() const
{
	return "Splits animation cache files";
}

bool CacheSplit::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string cacheLocation = parsedArgs["<path_to_cache>"].asString();
	string exportPath = parsedArgs["<output_path>"].asString();
	
	BeginConversion(cacheLocation, exportPath);
	return true;
}

bool BeginConversion(const string& cachePath, const string& exportPath) {

	fs::path animDataPath = fs::path(cachePath) / "animationdatasinglefile.txt";
	fs::path animDataSetPath = fs::path(cachePath) / "animationsetdatasinglefile.txt";
	if (!fs::exists(animDataPath) || !fs::exists(animDataSetPath))
	{
		Log::Error("Cannot locate cache files: %s", cachePath.c_str());
		return false;
	}

	if (!fs::exists(exportPath) || !fs::is_directory(exportPath)) {
		Log::Error("Invalid export path locations: %s", exportPath.c_str());
		return false;
	}

	Log::Info("Loading %s and %s", animDataPath.string().c_str(), animDataSetPath.string().c_str());
	AnimationCache cache(animDataPath, animDataSetPath);
	Log::Info("Loaded");

	fs::path animDataDir = fs::path(exportPath) / "animationdata";
	fs::path boundAnims = animDataDir / "boundanims";
	fs::path animSetDir = fs::path(exportPath) / "animationsetdata";
	
	fs::create_directories(boundAnims);
	fs::create_directory(animSetDir);
	
	size_t i{ 0 };
	vector<string> dirList{};
	for (const auto& project : cache.animationData.getProjectList().getStrings())
	{
		auto& projectBlock = cache.animationData.getProjectBlock(i);
		fs::path pFile = animDataDir / project;
		fs::path mFile = boundAnims / ("anims_" + project);
		ofstream ostream;

		ostream.open(pFile);
		ostream << projectBlock.getBlock();
		ostream.close();

		if (projectBlock.getHasAnimationCache()) {
			auto& pmBlock = cache.animationData.getprojectMovementBlock(i);
			ostream.open(mFile);
			ostream << pmBlock.getBlock();
			ostream.close();
		}

		string setDataProj = fs::path(project).filename().replace_extension("").string();
		string setDataFolder = setDataProj + "data";

		fs::path setDataDir{ animSetDir };
		setDataDir /= setDataFolder;

		fs::path relPath = fs::path(setDataFolder) / project;
		int index = cache.animationSetData.getProjectAttackBlock(relPath.string());
		if (index != -1)
		{
			fs::create_directory(setDataDir);

			// Iterate over all project attack files (there can be multiple) and write them to separate files
			auto& attackBlock = cache.animationSetData.getProjectAttackBlock(index);
			ofstream mainFile{};
			mainFile.open(setDataDir / project);
			size_t j = 0;
			for (const auto& projectComp : attackBlock.getProjectFiles().getStrings())
			{
				ostream.open(setDataDir / projectComp);
				ostream << attackBlock.getProjectAttackBlocks()[j++].getBlock();
				ostream.close();

				mainFile << projectComp << endl;
			}
			mainFile.close();
			dirList.push_back(relPath.string());
		}

		++i;
	}

	if (!dirList.empty())
	{
		ofstream ostream{ animSetDir / "dirlist.txt" };
		for (const auto& entry : dirList)
		{
			ostream << entry << endl;
		}
		ostream.close();
	}

	return true;
}
