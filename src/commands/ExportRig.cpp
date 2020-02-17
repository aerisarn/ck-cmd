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

static bool BeginConversion(const string& importSkeleton, const string& importSkeletonNif, const string& animationsPath, const string& additionalNifPath, const string& cacheFilePath, const string& behaviorFolder, const string& exportPath);
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
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + 
		" <path_to_skeleton_hkx> <path_to_skeleton_nif> [--a=<path_to_animations>] [--b=<path_to_behavior_folder>] [--c=<path_to_cache_file>] [--n=<path_to_additional_nifs>] [--e=<path_to_export>]\r\n";

	const char help[] =
		R"(Converts an HKX skeleton to FBX.
		
		Arguments:
			<path_to_skeleton_hkx> the animation skeleton in hkx format
			<path_to_skeleton_nif> the SAME animation skeleton in NIF format
			-a <path_to_animations>, --animations <path_to_animations>  optional animations to load on the rig
			-n <path_to_additional_nifs>, --nifs <path_to_additional_nifs>  optional meshes to load on the rig
			-c <path_to_cache_file>, --cache <path_to_cache_file> necessary to extract root motion into animations
			-b <path_to_behavior_folder>, --behavior <path_to_behavior_folder> necessary to extract root motion
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
	string importSkeleton, 
		importSkeletonNif, 
		animationsPath, 
		exportPath, 
		additionalNifPath,
		cacheFilePath,
		behaviorFolder;

	importSkeleton = parsedArgs["<path_to_skeleton_hkx>"].asString();
	importSkeletonNif = parsedArgs["<path_to_skeleton_nif>"].asString();
	if (parsedArgs["--a"].isString())
		animationsPath = parsedArgs["--a"].asString();
	if (parsedArgs["--n"].isString())
		additionalNifPath = parsedArgs["--n"].asString();
	if (parsedArgs["--c"].isString())
		cacheFilePath = parsedArgs["--c"].asString();
	if (parsedArgs["--b"].isString())
		behaviorFolder = parsedArgs["--b"].asString();
	if (parsedArgs["--e"].isString())
		exportPath = parsedArgs["--e"].asString();

	InitializeHavok();
	BeginConversion(
		importSkeleton, 
		importSkeletonNif, 
		animationsPath, 
		additionalNifPath,
		cacheFilePath,
		behaviorFolder,
		exportPath);
	CloseHavok();
	return true;
}

bool BeginConversion(const string& importSkeleton, 
	const string& importSkeletonNif, 
	const string& animationsPath, 
	const string& additionalNifPath, 
	const string& cacheFilePath, 
	const string& behaviorFolder,
	const string& exportPath
) {
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


	wrangler.setExportRig(true);
	vector<FbxProperty> floats;
	vector<FbxNode*> ordered_skeleton = wrangler.importExternalSkeleton(importSkeleton, floats);

	vector<fs::path> animation_files;
	if (fs::exists(animationsPath) && fs::is_directory(animationsPath))
	{
		CacheEntry entry;
		CreatureCacheEntry creature_entry;
		std::map< fs::path, RootMovement> map;

		find_files(animationsPath, ".hkx", animation_files);
		if (fs::exists(cacheFilePath) && !fs::is_directory(cacheFilePath) &&
			fs::exists(behaviorFolder) && fs::is_directory(behaviorFolder))
		{
			AnimationCache::get_entries(entry, creature_entry, cacheFilePath);
			Log::Info("Loaded animation cache info from %s", cacheFilePath.c_str());
			HKXWrapper wrap;
			wrap.GetClipsMovements(
				animation_files,
				entry,
				creature_entry,
				behaviorFolder,
				map
			);
		
		}

		for (const auto& anim : animation_files)
		{
			FBXWrangler anim_wrangler;
			anim_wrangler.NewScene();
			auto root_movement = map.find(anim);
			fs::path out_path = anim;  out_path.replace_extension(".fbx");
			vector<FbxProperty> floats;
			vector<FbxNode*> ordered_skeleton = anim_wrangler.importExternalSkeleton(importSkeleton, floats);
			
			if (root_movement != map.end())
			{
				anim_wrangler.importAnimationOnSkeleton(
					anim.string(),
					ordered_skeleton,
					floats,
					root_movement->second);
			}
			else {
				anim_wrangler.importAnimationOnSkeleton(
					anim.string(),
					ordered_skeleton,
					floats,
					RootMovement());
			}

			vector<fs::path> nif_files;
			if (fs::exists(additionalNifPath) && fs::is_directory(additionalNifPath))
			{
				find_files(additionalNifPath, ".nif", nif_files);
				for (const auto& nif : nif_files)
					anim_wrangler.AddNif(NifFile(nif.string().c_str()));
			}

			anim_wrangler.ExportScene(out_path.string().c_str());
		}
	}

	NifFile mesh(importSkeletonNif.c_str());
	wrangler.AddNif(mesh);

	wrangler.setExportRig(false);
	vector<fs::path> nif_files;
	if (fs::exists(additionalNifPath) && fs::is_directory(additionalNifPath))
	{
		find_files(additionalNifPath, ".nif", nif_files);
		for (const auto& nif : nif_files)
			wrangler.AddNif(NifFile(nif.string().c_str()));
	}

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