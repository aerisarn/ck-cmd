#include <commands/ExportRig.h>

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

static bool BeginConversion(const string& importSkeleton, const string& importSkeletonNif, const string& animationsPath, const string& exportPath);
static void InitializeHavok();
static void CloseHavok();


REGISTER_COMMAND_CPP(ExportRig)

ExportRig::ExportRig()
{
}

ExportRig::~ExportRig()
{
}

string ExportRig::GetName() const
{
	return "exportrig";
}

string ExportRig::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd importanimation
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_skeleton_hkx> <path_to_skeleton_nif> [-a <path_to_animations>] [-e <path_to_export>]\r\n";

	const char help[] =
		R"(Converts an HKX skeleton to FBX.
		
		Arguments:
			<path_to_skeleton_hkx> the animation skeleton in hkx format
			<path_to_skeleton_nif> the SAME animation skeleton in NIF format
			-a <path_to_animations>, --animations <path_to_animations>  optional animations to load on the rig
			-e <path_to_export>, --export-dir <path_to_export>  optional export path

		)";
	return usage + help;
}

string ExportRig::GetHelpShort() const
{
	return "TODO: Short help message for ImportFBX";
}

bool ExportRig::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	//We can improve this later, but for now this i'd say this is a good setup.
	string importSkeleton, importSkeletonNif, animationsPath, exportPath;

	importSkeleton = parsedArgs["<path_to_skeleton_hkx>"].asString();
	importSkeletonNif = parsedArgs["<path_to_skeleton_nif>"].asString();
	if (parsedArgs["-a"].asBool())
		animationsPath = parsedArgs["<path_to_animations>"].asString();
	if (parsedArgs["-e"].asBool())
		exportPath = parsedArgs["<path_to_export>"].asString();

	InitializeHavok();
	BeginConversion(importSkeleton, importSkeletonNif, animationsPath, exportPath);
	CloseHavok();
	return true;
}

bool BeginConversion(const string& importSkeleton, const string& importSkeletonNif, const string& animationsPath, const string& exportPath) {
	if (!fs::exists(importSkeleton) || !fs::is_regular_file(importSkeleton)) {
		Log::Error("Invalid file: %s", importSkeleton.c_str());
		return false;
	}
	if (!fs::exists(importSkeletonNif) || !fs::is_regular_file(importSkeletonNif)) {
		Log::Error("Invalid file: %s", importSkeleton.c_str());
		return false;
	}
	fs::path outputDir = fs::path(exportPath);
	if (!fs::exists(outputDir) || !fs::is_directory(outputDir)) {
		Log::Info("Invalid Directory: %s, using current_dir", exportPath.c_str());
		outputDir = fs::current_path();
	}

	FBXWrangler wrangler;
	wrangler.NewScene();



	vector<FbxProperty> floats;
	vector<FbxNode*> ordered_skeleton = wrangler.importExternalSkeleton(importSkeleton, floats);

	vector<fs::path> animation_files;
	if (fs::exists(animationsPath) && fs::is_directory(animationsPath))
	{
		find_files(animationsPath, ".hkx", animation_files);
		for (const auto& anim : animation_files)
			wrangler.importAnimationOnSkeleton(anim.string(), ordered_skeleton, floats);
	}

	NifFile mesh(importSkeletonNif.c_str());
	wrangler.AddNif(mesh);

	fs::path out_path = outputDir / fs::path(importSkeleton).filename().replace_extension(".fbx");
	fs::create_directories(outputDir);
	wrangler.ExportScene(out_path.string().c_str());

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