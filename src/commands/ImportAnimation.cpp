#include <commands/ImportAnimation.h>
#include <core/MathHelper.h>

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

static bool BeginConversion(const string& importSkeleton, const string& importFBX, const string& cacheFilePath, const string& behaviorFolder, const string& exportPath);
static void InitializeHavok();
static void CloseHavok();


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
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_skeleton_hkx> <path_to_fbx_animation> [--b=<path_to_behavior_folder>] [--c=<path_to_cache_file>] [--e=<path_to_export>]\r\n";

	const char help[] =
		R"(Converts a FBX animation to NIF. Requires a preexisting HKX skeleton
		
		Arguments:
			<path_to_skeleton_hkx> the animation skeleton in hkx format
			<path_to_fbx_animation> the FBX animation to convert
			--c=<path_to_cache_file>, --cache <path_to_cache_file> necessary to extract root motion into animations
			--b=<path_to_behavior_folder>, --behavior <path_to_behavior_folder> necessary to extract root motion
			--e=<path_to_export> path to the output directory

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
	string importFBX, importSkeleton, exportPath, cacheFilePath,
		behaviorFolder;;

	importSkeleton = parsedArgs["<path_to_skeleton_hkx>"].asString();
	importFBX = parsedArgs["<path_to_fbx_animation>"].asString();
	//exportPath = parsedArgs["<path_to_export>"].asString();
	if (parsedArgs["--c"].isString())
		cacheFilePath = parsedArgs["--c"].asString();
	if (parsedArgs["--b"].isString())
		behaviorFolder = parsedArgs["--b"].asString();
	if (parsedArgs["--e"].isString())
		exportPath = parsedArgs["--e"].asString();
	InitializeHavok();
	BeginConversion(importSkeleton, importFBX, cacheFilePath, behaviorFolder, exportPath);
	CloseHavok();
	return true;
}

bool BeginConversion(const string& importSkeleton, const string& importFBX, const string& cacheFilePath, const string& behaviorFolder, const string& exportPath) {
	bool batch = false;
	fs::path fbxModelpath = fs::path(importFBX);
	if (!fs::exists(importSkeleton) || !fs::is_regular_file(importSkeleton)) {
		Log::Error("Invalid file: %s", importSkeleton.c_str());
		return false;
	}
	if (fs::exists(importFBX))
	{
		if (fs::is_regular_file(importFBX)) {
			batch = false;
		}
		else if (fs::is_directory(importFBX)) {
			batch = true;
		}
		else {
			Log::Error("Invalid path: %s", importFBX.c_str());
			return false;
		}
	}
	fs::path outputDir = fs::path(exportPath);
	fs::create_directories(outputDir);
	if (!fs::exists(outputDir) || !fs::is_directory(outputDir)) {
		Log::Info("Invalid Directory: %s, using current_dir", exportPath.c_str());
		outputDir = fs::current_path();
	}
	if (!batch)
	{
		FBXWrangler wrangler;
		wrangler.setExternalSkeletonPath(importSkeleton);
		wrangler.ImportScene(fbxModelpath.string().c_str());

		fs::path out_path = outputDir / fbxModelpath.filename().replace_extension(".hkx");
		fs::create_directories(outputDir);
		auto root_info = wrangler.SaveAnimation(out_path.string());
		if (fs::exists(cacheFilePath) && !fs::is_directory(cacheFilePath) &&
			fs::exists(behaviorFolder) && fs::is_directory(behaviorFolder))
		{
			fs::path animDataPath = fs::path(cacheFilePath).parent_path().parent_path() / "animationdatasinglefile.txt";
			fs::path animDataSetPath = fs::path(cacheFilePath).parent_path().parent_path() / "animationsetdatasinglefile.txt";
			Log::Info("Adjusting cache, loading %s and %s", animDataPath.string().c_str(), animDataSetPath.string().c_str());
			AnimationCache cache(animDataPath, animDataSetPath);
			Log::Info("Loaded");
			string project = fs::path(cacheFilePath).filename().replace_extension("").string();
			CacheEntry* entry = cache.find(project);
			if (entry)
			{
				Log::Info("Found Project %s", entry->name.c_str());
				//auto creature_entry = dynamic_cast<CreatureCacheEntry*>(entry);
				wrangler.hkx_wrapper().PutClipMovement(
					fbxModelpath.replace_extension(".hkx"),
					*entry,
					behaviorFolder,
					root_info.begin()->second
				);

				cache.save_creature(project, entry, "animationdatasinglefile.txt", "animationsetdatasinglefile.txt", ".");
			}
		}
	}
	else {
		vector<fs::path> fbxs;
		find_files(importFBX, ".fbx", fbxs);
		AnimationCache * cache = NULL;
		CacheEntry* entry = NULL;
		string project;
		if (fs::exists(cacheFilePath) && !fs::is_directory(cacheFilePath) &&
			fs::exists(behaviorFolder) && fs::is_directory(behaviorFolder))
		{
			fs::path animDataPath = fs::path(cacheFilePath).parent_path().parent_path() / "animationdatasinglefile.txt";
			fs::path animDataSetPath = fs::path(cacheFilePath).parent_path().parent_path() / "animationsetdatasinglefile.txt";
			Log::Info("Adjusting cache, loading %s and %s", animDataPath.string().c_str(), animDataSetPath.string().c_str());
			cache = new AnimationCache(animDataPath, animDataSetPath);
			Log::Info("Loaded");
			project = fs::path(cacheFilePath).filename().replace_extension("").string();
			entry = cache->find(project);
		}
		for (const auto& fbx : fbxs) {
			Log::Info("Importing: %s, using current_dir", fbx.string().c_str());
			FBXWrangler wrangler;
			wrangler.setExternalSkeletonPath(importSkeleton);
			wrangler.ImportScene(fbx.string().c_str());
			fs::path parent_path = fbx.parent_path();
			fs::path rel_path = "";
			while (parent_path != importFBX)
			{
				rel_path = parent_path.filename() / rel_path;
				parent_path = parent_path.parent_path();
			}
			fs::path out_path = outputDir / rel_path / fbx.filename().replace_extension(".hkx");
			fs::create_directories(out_path.parent_path());
			auto root_info = wrangler.SaveAnimation(out_path.string());
			if (entry) {
				wrangler.hkx_wrapper().PutClipMovement(
					fbxModelpath.replace_extension(".hkx"),
					*entry,
					behaviorFolder,
					root_info.begin()->second
				);
			}
		}
		if (entry)
		{
			cache->save_creature(project, entry, "animationdatasinglefile.txt", "animationsetdatasinglefile.txt");
			delete cache;
		}
	}

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