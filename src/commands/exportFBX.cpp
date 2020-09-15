#include "stdafx.h"
#include <core/hkfutils.h>

#include <commands/ExportFBX.h>

#include <commands/Geometry.h>
#include <core/FBXWrangler.h>

#include <core/hkxcmd.h>
#include <core/log.h>
#include <core/games.h>
#include <core/bsa.h>
#include <bs/AnimDataFile.h>
#include <bs/AnimSetDataFile.h>

static bool BeginConversion(string importPath, string exportPath, string texturePath);
static void InitializeHavok();
static void CloseHavok();


using namespace ckcmd::FBX;
using namespace ckcmd::info;
using namespace ckcmd::BSA;


ExportFBX::ExportFBX()
{
}

ExportFBX::~ExportFBX()
{
}

string ExportFBX::GetName() const
{
    return "exportfbx";
}

string ExportFBX::GetHelp() const
{
    string name = GetName();
    transform(name.begin(), name.end(), name.begin(), ::tolower);

	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_nif> [-e <path_to_export>] [-t <path_to_textures>]\r\n";

	const char help[] =
		R"(Converts NIF format to FBX.
		
		Arguments:
			<path_to_nif> the NIF to convert
			<path_to_export> path to the output directory
			<path_to_textures> path to the folder with extracted texture (usually Skyrim Data subfolder)

		)";
    return usage + help;
}

string ExportFBX::GetHelpShort() const
{
    return "TODO: Short help message for ExportFBX";
}

bool ExportFBX::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	//We can improve this later, but for now this i'd say this is a good setup.
	string importNIF, exportPath = "", texturePath = "";

	if (parsedArgs.find("<path_to_nif>") == parsedArgs.end())
	{
		Log::Error("<path_to_nif> argument is mandatory");
		return false;
	}
	importNIF = parsedArgs["<path_to_nif>"].asString();
	if (parsedArgs.find("-e") != parsedArgs.end() && parsedArgs["-e"].asBool())
		exportPath = parsedArgs["<path_to_export>"].asString();
	if (parsedArgs.find("-t") != parsedArgs.end() && parsedArgs["-t"].asBool())
		texturePath = parsedArgs["<path_to_textures>"].asString();

	InitializeHavok();
	BeginConversion(importNIF, exportPath, texturePath);
	CloseHavok();
	return true;
}

bool BeginConversion(string importNIF, string exportPath, string texturePath) {
	fs::path nifModelpath = fs::path(importNIF);
	if (!fs::exists(nifModelpath) || !fs::is_regular_file(nifModelpath)) {
		Log::Info("Invalid file: %s", nifModelpath.c_str());
		return false;
	}
	fs::path outputDir = fs::path(exportPath);
	if (!fs::exists(outputDir) || !fs::is_directory(outputDir)) {
		Log::Info("Invalid Directory: %s, using current_dir", exportPath.c_str());
		outputDir = fs::current_path();
	}

	fs::path out_path = outputDir / nifModelpath.filename().replace_extension(".fbx");
	fs::create_directories(outputDir);

	FBXWrangler wrangler;
	wrangler.texture_path = texturePath;
	NifFile mesh(nifModelpath.string().c_str());
	wrangler.NewScene();
	wrangler.AddNif(mesh);
	wrangler.ExportScene(out_path.string().c_str());

	return true;
}

