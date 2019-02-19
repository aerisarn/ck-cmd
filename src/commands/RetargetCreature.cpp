#include <commands/RetargetCreature.h>

#include "stdafx.h"

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/AnimationCache.h>
#include <core/HKXWrangler.h>

#include <hkbBehaviorReferenceGenerator_0.h>
#include <hkbClipGenerator_2.h>
#include <hkbProjectData_2.h>

using namespace ckcmd::info;
using namespace ckcmd::BSA;
using namespace ckcmd::HKX;

static void InitializeHavok();
static void CloseHavok();

REGISTER_COMMAND_CPP(RetargetCreatureCmd)

RetargetCreatureCmd::RetargetCreatureCmd()
{
}

RetargetCreatureCmd::~RetargetCreatureCmd()
{
}

string RetargetCreatureCmd::GetName() const
{
	return "RetargetCreature";
}

string RetargetCreatureCmd::GetHelp() const
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

string RetargetCreatureCmd::GetHelpShort() const
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

bool RetargetCreatureCmd::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string source_havok_project = "", output_havok_project_name = "";
	if (parsedArgs["<source_havok_project>"].isString())
		source_havok_project = parsedArgs["<source_havok_project>"].asString();
	if (parsedArgs["<output_havok_project_name>"].isString())
		output_havok_project_name = parsedArgs["<output_havok_project_name>"].asString();

	//Games& games = Games::Instance();
	//Games::Game tes5 = Games::TES5;

	//if (!games.isGameInstalled(tes5)) {
	//	Log::Error("This command only works on TES5, and doesn't seem to be installed. Be sure to run the game at least once.");
	//	return false;
	//}

	BSAFile bsa_file("I:\\git_ref\\resources\\bsa\\Skyrim - Animations.bsa");
	const std::regex re_actors("meshes\\\\actors\\\\(?!.*(animations|characters|character assets|characterassets|sharedkillmoves|behaviors)).*hkx", std::regex_constants::icase);
	const std::regex re_misc("(?!.*(actors|animations|characters|character assets|characterassets|sharedkillmoves|behaviors)).*hkx", std::regex_constants::icase);


	vector<string> projects;
	vector<string> misc;
	//find all projects
	vector<string> actors_folders = bsa_file.assets(".*\.hkx");
	for (const auto& hkx_file : actors_folders)
	{
		std::smatch match;
		if (std::regex_match(hkx_file, match, re_actors))
		{
			//yeah, I bet _1st person didn't actually exist when exported
			if (hkx_file.find("_1st") != string::npos)
			{
				projects.push_back("meshes\\actors\\character\\firstperson.hkx");
			}
			else {
				projects.push_back(hkx_file);
			}
		}
		if (std::regex_match(hkx_file, match, re_misc))
		{
			misc.push_back(hkx_file);
		}
	}
	std::sort(projects.begin(), projects.end(),
		[](const string& lhs, const string& rhs) -> bool
	{
		return rhs > lhs;
	});

	std::sort(misc.begin(), misc.end(),
		[](const string& lhs, const string& rhs) -> bool
	{
		return rhs > lhs;
	});


	//AnimationCache::rebuild_from_bsa(bsa_file, projects, misc);

	string animDataContent;
	string animSetDataContent;

	const std::string animDataPath = "meshes\\animationdatasinglefile.txt";
	const std::string animSetDataPath = "meshes\\animationsetdatasinglefile.txt";

	size_t anim_data_size;
	size_t anim_data_set_size;
	const uint8_t* anim_data = bsa_file.extract(animDataPath, anim_data_size);
	const uint8_t* anim_set_data = bsa_file.extract(animSetDataPath, anim_data_set_size);
	string anim_data_content((char*)anim_data, anim_data_size);
	string anim_set_data_content((char*)anim_set_data, anim_data_set_size);


	//const vector<string> bsas = { "Update.bsa", "Skyrim - Animations.bsa" };

	////by priority order, we first check for overrides
	//loadOverrideOrBSA(animDataPath, animDataContent, tes5, bsas);
	//loadOverrideOrBSA(animSetDataPath, animSetDataContent, tes5, bsas);

	AnimationCache cache(anim_data_content, anim_set_data_content);

	cache.check_from_bsa(bsa_file, projects, misc);

	cache.printInfo();

	delete anim_data;
	delete anim_set_data;
	
	HKXWrapper wrapper;
	hkRefPtr<hkbProjectData> project;
	bool in_override = false;
	InitializeHavok();
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