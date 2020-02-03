#include <commands/RetargetCreature.h>

#include "stdafx.h"

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/AnimationCache.h>
#include <core/HKXWrangler.h>

#include <hkbBehaviorReferenceGenerator_0.h>
#include <hkbClipGenerator_2.h>
#include <hkbProjectData_2.h>
#include <hkbCharacterData_7.h>
#include <hkbBehaviorGraph_1.h>

using namespace ckcmd::info;
using namespace ckcmd::BSA;
using namespace ckcmd::HKX;

static void InitializeHavok();
static void CloseHavok();

RetargetCreatureCmd::RetargetCreatureCmd()
{
}

RetargetCreatureCmd::~RetargetCreatureCmd()
{
}

string RetargetCreatureCmd::GetName() 
{
	return "RetargetCreature";
}

string RetargetCreatureCmd::GetHelp() 
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd games
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <source_havok_project> <output_havok_project_name>\r\n";

	//will need to check this help in console/
	const char help[] =
	R"(Creates a new havok project (to be used in a separate CK racial type) starting from an existing one.
	  The new project will be directly created into your Skyrim installation folder and the necessary 
	  files, like the animation cache merged files, will be amended too.
		
	  Arguments:
	  <source_havok_project> a valid havok project already loaded. 
							 Use ListCreatures command to find one"
	  <output_havok_project_name> the name of the havok project to be created. 
                                  Must not be already present into the Animation Cache;

	  arguments are mandatory)";

	return usage + help;
}

string RetargetCreatureCmd::GetHelpShort() 
{
	return "Checks for installed bethesda games and their install path";
}

struct compare {
	bool operator()(const std::string& first, const std::string& second) {
		return first.size() < second.size();
	}
};

template<typename hkType>
hkRefPtr<hkType> load_havok_file(const fs::path& path, HKXWrapper& wrapper)
{
	Games& games = Games::Instance();
	Games::Game tes5 = Games::TES5;

	if (fs::exists(games.data(tes5) / path)) {
		fs::path file_path = games.data(tes5) / path;
		return wrapper.load<hkType>(file_path);
	}
	for (const auto& bsa_path : games.bsas(tes5)) {
		BSAFile bsa_file(bsa_path);
		bool found = bsa_file.find(path.string());
		if (found)
		{
			//get the shortest path
			size_t size = -1;
			const uint8_t* data = bsa_file.extract(path.string(), size);
			hkRefPtr<hkType> hkroot = wrapper.load<hkType>(data, size);
			delete data;
			return hkroot;
		}
	}
	throw runtime_error("File not found:" + path.string());
	return HK_NULL;
}

template<typename hkType>
hkRefPtr<hkType> load_havok_file(const fs::path& path, hkArray<hkVariant>& objects, HKXWrapper& wrapper)
{
	Games& games = Games::Instance();
	Games::Game tes5 = Games::TES5;

	if (fs::exists(games.data(tes5) / path)) {
		fs::path file_path = games.data(tes5) / path;
		return wrapper.load<hkType>(file_path, objects);
	}
	for (const auto& bsa_path : games.bsas(tes5)) {
		BSAFile bsa_file(bsa_path);
		bool found = bsa_file.find(path.string());
		if (found)
		{
			//get the shortest path
			size_t size = -1;
			const uint8_t* data = bsa_file.extract(path.string(), size);
			hkRefPtr<hkType> hkroot = wrapper.load<hkType>(data, size, objects);
			delete data;
			return hkroot;
		}
	}
	throw runtime_error("File not found:" + path.string());
	return HK_NULL;
}

HKXWrapper wrapper;

struct clip_compare {
	bool operator() (const hkRefPtr<hkbClipGenerator>& lhs, const hkRefPtr<hkbClipGenerator>& rhs) const {
		return string(lhs->m_name) < string(rhs->m_name);
	}
};


