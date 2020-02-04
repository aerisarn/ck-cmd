#include <commands/ImportRig.h>

#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <commands/Geometry.h>
#include <core/FBXWrangler.h>

#include <core/games.h>
#include <core/bsa.h>
#include <bs/AnimDataFile.h>
#include <bs/AnimSetDataFile.h>

#include <core/MathHelper.h>


using namespace ckcmd::FBX;
using namespace ckcmd::info;
using namespace ckcmd::BSA;

static void InitializeHavok();
static void CloseHavok();


REGISTER_COMMAND_CPP(ImportRig)

ImportRig::ImportRig()
{
}

ImportRig::~ImportRig()
{
}

string ImportRig::GetName() const
{
	return "importrig";
}

string ImportRig::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd importanimation
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_skeleton_fbx> [-a <path_to_animations>] [-e <path_to_export>]\r\n";

	const char help[] =
		R"(Converts an FBX skeleton to NIF and HKX.
		
		Arguments:
			<path_to_skeleton_fbx> the animation skeleton in hkx format
			-a <path_to_animations>, --animations <path_to_animations> animations output path
			-e <path_to_export>, --export-dir <path_to_export>  optional export path

		)";
	return usage + help;
}

string ImportRig::GetHelpShort() const
{
	return "TODO: Short help message for ImportFBX";
}

bool ImportRig::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	//We can improve this later, but for now this i'd say this is a good setup.
	string importSkeleton, importSkeletonNif, animationsPath, exportPath;

	importSkeleton = parsedArgs["<path_to_skeleton_fbx>"].asString();
	if (parsedArgs["-a"].asBool())
		animationsPath = parsedArgs["<path_to_animations>"].asString();
	if (parsedArgs["-e"].asBool())
		exportPath = parsedArgs["<path_to_export>"].asString();
	fs::path outputDir = fs::path(exportPath);

	if (!fs::exists(exportPath) || !fs::is_directory(outputDir)) {
		Log::Info("Invalid Directory: %s, using current_dir", exportPath.c_str());
		outputDir = fs::current_path();
	}


	InitializeHavok();
	FBXWrangler wrangler;
	if (wrangler.ImportScene(importSkeleton.c_str()))
	{
		fs::path out_path = outputDir / fs::path(importSkeleton).filename().replace_extension(".nif");
		fs::create_directories(outputDir);
		wrangler.SaveNif(out_path.string());
	}
	CloseHavok();
	return true;
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