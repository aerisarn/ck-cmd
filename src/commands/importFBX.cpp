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


struct AnimationCache {
    AnimData::AnimDataFile animationData;
    AnimData::AnimSetDataFile animationSetData;

    AnimationCache(const string&  animationDataContent, const string&  animationSetDataContent) {
        animationData.parse(animationDataContent);
        animationSetData.parse(animationSetDataContent);
    }

    void printInfo() {
        Log::Info("Parsed correctly %d havok projects", animationData.getProjectList().getStrings().size());
        Log::Info("Found %d creatures:", animationSetData.getProjectsList().getStrings().size());
        set<string> creatures;
        int index = 0;
        for (string creature : animationSetData.getProjectsList().getStrings()) {
            string creature_project_name = fs::path(creature).filename().replace_extension("").string();
            if (creature_project_name.find("Project") != string::npos)
                creature_project_name = creature_project_name.substr(0, creature_project_name.find("Project"));
            Log::Info("\t%d: %s", index++, creature_project_name.c_str());
            creatures.insert(creature_project_name);
        }
        index = 0;
        int misc_index = 0;
        Log::Info("Found %d misc projects:", animationData.getProjectList().getStrings().size() - animationSetData.getProjectsList().getStrings().size());
        for (string misc : animationData.getProjectList().getStrings()) {
            string misc_project_name = fs::path(misc).filename().replace_extension("").string();
            if (!animationData.getProjectBlock(index).getHasAnimationCache()) {
                Log::Info("\t%d: %s", misc_index++, misc_project_name.c_str());
            }
            index++;
            //if (misc_project_name.find("Project") != string::npos)
            //	misc_project_name = misc_project_name.substr(0, misc_project_name.find("Project"));
            //bool is_misc = true;
            //for (string creature : creatures)
            //	if (misc_project_name.find(creature) != string::npos)
            //		is_misc = false;
            //if(is_misc)

        }
    }
};


REGISTER_COMMAND_CPP(ImportFBX)

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