void extract_clips(BSAFile& bsa_file, const string& behavior_path, const fs::path& root,  set<hkRefPtr<hkbClipGenerator>, clip_compare>& clips)
{
	string behavior_full_path = fs::canonical(root / behavior_path).string();
	size_t offset = behavior_full_path.find("meshes", 0);
	size_t end = behavior_full_path.length();
	if (offset < end)
		behavior_full_path = behavior_full_path.substr(offset, end);
	if (bsa_file.find(behavior_full_path))
	{
		hkRootLevelContainer* broot = NULL;
		string bdata = bsa_file.extract(behavior_full_path);
		hkArray<hkVariant> objects;
		hkRefPtr<hkbBehaviorGraph> bhkroot = wrapper.load<hkbBehaviorGraph>((const uint8_t *)bdata.c_str(), bdata.size(), broot, objects);
		Log::Info("Graph: %s", bhkroot->m_name);
		for (const auto& object : objects)
		{
			if (hkbClipGenerator::staticClass().getSignature() == object.m_class->getSignature())
			{
				hkRefPtr<hkbClipGenerator> clip = (hkbClipGenerator*)object.m_object;
				Log::Info("Clip: %s, animation: %s", clip->m_name, clip->m_animationName);
				clips.insert(clip);
				clip = broot->findObject<hkbClipGenerator>(clip);
			}
			if (hkbBehaviorReferenceGenerator::staticClass().getSignature() == object.m_class->getSignature())
			{
				hkRefPtr<hkbBehaviorReferenceGenerator> sub_graph = (hkbBehaviorReferenceGenerator*)object.m_object;
				Log::Info("Sub Graph: %s", sub_graph->m_behaviorName);
				extract_clips(
					bsa_file,
					string(sub_graph->m_behaviorName),
					root,
					clips
				);
			}
		}
	}
}

