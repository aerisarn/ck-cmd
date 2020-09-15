#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <core/HKXWrangler.h>
#include <commands/AddNodesToSkeleton.h>
#include <commands/Geometry.h>
#include <core/games.h>
#include <core/bsa.h>
#include <core/NifFile.h>
#include <commands/NifScan.h>

#include <Physics\Dynamics\Constraint\Bilateral\Ragdoll\hkpRagdollConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\BallAndSocket\hkpBallAndSocketConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Hinge\hkpHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\LimitedHinge\hkpLimitedHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Prismatic\hkpPrismaticConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\StiffSpring\hkpStiffSpringConstraintData.h>
#include <Physics\Dynamics\Constraint\Malleable\hkpMalleableConstraintData.h>

#include <Physics\Collide\Util\hkpTriangleUtil.h>

#include <limits>
#include <array>
#include <unordered_map>

static bool BeginConversion(string importPath, string exportPath);
static void InitializeHavok();
static void CloseHavok();


AddNodesToSkeleton::AddNodesToSkeleton()
{
}

AddNodesToSkeleton::~AddNodesToSkeleton()
{
}

string AddNodesToSkeleton::GetName() const
{
	return "addnodestoskeleton";
}

string AddNodesToSkeleton::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd convertnif [-i <path_to_import>] [-e <path_to_export>]
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " [-i <path_to_import>] [-e <path_to_export>]\r\n";

	//will need to check this help in console/
	const char help[] =
		R"(Adds dummy nodes to skeleton.nif.
		
		Arguments:
			<path_to_import> path to the skeleton NIF model which you want to modify with dummy nodes between nodes"
			<path_to_export> path to the output directory;

		)";

	return usage + help;
}

string AddNodesToSkeleton::GetHelpShort() const
{
	//I'm unsure about returning a string.. It doesn't show up on the console..
	//Log::Info("Convert Oblivion version models to Skyrim's format.");
	return "TODO: Short help message for ConvertNif";
}

bool AddNodesToSkeleton::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	//We can improve this later, but for now this i'd say this is a good setup.
	string importPath, exportPath;

	importPath = parsedArgs["<path_to_import>"].asString();
	exportPath = parsedArgs["<path_to_export>"].asString();

	InitializeHavok();
	BeginConversion(importPath, exportPath);
	CloseHavok();
	return true;
}

using namespace ckcmd::info;
using namespace ckcmd::BSA;
using namespace ckcmd::Geometry;
using namespace ckcmd::NIF;
using namespace ckcmd::HKX;
using namespace ckcmd::nifscan;


bool BeginConversion(string importPath, string exportPath) {

	fs::path skeletonModelpath = fs::path(importPath);
	if (!fs::exists(skeletonModelpath) || !fs::is_regular_file(skeletonModelpath)) {
		Log::Info("Invalid file: %s", importPath.c_str());
		return false;
	}
	fs::path outputDir = fs::path(exportPath);
	if (!fs::exists(outputDir) || !fs::is_directory(outputDir)) {
		Log::Info("Invalid file: %s", exportPath.c_str());
		return false;
	}

	NifInfo info;
	vector<NiObjectRef> blocks = ReadNifList(skeletonModelpath.string().c_str(), &info);
	NiObjectRef root = GetFirstRoot(blocks);

	for (NiObjectRef niobj : blocks) {
		if (niobj->IsSameType(NiNode::TYPE)) {
			NiNodeRef ninodeobj = DynamicCast<NiNode>(niobj);
			//if (ninodeobj->GetName().find("NPC") != string::npos) {
				vector<Ref<NiAVObject>> children = ninodeobj->GetChildren();
				vector<Ref<NiAVObject>> new_children;
				for (NiAVObjectRef rr : children) {
					NiNode* proxyRoot = new NiNode();

					proxyRoot->SetFlags(ninodeobj->GetFlags());
					proxyRoot->SetChildren({rr});
					proxyRoot->SetName(IndexString("ProxyNode_" + rr->GetName()));
					new_children.push_back(proxyRoot);
				}
				if (!new_children.empty())
					ninodeobj->SetChildren(new_children);
			//}
		}
	}

	fs::path out_path = outputDir / "skeleton.nif";
	fs::create_directories(outputDir);
	WriteNifTree(out_path.string(), root, info);
	Log::Info("Done");
	return true;
}

static void HelpString(hkxcmd::HelpType type) {
	switch (type)
	{
	case hkxcmd::htShort: Log::Info("About - Help about this program."); break;
	case hkxcmd::htLong: {
		char fullName[MAX_PATH], exeName[MAX_PATH];
		GetModuleFileName(NULL, fullName, MAX_PATH);
		_splitpath(fullName, NULL, NULL, exeName, NULL);
		Log::Info("Usage: %s about", exeName);
		Log::Info("  Prints additional information about this program.");
	}
						 break;
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