//bool ExportFBX::InternalRunCommand(map<string, docopt::value> parsedArgs)
//{
//    Log::Info("Begin Scan");
//
//    Games& games = Games::Instance();
//    const Games::GamesPathMapT& installations = games.getGames();
//#if 0
//
//    map<NifFile*, fs::path> skeletons;
//    map<NifFile*, set<NifFile*>> pieces;
//
//    Games::Game toDo = Games::TES5;
//
//    for (const auto& bsa : games.bsas(toDo)) {
//        std::cout << "Scan: " << bsa.filename() << std::endl;
//        BSAFile bsa_file(bsa);
//
//
//
//        for (const auto& nif : bsa_file.assets(".*\.nif")) {
//            Log::Info("Current File: %s", nif.c_str());
//
//            size_t size = -1;
//            const uint8_t* data = bsa_file.extract(nif, size);
//
//            std::string sdata((char*)data, size);
//            std::istringstream iss(sdata);
//
//            bool save = false;
//
//            NifFile* niffile = new NifFile(iss);
//            if (niffile->isSkeletonOnly()) {
//                save = true;
//                //skeletons.insert(niffile);
//                //out_path = fs::path(".") / fs::path("nif") / "skeletons" / fs::path(nif);
//                //fs::create_directories(out_path.parent_path());
//                //niffile->Save(out_path.string().c_str());
//                if (toDo == Games::TES4)
//                    niffile->name = fs::path(nif).parent_path().filename().replace_extension("").string();
//                else if (toDo == Games::TES5) //Character Assets
//                    niffile->name = fs::path(nif).parent_path().parent_path().filename().replace_extension("").string();
//                skeletons[niffile] = fs::path(fs::path(nif));
//            }
//
//            if (!save) delete niffile;
//            delete data;
//
//        }
//    }
//    for (const auto& bsa : games.bsas(Games::TES5)) {
//        std::cout << "Scan: " << bsa.filename() << std::endl;
//        BSAFile bsa_file(bsa);
//        for (const auto& nif : bsa_file.assets(".*\.nif")) {
//            Log::Info("Current File: %s", nif.c_str());
//
//            size_t size = -1;
//            const uint8_t* data = bsa_file.extract(nif, size);
//
//            std::string sdata((char*)data, size);
//            std::istringstream iss(sdata);
//
//            bool save = false;
//
//            NifFile* niffile = new NifFile(iss);
//            if (niffile->hasExternalSkin()) {
//
//                //meshes.insert(niffile);
//                niffile->name = fs::path(nif).filename().replace_extension("").string();
//                for (const auto & skeleton : skeletons)
//                    if (nif.find(skeleton.second.parent_path().string()) != string::npos) {
//                        save = true;
//                        pieces[skeleton.first].insert(niffile);
//                    }
//                //out_path = fs::path(".") / fs::path("nif") / "meshes" / fs::path(nif);
//                //fs::create_directories(out_path.parent_path());
//                //niffile->Save(out_path.string().c_str());
//            }
//
//            if (!save) delete niffile;
//            delete data;
//
//        }
//    }
//#else
//    FBXWrangler* wrangler = new FBXWrangler();
//    //NifFile* skeleton = new NifFile("D:\\git\\resources\\in\\Data\\Meshes\\actors\\TES4Goblin\\CharacterAssets\\skeleton.nif");
//    NifFile* mesh = new NifFile("D:\\git\\resources\\in\\boatdooranim01_obl.nif");
//    //wrangler->AddExternalSkinnedMeshes(*skeleton, {mesh});
//    //wrangler->AddNif(*skeleton);
//    //wrangler->ExportScene("D:\\git\\resources\\in\\Data\\Meshes\\actors\\TES4Goblin\\CharacterAssets\\skeleton.fbx");
//    //wrangler->CloseScene();
//    wrangler->NewScene();
//    wrangler->AddNif(*mesh);
//    wrangler->ExportScene("D:\\git\\resources\\in\\boatdooranim01_obl.fbx");
//
//    //Games::Game tes5 = Games::TES5;
//
//    //if (!games.isGameInstalled(tes5)) {
//    //    Log::Error("This command only works on TES5, and doesn't seem to be installed. Be sure to run the game at least once.");
//    //    return false;
//    //}
//
//    //string animDataContent;
//    //string animSetDataContent;
//
//    //const std::string animDataPath = "meshes\\animationdatasinglefile.txt";
//    //const std::string animSetDataPath = "meshes\\animationsetdatasinglefile.txt";
//
//    //const vector<string> bsas = { "Update.bsa", "Skyrim - Animations.bsa" };
//
//    //by priority order, we first check for overrides
//    //loadOverrideOrBSA(animDataPath, animDataContent, tes5, bsas);
//    //loadOverrideOrBSA(animSetDataPath, animSetDataContent, tes5, bsas);
//
//    //AnimationCache cache(animDataContent, animSetDataContent);
//    //cache.printInfo();
//
//#endif
//
//    //FBXWrangler* wrangler = new FBXWrangler();
//
//    //for (const auto & piece : pieces) {
//    //	for (const auto & mesh : piece.second) {
//    //		wrangler->NewScene();
//    //		wrangler->AddExternalSkinnedMeshes(*piece.first, { mesh });
//    //		fs::path out_path = fs::path(".") / fs::path("fbx") / fs::path(piece.first->name + "_" + mesh->name + ".fbx");
//    //		fs::create_directories(out_path.parent_path());
//    //		wrangler->ExportScene(out_path.string());
//    //		wrangler->CloseScene();
//    //	}
//    //}
//
//    //for (auto & piece : pieces) {
//    //	for (auto & mesh : piece.second) {
//    //		delete mesh;
//    //	}
//    //	if (piece.first) delete piece.first;
//    //}
//
//    //	delete wrangler;
//    return true;
//}

//Havok initialization

static void HK_CALL errorReport(const char* msg, void*)
{
	Log::Error("%s", msg);
}

static void HK_CALL debugReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}


static hkThreadMemory* threadMemory = NULL;
static char* stackBuffer = NULL;
static void InitializeHavok()
{
	// Initialize the base system including our memory system
	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
}

static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}