bool RetargetCreatureCmd::InternalRunCommand(const CommandSettings& settings)
{
	string source_havok_project = "", output_havok_project_name = "";
	//if (parsedArgs["<source_havok_project>"].isString())
	//	source_havok_project = parsedArgs["<source_havok_project>"].asString();
	//if (parsedArgs["<output_havok_project_name>"].isString())
	//	output_havok_project_name = parsedArgs["<output_havok_project_name>"].asString();

	//Games& games = Games::Instance();
	//Games::Game tes5 = Games::TES5;

	//if (!games.isGameInstalled(tes5)) {
	//	Log::Error("This command only works on TES5, and doesn't seem to be installed. Be sure to run the game at least once.");
	//	return false;
	//}
	InitializeHavok();

	BSAFile bsa_file("I:\\git_ref\\resources\\bsa\\Skyrim - Animations.bsa");
	const std::regex re_actors("meshes\\\\actors\\\\(?!.*(animations|characters|character assets|characterassets|sharedkillmoves|behaviors)).*hkx", std::regex_constants::icase);
	const std::regex re_misc("(?!.*(\\\\actors|\\\\.*animations|\\\\.*characters|\\\\.*character assets|\\\\.*characterassets|\\\\.*sharedkillmoves|\\\\behaviors|\\\\.+behaviors\\\\.*behavior|handeffect|handfx|fx.*cloak)).*hkx", std::regex_constants::icase);


	vector<string> projects;
	vector<string> misc;
	set<fs::path> filenames;

	//find all projects
	vector<string> assets = bsa_file.assets(".*\.hkx");
	for (const auto& hkx_file : assets)
	{
		string data = bsa_file.extract(hkx_file);
		hkRootLevelContainer* root = NULL;
		hkRefPtr<hkbProjectData> hkroot = wrapper.load<hkbProjectData>((const uint8_t *)data.c_str(), data.size(), root);
		if (hkroot != NULL)
		{
			if (fs::path(hkx_file).parent_path().string().find("actors") != string::npos)
				projects.push_back(hkx_file);
			else
			{
				string filename = fs::path(hkx_file).filename().string();
				//if (!filenames.insert(filename).second)
				//	continue;
				if (filename.find("fxfirecloak01.hkx") != string::npos ||
					filename.find("fxicecloak") != string::npos ||
					filename.find("healcontargetfx01") != string::npos ||
					filename.find("healfxhand01.hkx") != string::npos ||
					filename.find("healmystcontargetfx01.hkx") != string::npos ||
					filename.find("healmystfxhand01.hkx") != string::npos ||
					filename.find("healmysttargetfx.hkx") != string::npos ||
					filename.find("invisfxbody01.hkx") != string::npos ||
					filename.find("invisfxhand01.hkx") != string::npos ||
					filename.find("lightspellactorfx.hkx") != string::npos ||
					filename.find("lightspellhazard.hkx") != string::npos ||
					filename.find("shockhandeffects.hkx") != string::npos ||
					filename.find("soultrapcastpointfx01.hkx") != string::npos ||
					filename.find("soultraphandeffects.hkx") != string::npos ||
					filename.find("soultraptargeteffects.hkx") != string::npos ||
					filename.find("soultraptargetpointfx.hkx") != string::npos ||
					filename.find("summontargetfx.hkx") != string::npos ||
					filename.find("turnundeadfxhand01.hkx") != string::npos ||
					filename.find("turnundeadtargetfx.hkx") != string::npos ||
					hkx_file.find("traps\\leantotrap\\norretractablebridge01.hkx") != string::npos ||
					filename.find("fxlightspellhandeffects") != string::npos
				)
					continue;
				hkRefPtr<hkbProjectStringData> sdata = hkroot->m_stringData;
				if (sdata->m_characterFilenames.getSize() > 0)
				{
					string char_rel_path = sdata->m_characterFilenames[0];
					hkRootLevelContainer* croot = NULL;
					string char_path = fs::canonical((fs::path(hkx_file).parent_path() / char_rel_path)).string();
					size_t offset = char_path.find("meshes", 0);
					size_t end = char_path.length();
					if (offset < end)
						char_path = char_path.substr(offset, end);
					if (bsa_file.find(char_path))
					{
						string cdata = bsa_file.extract(char_path);
						hkRefPtr<hkbCharacterData> chkroot = wrapper.load<hkbCharacterData>((const uint8_t *)cdata.c_str(), cdata.size(), croot);
						misc.push_back(hkx_file);
					}
					else {
						Log::Warn("Project file with character file not found!: %s", hkx_file.c_str());
					}
				}
				else {
					Log::Warn("Project file without character: %s", hkx_file.c_str());
				}
			}
		}

	}
	std::sort(projects.begin(), projects.end(),
		[](const string& lhs, const string& rhs) -> bool
	{
		fs::path pllhs = lhs;
		fs::path plrhs = rhs;

		if (pllhs.parent_path() != plrhs.parent_path())
			return plrhs.parent_path() > pllhs.parent_path();

		return plrhs.filename() > pllhs.filename();
	});

	std::sort(misc.begin(), misc.end(),
		[](const string& lhs, const string& rhs) -> bool
	{
		fs::path pllhs = lhs; 
		fs::path plrhs = rhs;

		if (pllhs.parent_path() != plrhs.parent_path())
			return plrhs.parent_path() > pllhs.parent_path();

		return plrhs.filename() > pllhs.filename();
	});


	//AnimationCache::rebuild_from_bsa(bsa_file, projects, misc);

	string animDataContent;
	string animSetDataContent;

	const std::string animDataPath = "meshes\\animationdatasinglefile.txt";
	const std::string animSetDataPath = "meshes\\animationsetdatasinglefile.txt";

	string anim_data_content = bsa_file.extract(animDataPath);
	string anim_set_data_content = bsa_file.extract(animSetDataPath);

	AnimationCache cache(anim_data_content, anim_set_data_content);

	int cache_index = 0;

	for (const auto& creature_project_file : projects)
	{
		string project_file_data = bsa_file.extract(creature_project_file);

		hkRootLevelContainer* root = NULL;
		hkRefPtr<hkbProjectData>  project_file_hkroot =
			wrapper.load<hkbProjectData>((const uint8_t *)project_file_data.c_str(), project_file_data.size(), root);

		Log::Info("Analyzing project: %s", creature_project_file.c_str());

		//Characters
		if (project_file_hkroot->m_stringData != NULL)
		{
			for (const auto& character : project_file_hkroot->m_stringData->m_characterFilenames)
			{
				Log::Info("Found Character: %s", character);
				string char_path = fs::canonical((fs::path(creature_project_file).parent_path() / string(character))).string();
				size_t offset = char_path.find("meshes", 0);
				size_t end = char_path.length();
				if (offset < end)
					char_path = char_path.substr(offset, end);
				Log::Info("Canonical Character Path: %s", char_path.c_str());

				string character_file_data = bsa_file.extract(char_path);
				hkRootLevelContainer* character_root = NULL;
				hkRefPtr<hkbCharacterData>  character_file_hkroot =
					wrapper.load<hkbCharacterData>((const uint8_t *)character_file_data.c_str(), character_file_data.size(), root);
				set<hkRefPtr<hkbClipGenerator>, clip_compare> generators;
				if (character_file_hkroot->m_stringData != NULL)
				{
					Log::Info("Animations: %d", character_file_hkroot->m_stringData->m_animationNames.getSize());
					Log::Info("Behavior: %s", character_file_hkroot->m_stringData->m_behaviorFilename);
					Log::Info("Rig: %s", character_file_hkroot->m_stringData->m_rigName);
					Log::Info("Ragdoll: %s", character_file_hkroot->m_stringData->m_ragdollName);

					
					extract_clips(
						bsa_file,
						string(character_file_hkroot->m_stringData->m_behaviorFilename),
						fs::path(creature_project_file).parent_path(),
						generators
					);
				}
				set<string> referenced_animations;
				for (const auto& generator : generators)
				{
					referenced_animations.insert(string(generator->m_animationName));
				}

				CreatureCacheEntry& entry = cache.creature_entries[cache_index];
				if (character_file_hkroot->m_stringData->m_animationNames.getSize() !=
					referenced_animations.size())
				{
					Log::Warn("Declared animations %d, referenced by clips %d",
						character_file_hkroot->m_stringData->m_animationNames.getSize(),
						referenced_animations.size());
					for (const auto& decl_anim : character_file_hkroot->m_stringData->m_animationNames)
					{
						if (referenced_animations.find(string(decl_anim)) == referenced_animations.end())
						{
							Log::Warn("Declared but not in clip %s", decl_anim);
						}
					}
				}
				if (character_file_hkroot->m_stringData->m_animationNames.getSize() !=
					entry.movements.getMovementData().size())
					Log::Warn("Declared animations %d, cache movements %d",
						character_file_hkroot->m_stringData->m_animationNames.getSize(),
						entry.movements.getMovementData().size());
				if (generators.size() !=
					entry.block.getClips().size())
					Log::Warn("Behavior clips %d, cache blocks %d",
						generators.size(),
						entry.block.getClips().size());

			}
		}
		cache_index++;

	}
	
	fs::path project_path;

	//TODO: take load order into account!

	//find the project and load it
	//if (cache.hasCreatureProject(source_havok_project)) {
	//	Log::Info("Retargeting %s into %s", source_havok_project.c_str(), output_havok_project_name.c_str());
	//	//search into override
	//	string to_find = fs::path(source_havok_project).replace_extension(".hkx").string();
	//	transform(to_find.begin(), to_find.end(), to_find.begin(), ::tolower);
	//	for (auto& p : fs::recursive_directory_iterator(games.data(tes5))) {
	//		string filename = p.path().filename().string();
	//		transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
	//		if (filename == to_find) {
	//			project_path = p.path();
	//			project = wrapper.load<hkbProjectData>(p.path());
	//			in_override = true;
	//			break;
	//		}
	//	}
	//	if (!in_override) {
	//		for (const auto& bsa_path : games.bsas(tes5)) {
	//			BSAFile bsa_file(bsa_path);
	//			std::vector<std::string> results = bsa_file.assets(".*" + to_find);
	//			if (results.size() > 0)
	//			{
	//				//get the shortest path
	//				std::sort(results.begin(), results.end(), compare());
	//				project_path = results[0];
	//				Log::Info("%s", results[0].c_str());
	//				size_t size = -1;
	//				const uint8_t* data = bsa_file.extract(results[0], size);
	//				project = wrapper.load<hkbProjectData>(data, size);
	//				delete data;
	//				break;
	//			}					
	//		}
	//	}

	//}

	//fs::path out = games.data(tes5) / project_path.parent_path() / output_havok_project_name;
	//
	////skyrim files just usually have the character file here
	//vector<fs::path> character_paths;
	//hkArray<hkStringPtr>& characters_files = project->m_stringData->m_characterFilenames;
	//for (const auto& char_path_string : characters_files) {
	//	character_paths.push_back(project_path.parent_path() / fs::path(char_path_string.cString()));
	//}

	//vector<hkRefPtr<hkbCharacterData>> characters;
	////If not in override, we must extract all resources from bsa
	//for (const auto& rel_path : character_paths) {
	//	characters.push_back(load_havok_file<hkbCharacterData>(rel_path, wrapper));
	//}

	//vector<fs::path> rigs_paths;
	//vector<hkRefPtr<hkaAnimationContainer>> rigs;
	//vector<fs::path> behaviors_paths;
	//vector<hkRefPtr<hkbBehaviorGraph>> behaviors;
	//map<hkRefPtr<hkbBehaviorGraph>, hkArray<hkVariant>> behaviors_objects;
	//
	//for (const auto& char_ptr : characters) {
	//	hkRefPtr<hkbCharacterStringData> data = char_ptr->m_stringData;
	//	rigs_paths.push_back(project_path.parent_path() / fs::path(data->m_rigName.cString()));
	//	rigs.push_back(load_havok_file<hkaAnimationContainer>(project_path.parent_path() / fs::path(data->m_rigName.cString()), wrapper));
	//	behaviors_paths.push_back(project_path.parent_path() / fs::path(data->m_behaviorFilename.cString()));
	//	hkArray<hkVariant> behavior_objs;
	//	hkRefPtr<hkbBehaviorGraph> behavior_root = load_havok_file<hkbBehaviorGraph>(project_path.parent_path() / fs::path(data->m_behaviorFilename.cString()), behavior_objs, wrapper);
	//	behaviors_objects[behavior_root] = behavior_objs;
	//	behaviors.push_back(behavior_root);
	//}
	//size_t size = behaviors.size();
	//for (size_t i = 0; i < size; ++i)
	//{
	//	//look into behaviors for referenced files
	//	hkArray<hkVariant>& objects = behaviors_objects[behaviors[i]];
	//	for (const auto& obj : objects) {
	//		if (strcmp(obj.m_class->getName(), "hkbBehaviorReferenceGenerator") == 0) {
	//			hkRefPtr<hkbBehaviorReferenceGenerator> brg = (hkbBehaviorReferenceGenerator*)obj.m_object;
	//			fs::path sub_behavior_path = project_path.parent_path() / brg->m_behaviorName.cString();
	//			if (find(behaviors_paths.begin(), behaviors_paths.end(), sub_behavior_path) == behaviors_paths.end()) {
	//				hkArray<hkVariant> behavior_objs;
	//				behaviors_paths.push_back(sub_behavior_path);
	//				hkRefPtr<hkbBehaviorGraph> behavior_root = load_havok_file<hkbBehaviorGraph>(sub_behavior_path, behavior_objs, wrapper);
	//				behaviors_objects[behavior_root] = behavior_objs;
	//				behaviors.push_back(behavior_root);
	//				size++;
	//			}
	//		}
	//	}

	//}

	//vector<fs::path> animations_paths;
	//vector<hkRefPtr<hkaAnimationContainer>> animations;

	//for (const auto& behavior_ptr : behaviors) {
	//	hkArray<hkVariant>& objects = behaviors_objects[behavior_ptr];
	//	for (const auto& obj : objects) {
	//		if (strcmp(obj.m_class->getName(), "hkbClipGenerator") == 0) 
	//		{
	//			hkRefPtr<hkbClipGenerator> clip = (hkbClipGenerator*)obj.m_object;
	//			fs::path clip_path = project_path.parent_path() / clip->m_animationName.cString();
	//			if (find(animations_paths.begin(), animations_paths.end(), clip_path) == animations_paths.end())
	//			{
	//				wrapper.save(load_havok_file<hkaAnimationContainer>(clip_path, wrapper),
	//					out / clip->m_animationName.cString());

	//			}
	//		}
	//	}
	//}

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