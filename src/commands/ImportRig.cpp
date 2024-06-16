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
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_skeleton_fbx> <path_to_skeleton_hkx> <path_to_skeleton_le_hkx> <path_to_skeleton_nif>\r\n";

	const char help[] =
		R"(Converts an FBX skeleton to NIF and HKX.
		
		Arguments:
			<path_to_skeleton_fbx> the animation skeleton in fbx format
			<path_to_skeleton_hkx> the output animation skeleton in hkx format
			<path_to_skeleton_le_hkx> the output animation skeleton in legacy hkx format
			<path_to_skeleton_nif> the output animation skeleton in nif format

		)";
	return usage + help;
}

string ImportRig::GetHelpShort() const
{
	return "TODO: Short help message for ImportFBX";
}

bool ImportRig::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string importSkeleton, exportSkeleton, exportLegacySkeleton, exportSkeletonNif;

	importSkeleton = parsedArgs["<path_to_skeleton_fbx>"].asString();
	exportSkeleton = parsedArgs["<path_to_skeleton_hkx>"].asString();
	exportLegacySkeleton = parsedArgs["<path_to_skeleton_le_hkx>"].asString();
	exportSkeletonNif = parsedArgs["<path_to_skeleton_nif>"].asString();

	InitializeHavok();
	FBXWrangler wrangler;
	wrangler.setExportRig();
	if (wrangler.ImportScene(importSkeleton.c_str(), exportSkeleton.c_str(), exportLegacySkeleton.c_str()))
	{
		wrangler.SaveNif(exportSkeletonNif);
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