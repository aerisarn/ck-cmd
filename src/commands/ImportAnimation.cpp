#include <commands/ImportAnimation.h>

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


using namespace ckcmd::FBX;
using namespace ckcmd::info;
using namespace ckcmd::BSA;

static bool BeginConversion(const string& importSkeleton, const string& importFBX, const string& exportPath);
static void InitializeHavok();
static void CloseHavok();


REGISTER_COMMAND_CPP(ImportAnimation)

ImportAnimation::ImportAnimation()
{
}

ImportAnimation::~ImportAnimation()
{
}

string ImportAnimation::GetName() const
{
	return "importanimation";
}

string ImportAnimation::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd importanimation
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_skeleton_hkx> <path_to_fbx_animation> [-e <path_to_export>]\r\n";

	const char help[] =
		R"(Converts a FBX animation to NIF. Requires a preexisting HKX skeleton
		
		Arguments:
			<path_to_skeleton_hkx> the animation skeleton in hkx format
			<path_to_fbx_animation> the FBX animation to convert
			<path_to_export> path to the output directory

		)";
	return usage + help;
}

string ImportAnimation::GetHelpShort() const
{
	return "TODO: Short help message for ImportFBX";
}

bool ImportAnimation::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	//We can improve this later, but for now this i'd say this is a good setup.
	string importFBX, importSkeleton, exportPath;

	importSkeleton = parsedArgs["<path_to_skeleton_hkx>"].asString();
	importFBX = parsedArgs["<path_to_fbx_animation>"].asString();
	exportPath = parsedArgs["<path_to_export>"].asString();

	InitializeHavok();
	BeginConversion(importSkeleton, importFBX, exportPath);
	CloseHavok();
	return true;
}

bool BeginConversion(const string& importSkeleton, const string& importFBX, const string& exportPath) {
	fs::path fbxModelpath = fs::path(importFBX);
	if (!fs::exists(importSkeleton) || !fs::is_regular_file(importSkeleton)) {
		Log::Error("Invalid file: %s", fbxModelpath.c_str());
		return false;
	}
	if (!fs::exists(fbxModelpath) || !fs::is_regular_file(fbxModelpath)) {
		Log::Error("Invalid file: %s", fbxModelpath.c_str());
		return false;
	}
	fs::path outputDir = fs::path(exportPath);
	if (!fs::exists(outputDir) || !fs::is_directory(outputDir)) {
		Log::Info("Invalid Directory: %s, using current_dir", exportPath.c_str());
		outputDir = fs::current_path();
	}

	FBXWrangler wrangler;
	wrangler.setExternalSkeletonPath(importSkeleton);
	wrangler.ImportScene(fbxModelpath.string().c_str());

	fs::path out_path = outputDir / fbxModelpath.filename().replace_extension(".hkx");
	fs::create_directories(outputDir);
	wrangler.SaveAnimation(out_path.string());
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