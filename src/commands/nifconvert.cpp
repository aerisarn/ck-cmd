#include "stdafx.h"
#include "niflib.h"

#include <gtest\gtest.h>
#include <core/hkxcmd.h>
#include <core/log.h>
#include <string>
#include <fstream>
#include <filesystem>

using namespace Niflib;
using namespace std;
using namespace std::experimental::filesystem;

static const path nif_in = "resources//in//";
static const path nif_out = "resources//out//";

void findFiles(path startingDir, string extension, vector<path>& results) {
	if (!exists(startingDir) || !is_directory(startingDir)) return;
	for (auto& dirEntry : std::experimental::filesystem::recursive_directory_iterator(startingDir))
	{
		if (is_directory(dirEntry.path()))
			continue;

		std::string entry_extension = dirEntry.path().extension().string();
		transform(entry_extension.begin(), entry_extension.end(), entry_extension.begin(), ::tolower);
		if (entry_extension == extension) {
			results.push_back(dirEntry.path().string());
		}
	}
}

bool BeginConversion()
{
	NifInfo info;
	vector<path> nifs;

	findFiles("", ".nif", nifs);

	if (nifs.empty()) Log::Info("No NIFs found.."); return false;

	for (size_t i = 0; i < nifs.size(); i++)
	{
		NiObjectRef root = ReadNifTree(nifs[i].string().c_str(), &info);
		vector<NiObjectRef> blocks = ReadNifList(nifs[i].string().c_str(), &info);

		//info.userVersion = 12;
		//info.userVersion2 = 83;
		//info.version = Niflib::VER_20_2_0_7;

		//path out_path = nif_out / nifs[i].filename();

		//WriteNifTree(out_path.string().c_str(), root, info);

	}
	Log::Info("Found : %i", nifs.size());
	return true;
}

static void HelpString(hkxcmd::HelpType type) {
	switch (type)
	{
	case hkxcmd::htShort: Log::Info("About - Help about this program."); break;
	case hkxcmd::htLong:
	{
		char fullName[MAX_PATH], exeName[MAX_PATH];
		GetModuleFileName(NULL, fullName, MAX_PATH);
		_splitpath(fullName, NULL, NULL, exeName, NULL);
		Log::Info("Usage: %s about", exeName);
		Log::Info("  Prints additional information about this program.");
	}
	break;
	}
}


static bool ExecuteCmd(hkxcmdLine &cmdLine)
{
	BeginConversion();
	return true;
}

REGISTER_COMMAND(NifConvert, HelpString, ExecuteCmd);