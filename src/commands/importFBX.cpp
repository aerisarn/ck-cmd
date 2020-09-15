#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <commands/importFBX.h>

#include <commands/Geometry.h>
#include <core/FBXWrangler.h>

#include <core/games.h>
#include <core/bsa.h>
#include <bs/AnimDataFile.h>
#include <bs/AnimSetDataFile.h>


using namespace ckcmd::FBX;
using namespace ckcmd::info;
using namespace ckcmd::BSA;

static bool BeginConversion(string importPath, string exportPath);
static void InitializeHavok();
static void CloseHavok();


ImportFBX::ImportFBX()
{
}

ImportFBX::~ImportFBX()
{
}

string ImportFBX::GetName() const
{
    return "importfbx";
}

string ImportFBX::GetHelp() const
{
    string name = GetName();
    transform(name.begin(), name.end(), name.begin(), ::tolower);

    // Usage: ck-cmd exportfbx
    string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_fbx> [-e <path_to_export>]\r\n";

	const char help[] =
		R"(Converts FBX format to NIF.
		
		Arguments:
			<path_to_fbx> the FBX to convert
			<path_to_export> path to the output directory

		)";
    return usage + help;
}

string ImportFBX::GetHelpShort() const
{
    return "TODO: Short help message for ImportFBX";
}

bool ImportFBX::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	//We can improve this later, but for now this i'd say this is a good setup.
	string importFBX, exportPath;

	importFBX = parsedArgs["<path_to_fbx>"].asString();
	exportPath = parsedArgs["<path_to_export>"].asString();

	InitializeHavok();
	BeginConversion(importFBX, exportPath);
	CloseHavok();
	return true;
}

bool BeginConversion(string importFBX, string exportPath) {
	fs::path fbxModelpath = fs::path(importFBX);
	if (!fs::exists(fbxModelpath) || !fs::is_regular_file(fbxModelpath)) {
		Log::Info("Invalid file: %s", fbxModelpath.c_str());
		return false;
	}
	fs::path outputDir = fs::path(exportPath);
	if (!fs::exists(outputDir) || !fs::is_directory(outputDir)) {
		Log::Info("Invalid Directory: %s, using current_dir", exportPath.c_str());
		outputDir = fs::current_path();
	}

	FBXWrangler wrangler;
	if (wrangler.ImportScene(fbxModelpath.string().c_str()))
	{

		fs::path out_path = outputDir / fbxModelpath.filename().replace_extension(".nif");
		fs::create_directories(outputDir);
		wrangler.SaveNif(out_path.string());
	}
	else {
		Log::Error("Invalid FBX File: %s", fbxModelpath.string().c_str());
	}

}

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