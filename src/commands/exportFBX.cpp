#include "stdafx.h"

#include <commands/Geometry.h>
#include <core/FBXWrangler.h>

#include <core/hkxcmd.h>
#include <core/log.h>
#include <core/games.h>
#include <core/bsa.h>

using namespace ckcmd::FBX;
using namespace ckcmd::info;
using namespace ckcmd::BSA;

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
	Log::Info("Begin Scan");

	Games& games = Games::Instance();
	const Games::GamesPathMapT& installations = games.getGames();
#if 1
	for (const auto& bsa : games.bsas(Games::TES5)) {
		std::cout << "Scan: " << bsa.filename() << std::endl;
		BSAFile bsa_file(bsa);
		for (const auto& nif : bsa_file.assets(".*\.nif")) {
			Log::Info("Current File: %s", nif.c_str());

			size_t size = -1;
			const uint8_t* data = bsa_file.extract(nif, size);

			std::string sdata((char*)data, size);
			std::istringstream iss(sdata);

			FBXWrangler* wrangler = new FBXWrangler();
			NifFile* niffile = new NifFile(iss);
			wrangler->AddNif(*niffile);
			fs::path out_path = fs::path(".") / "fbx" / fs::path(nif).replace_extension("fbx");
			fs::create_directories(out_path.parent_path());
			wrangler->ExportScene(out_path.string().c_str());

			delete data;
			delete niffile;
			delete wrangler;
		}
	}
#else
	FBXWrangler* wrangler = new FBXWrangler();
	NifFile* nif = new NifFile("I:\\git_ref\\resources\\nifs\\in\\alduin.nif");
	wrangler->AddNif(*nif);
	wrangler->ExportScene("I:\\git_ref\\resources\\nifs\\in\\alduin.fbx");
#endif

	return true;
}

REGISTER_COMMAND(exportFBX, HelpString, ExecuteCmd);