#include <src/Skyrim/TES5File.h>
#include <src/Collection.h>
#include <src/ModFile.h>

#include <commands/DecodeCache.h>

#include "stdafx.h"
#undef max
#undef min

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/AnimationCache.h>
#include <core/HKXWrangler.h>

#include <core/MathHelper.h>

#include <hkbBehaviorReferenceGenerator_0.h>
#include <hkbClipGenerator_2.h>
#include <hkbProjectData_2.h>
#include <hkbCharacterData_7.h>
#include <hkbBehaviorGraph_1.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbStateMachine_4.h>
#include <hkbExpressionDataArray_0.h>
#include <hkbExpressionDataArray_0.h>
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Animation/SplineCompressed/hkaSplineCompressedAnimation.h>
#include <BSSynchronizedClipGenerator_1.h>

#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>



struct ci_less
{
	// case-independent (ci) compare_less binary function
	struct nocase_compare
	{
		bool operator() (const unsigned char& c1, const unsigned char& c2) const {
			return tolower(c1) < tolower(c2);
		}
	};
	bool operator() (const std::string & s1, const std::string & s2) const {
		return std::lexicographical_compare
		(s1.begin(), s1.end(),   // source range
			s2.begin(), s2.end(),   // dest range
			nocase_compare());  // comparison
	}
};

size_t _lower_find(const string& source, const string& pattern)
{
	string lower_source = source;
	std::transform(lower_source.begin(), lower_source.end(), lower_source.begin(), ::tolower);
	string lower_pattern = pattern;
	std::transform(lower_pattern.begin(), lower_pattern.end(), lower_pattern.begin(), ::tolower);
	string::size_type n = 0;
	if ((n = lower_source.find(lower_pattern, n)) != string::npos)
	{
		return n;
	}
	return std::string::npos;
}

string _replace_all(const string& source, const string& pattern, const string& new_pattern)
{
	string result = source;
	string lower_source = source;
	std::transform(lower_source.begin(), lower_source.end(), lower_source.begin(), ::tolower);
	string lower_pattern = pattern;
	std::transform(lower_pattern.begin(), lower_pattern.end(), lower_pattern.begin(), ::tolower);
	string::size_type n = 0;
	while ((n = lower_source.find(lower_pattern, n)) != string::npos)
	{
		lower_source.replace(n, pattern.size(), new_pattern);
		result.replace(n, pattern.size(), new_pattern);
		n += new_pattern.size();
	}
	return result;
}

template<typename T>
typename T::size_type GeneralizedLevensteinDistance(const T &source,
	const T &target,
	typename T::size_type insert_cost = 1,
	typename T::size_type delete_cost = 1,
	typename T::size_type replace_cost = 1) {
	if (source.size() > target.size()) {
		return GeneralizedLevensteinDistance(target, source, delete_cost, insert_cost, replace_cost);
	}

	using TSizeType = typename T::size_type;
	const TSizeType min_size = source.size(), max_size = target.size();
	std::vector<TSizeType> lev_dist(min_size + 1);

	lev_dist[0] = 0;
	for (TSizeType i = 1; i <= min_size; ++i) {
		lev_dist[i] = lev_dist[i - 1] + delete_cost;
	}

	for (TSizeType j = 1; j <= max_size; ++j) {
		TSizeType previous_diagonal = lev_dist[0], previous_diagonal_save;
		lev_dist[0] += insert_cost;

		for (TSizeType i = 1; i <= min_size; ++i) {
			previous_diagonal_save = lev_dist[i];
			if (source[i - 1] == target[j - 1]) {
				lev_dist[i] = previous_diagonal;
			}
			else {
				lev_dist[i] = std::min(std::min(lev_dist[i - 1] + delete_cost, lev_dist[i] + insert_cost), previous_diagonal + replace_cost);
			}
			previous_diagonal = previous_diagonal_save;
		}
	}

	return lev_dist[min_size];
}


using namespace ckcmd::info;
using namespace ckcmd::BSA;
using namespace ckcmd::HKX;

static void InitializeHavok();
static void CloseHavok();


DecodeCacheCmd::DecodeCacheCmd()
{
}

DecodeCacheCmd::~DecodeCacheCmd()
{
}

string DecodeCacheCmd::GetName() const
{
	return "DecodeCache";
}

string DecodeCacheCmd::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd games
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <animation_folder> <dest_folder>\r\n";

	//will need to check this help in console/
	const char help[] =
	R"(Decodes number entries inside weapon styles.
		
	  Arguments:
	  <animation_folder> folder containing both the animation files and the cache txts
	  <dest_folder> decoded files destination folder

	  arguments are mandatory)";

	return usage + help;
}

string DecodeCacheCmd::GetHelpShort() const
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

//HKXWrapper wrapper;

struct clip_compare {
	bool operator() (const hkRefPtr<hkbClipGenerator>& lhs, const hkRefPtr<hkbClipGenerator>& rhs) const {
		return string(lhs->m_name) < string(rhs->m_name);
	}
};


void _extract_clips(BSAFile& bsa_file, const string& behavior_path, const fs::path& root,  set<hkRefPtr<hkbClipGenerator>, clip_compare>& clips)
{
	//string behavior_full_path = fs::canonical(root / behavior_path).string();
	//size_t offset = behavior_full_path.find("meshes", 0);
	//size_t end = behavior_full_path.length();
	//if (offset < end)
	//	behavior_full_path = behavior_full_path.substr(offset, end);
	//if (bsa_file.find(behavior_full_path))
	//{
	//	hkRootLevelContainer* broot = NULL;
	//	string bdata = bsa_file.extract(behavior_full_path);
	//	hkArray<hkVariant> objects;
	//	hkRefPtr<hkbBehaviorGraph> bhkroot = wrapper.load<hkbBehaviorGraph>((const uint8_t *)bdata.c_str(), bdata.size(), broot, objects);
	//	Log::Info("Graph: %s", bhkroot->m_name);
	//	for (const auto& object : objects)
	//	{
	//		if (hkbClipGenerator::staticClass().getSignature() == object.m_class->getSignature())
	//		{
	//			hkRefPtr<hkbClipGenerator> clip = (hkbClipGenerator*)object.m_object;
	//			Log::Info("Clip: %s, animation: %s", clip->m_name, clip->m_animationName);
	//			clips.insert(clip);
	//			clip = broot->findObject<hkbClipGenerator>(clip);
	//		}
	//		if (hkbBehaviorReferenceGenerator::staticClass().getSignature() == object.m_class->getSignature())
	//		{
	//			hkRefPtr<hkbBehaviorReferenceGenerator> sub_graph = (hkbBehaviorReferenceGenerator*)object.m_object;
	//			Log::Info("Sub Graph: %s", sub_graph->m_behaviorName);
	//			extract_clips(
	//				bsa_file,
	//				string(sub_graph->m_behaviorName),
	//				root,
	//				clips
	//			);
	//		}
	//	}
	//}
}


void populateDecodingMap(const fs::path& folder, std::map<std::string, std::string>& decoding_map)
{
	vector<fs::path> hkx_files;
	find_files(folder, ".hkx", hkx_files);
	decoding_map["7891816"] = "hkx";
	for (const auto& hkx_file : hkx_files)
	{
		fs::path hhk_folder = hkx_file.parent_path();
		fs::path filename = hkx_file.filename().replace_extension("");
		fs::path relative = "meshes" / relative_to(hhk_folder, folder);
		string to_crc = relative.string();
		transform(to_crc.begin(), to_crc.end(), to_crc.begin(), ::tolower);
		string::size_type n = 0;
		while ((n = to_crc.find('/', n)) != string::npos)
		{
			to_crc.replace(n, 1, "\\");
			n += 1;
		}
		if (to_crc.at(to_crc.size() - 1) == '\\')
			to_crc = to_crc.substr(0, to_crc.size() - 1);
		long long crc = stoll(HkCRC::compute(to_crc), NULL, 16);
		string crc_str = to_string(crc);
		decoding_map[crc_str] = to_crc;
		long long name_crc = stoll(HkCRC::compute(filename.string()), NULL, 16);
		string name_crc_str = to_string(name_crc);
		decoding_map[name_crc_str] = filename.string();

	}
}

struct IDLENode;

class IDLEVisitor
{
public:
	virtual void visit(IDLENode&) = 0;
};

struct IDLENode 
{
	std::string name;
	std::string event;
	std::vector<std::shared_ptr<IDLENode>> children;

	uint32_t id;
	std::shared_ptr<IDLENode> parent;
	

	void accept(IDLEVisitor& visitor)
	{
		visitor.visit(*this);
	}

	shared_ptr<IDLENode> findChildren(std::string data)
	{
		for (const auto& child : children)
		{
			if (child->name == data)
				return child;
		}
	}

	shared_ptr<IDLENode> findOrAddChildren(std::string data)
	{
		for (const auto& child : children)
		{
			if (child->name == data)
				return child;
		}
		std::shared_ptr<IDLENode> newnode = make_shared<IDLENode>();
		newnode->name = data;
		children.push_back(newnode);
		return newnode;
	}
};

class ConcreteIDLEVisitor : public IDLEVisitor
{
	enum {
		vDefault = 0,
		vEquip = 1,
		vForceEquip = 2,
		vIdle = 3,
		vLoose = 4
	};

	int state = vDefault;



public:

	std::set<std::string> equip_events;

	int equipBranching = 0;
	int forceEquipBranching = 0;

	virtual void visit(IDLENode& node) override
	{
		if (state == vEquip && equipBranching < node.children.size())
			equipBranching = node.children.size();
		if (state == vForceEquip && forceEquipBranching < node.children.size())
			forceEquipBranching = node.children.size();

		int old_state = state;
		if (node.name == "ActionDraw")
			state = vEquip;
		else if (node.name == "ActionForceEquip")
			state = vForceEquip;
		else if (node.name == "ActionIdle")
			state = vIdle;
		else if (node.name == "LOOSE")
			state = vLoose;

		for (const auto& child : node.children)
		{
			child->accept(*this);
		}

		state = old_state;
	};
};



bool DecodeCacheCmd::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string animation_folder = "", dest_folder = "";
	if (parsedArgs["<animation_folder>"].isString())
		animation_folder = parsedArgs["<animation_folder>"].asString();
	if (parsedArgs["<dest_folder>"].isString())
		dest_folder = parsedArgs["<dest_folder>"].asString();
	std::map<std::string, std::string> decoding_map;
	//populateDecodingMap(animation_folder, decoding_map);

	fs::path source_havok_project_cache = fs::path(animation_folder);

	//Collection skyrimCollection = Collection("D:\\SteamLibrary\\steamapps\\common\\Skyrim\\Data", 3);
	//ModFlags masterFlags = ModFlags(0xA);
	//ModFile* esm = skyrimCollection.AddMod("Skyrim.esm", masterFlags);
	//esm = skyrimCollection.AddMod("Update.esm", masterFlags);
	//esm = skyrimCollection.AddMod("Dragonborn.esm", masterFlags);

	//char* argvv[4];
	//argvv[0] = new char();
	//argvv[1] = new char();
	//argvv[2] = new char();
	//argvv[3] = new char();
	//logger.init(4, argvv);

	//skyrimCollection.Load();

	//std::map<std::string, std::map<uint32_t, std::shared_ptr<IDLENode>>> nodes;

	//std::function<std::shared_ptr<IDLENode>(std::string, uint32_t, std::string, std::string)> findOrCreateNode =
	//	[&nodes](std::string behavior, uint32_t id, std::string name, std::string event)
	//{ 
	//	if (nodes[behavior].find(id) == nodes[behavior].end())
	//	{
	//		std::shared_ptr<IDLENode> node = make_shared<IDLENode>();
	//		node->name = name;
	//		node->event = event;
	//		nodes[behavior][id] = node;
	//		return node;
	//	}
	//	return nodes[behavior][id];
	//};

	//std::shared_ptr<IDLENode> root = make_shared<IDLENode>();
	//root->name = "Root";

	////IDLE are full of errors, we do a first pass to check and categorize
	//for (auto idle_record_it = skyrimCollection.FormID_ModFile_Record.begin(); idle_record_it != skyrimCollection.FormID_ModFile_Record.end(); idle_record_it++)
	//{
	//	Record* record = idle_record_it->second;
	//	if (record->GetType() == REV32(IDLE)) {
	//		Sk::IDLERecord* idle = dynamic_cast<Sk::IDLERecord*>(record);
	//		std::string this_behavior = "";
	//		std::string edid = "";

	//		if (!idle->DNAM.IsLoaded())
	//			idle->DNAM.Load();

	//		if (NULL != idle->DNAM.value)
	//			this_behavior = idle->DNAM.value;

	//		if (!idle->EDID.IsLoaded())
	//			idle->EDID.Load();

	//		if (idle->EDID.value == NULL)
	//		{
	//			edid = "Unnamed";
	//		}
	//		else {
	//			edid = idle->EDID.value;
	//		}

	//		//counter check behavior with parent and sibling
	//		if (!idle->ANAM.IsLoaded())
	//			idle->ANAM.Load();
	//

	//		std::string parent_behavior = "";
	//		std::string sibling_behavior = "";
	//		auto parent = idle->ANAM.value.parent;
	//		Sk::IDLERecord* parent_idle = NULL;
	//		auto find_parent_it = skyrimCollection.FormID_ModFile_Record.find(parent);
	//		if (find_parent_it != skyrimCollection.FormID_ModFile_Record.end())
	//		{
	//			auto parent_record = dynamic_cast<Sk::IDLERecord*>(find_parent_it->second);
	//			if (NULL != parent_record && REV32(IDLE) == parent_record->GetType())
	//			{
	//				parent_idle = dynamic_cast<Sk::IDLERecord*>(parent_record);
	//				if (!parent_idle->DNAM.IsLoaded())
	//					parent_idle->DNAM.Load();

	//				if (NULL != parent_idle->DNAM.value)
	//					parent_behavior = parent_idle->DNAM.value;
	//			}
	//		}
	//		auto sibling = idle->ANAM.value.sibling;
	//		auto find_sibling_it = skyrimCollection.FormID_ModFile_Record.find(sibling);
	//		if (find_sibling_it != skyrimCollection.FormID_ModFile_Record.end())
	//		{
	//			auto sibling_record = dynamic_cast<Sk::IDLERecord*>(find_sibling_it->second);
	//			if (NULL != sibling_record && REV32(IDLE) == sibling_record->GetType())
	//			{
	//				Sk::IDLERecord* sibling_idle = dynamic_cast<Sk::IDLERecord*>(sibling_record);
	//				if (!sibling_idle->DNAM.IsLoaded())
	//					sibling_idle->DNAM.Load();

	//				if (NULL != sibling_idle->DNAM.value)
	//					sibling_behavior = sibling_idle->DNAM.value;
	//			}
	//		}

	//		if (!parent_behavior.empty())
	//		{
	//			if (this_behavior.empty())
	//				this_behavior = parent_behavior;
	//			else if (!this_behavior.empty() && 
	//				this_behavior != parent_behavior)
	//			{	
	//				if (!sibling_behavior.empty())
	//				{
	//					//two different behaviors between parent and childer, check the sibling
	//					if (sibling_behavior == parent_behavior)
	//						this_behavior = parent_behavior;
	//				}
	//				else {
	//					//check parent's parent to be sure
	//					std::string parent_parent_behavior;
	//					if (parent_idle != NULL)
	//					{
	//						auto find_parent_it = skyrimCollection.FormID_ModFile_Record.find(parent_idle->ANAM.value.parent);
	//						if (find_parent_it != skyrimCollection.FormID_ModFile_Record.end())
	//						{
	//							auto parent_record = dynamic_cast<Sk::IDLERecord*>(find_parent_it->second);
	//							if (NULL != parent_record && REV32(IDLE) == parent_record->GetType())
	//							{
	//								parent_idle = dynamic_cast<Sk::IDLERecord*>(parent_record);
	//								if (!parent_idle->DNAM.IsLoaded())
	//									parent_idle->DNAM.Load();

	//								if (NULL != parent_idle->DNAM.value)
	//									parent_parent_behavior = parent_idle->DNAM.value;
	//							}
	//						}
	//					}
	//					if (parent_behavior == parent_parent_behavior)
	//						this_behavior = parent_behavior;
	//					else if (parent_parent_behavior.empty())
	//						this_behavior = parent_behavior;
	//				}
	//			}
	//		}
	//		if (this_behavior.empty() && !sibling_behavior.empty())
	//		{
	//			this_behavior = sibling_behavior;
	//		}

	//		while (this_behavior.empty() && parent_idle != NULL)
	//		{
	//			auto find_parent_it = skyrimCollection.FormID_ModFile_Record.find(parent_idle->ANAM.value.parent);
	//			if (find_parent_it != skyrimCollection.FormID_ModFile_Record.end())
	//			{
	//				auto parent_record = dynamic_cast<Sk::IDLERecord*>(find_parent_it->second);
	//				if (NULL != parent_record && REV32(IDLE) == parent_record->GetType())
	//				{
	//					parent_idle = dynamic_cast<Sk::IDLERecord*>(parent_record);
	//					if (!parent_idle->DNAM.IsLoaded())
	//						parent_idle->DNAM.Load();

	//					if (NULL != parent_idle->DNAM.value)
	//						this_behavior = parent_idle->DNAM.value;
	//				}
	//			}
	//			else {
	//				break;
	//			}
	//			parent_idle = dynamic_cast<Sk::IDLERecord*>(find_parent_it->second);
	//		}

	//		if (idle->EDID.value != NULL && 
	//			(
	//				 strcmp(idle->EDID.value, "SabreCatTurnToRun") == 0 ||
	//				 strcmp(idle->EDID.value, "SabreCatIdleToRunLeft90") == 0 ||
	//				 strcmp(idle->EDID.value, "SabreCatIdleToRunLeft180") == 0 ||
	//				 strcmp(idle->EDID.value, "SabreCatIdleToRunRight90") == 0 ||
	//				 strcmp(idle->EDID.value, "SabreCatIdleToRunRight180") == 0
	//			)
	//		)
	//		{
	//			this_behavior = "Actors\\SabreCat\\Behaviors\\SabreCatBehavior.hkx";
	//		}

	//		std::string event = "";
	//		if (!idle->ENAM.IsLoaded())
	//			idle->ENAM.Load();
	//		if (idle->ENAM.value != NULL)
	//			event = idle->ENAM.value;

	//		if (this_behavior.empty())
	//			__debugbreak();

	//		findOrCreateNode(this_behavior, idle_record_it->first, edid, event);
	//	}
	//}

	////set hierarchy
	//for (auto& entry : nodes)
	//{
	//	std::shared_ptr<IDLENode> node = std::make_shared<IDLENode>();
	//	node->parent = root;
	//	node->name = entry.first;
	//	root->children.push_back(node);
	//	for (auto& idle_entry : entry.second)
	//	{
	//		auto find_it = skyrimCollection.FormID_ModFile_Record.find(idle_entry.first);
	//		if (find_it == skyrimCollection.FormID_ModFile_Record.end())
	//		{
	//			__debugbreak();
	//		}
	//		if (find_it->second->GetType() == REV32(IDLE))
	//		{
	//			auto idle = dynamic_cast<Sk::IDLERecord*>(find_it->second);
	//			if (!idle->ANAM.IsLoaded())
	//				idle->ANAM.Load();

	//			auto parent_it = skyrimCollection.FormID_ModFile_Record.find(idle->ANAM.value.parent);
	//			if (parent_it == skyrimCollection.FormID_ModFile_Record.end()) {
	//				auto aact_root = std::make_shared<IDLENode>();
	//				aact_root->parent = node;
	//				aact_root->name = "LOOSE";
	//				aact_root->children.push_back(idle_entry.second);
	//				idle_entry.second->parent = aact_root;
	//				node->children.push_back(aact_root);
	//			}
	//			else if (parent_it->second->GetType() == REV32(IDLE))
	//			{
	//				auto parent = entry.second.at(parent_it->first);
	//				idle_entry.second->parent = parent;
	//				parent->children.push_back(idle_entry.second);
	//			}
	//			else if (parent_it->second->GetType() == REV32(AACT))
	//			{
	//				auto aact = dynamic_cast<Sk::AACTRecord*>(parent_it->second);
	//				if (!aact->EDID.IsLoaded())
	//					aact->EDID.Load();
	//				//aact_root = findOrCreateNode(entry.first, find_it->first, aact->EDID.value, "");
	//				auto aact_root = std::make_shared<IDLENode>();
	//				aact_root->parent = node;
	//				aact_root->name = aact->EDID.value;
	//				aact_root->children.push_back(idle_entry.second);
	//				idle_entry.second->parent = aact_root;
	//				node->children.push_back(aact_root);
	//			}
	//			
	//			
	//			//TODO
	//			if (NULL != idle->ANAM.value.sibling)
	//				auto sibling = skyrimCollection.FormID_ModFile_Record.find(idle->ANAM.value.sibling);
	//		}
	//		else {
	//			__debugbreak();
	//		}
	//	}
	//}
	//std::set<std::string> boh;
	//for (auto& child : root->children)
	//{
	//	ConcreteIDLEVisitor v;
	//	child->accept(v);
	//	auto equip_branching = v.equipBranching;
	//	auto forceequip_branching = v.forceEquipBranching;
	//	if (equip_branching > 1 || forceequip_branching > 1)
	//	{
	//		boh.insert(child->name);
	//	}
	//}

	fs::path animDataPath = fs::path(source_havok_project_cache) / "animationdatasinglefile.txt";
	fs::path animDataSetPath = fs::path(source_havok_project_cache) / "animationsetdatasinglefile.txt";
	Log::Info("Loading cache, loading %s and %s", animDataPath.string().c_str(), animDataSetPath.string().c_str());
	AnimationCache cache(animDataPath, animDataSetPath);


	InitializeHavok();
	HKXWrapper wrapper;
	vector<fs::path> cache_files;
	find_files(source_havok_project_cache / "actors", ".hkx", cache_files);
	std::map<std::string, fs::path> projectpaths;
	for (const auto& file : cache_files)
	{
		hkRootLevelContainer* root = NULL;
		hkbProjectData* hkroot = wrapper.load<hkbProjectData>(file, root);
		if (hkroot != NULL)
		{
			hkbProjectStringData* sdata = hkroot->m_stringData;
			fs::path behaviors_folder = file.parent_path() / "behaviors";
			if (!fs::exists(behaviors_folder))
				continue;
			projectpaths[file.filename().replace_extension().string()] = file.parent_path();
		}
	}


	auto& creatures = cache.creature_entries;

	if (projectpaths.size() != creatures.size())
		__debugbreak();

	//auto& projectlist = cache.animationSetData.getProjectsList().getStrings();
	//auto& projectdata = cache.animationSetData.getProjectAttackList();

	std::map<std::string, int> global_variables;

	for (int i = 0; i < creatures.size(); i++)
	{


		if (creatures[i].getProjectSetFiles().size() > 1)
		{
			std::cout << "Multi set project: " << creatures[i].name << std::endl;

			auto files = creatures[i].block.getProjectFiles().getStrings();
			string lower = creatures[i].name;
			transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return tolower(c); });
			if (projectpaths.find(lower) == projectpaths.end())
				__debugbreak();
			std::vector<hkbStateMachine*> fsms;
			std::vector<std::vector<std::string>> behaviors_events;
			std::vector<std::vector<std::string>> behaviors_variables;
			std::map<hkbStateMachine*, size_t> fsm_events_index;
			std::cout << "\tbehaviors: " << std::endl;
			for (const auto& file : files)
			{
				if (file.find("Behavior") == 0)
				{
					fs::path behavior_path = projectpaths.at(lower) / file;
					if (!fs::exists(behavior_path))
						__debugbreak;
					std::cout << "\t\t" << file << std::endl;
					hkRootLevelContainer* root = NULL;
					hkArray<hkVariant> objects;
					hkbBehaviorGraph* hkroot = wrapper.load<hkbBehaviorGraph>(behavior_path, root, objects);
					hkbBehaviorGraphStringData* sdata = hkroot->m_data->m_stringData;
					size_t events_index = behaviors_events.size();
					std::vector<std::string> events;
					for (int e = 0; e < sdata->m_eventNames.getSize(); e++)
					{
						events.push_back(sdata->m_eventNames[e].cString());
					}
					behaviors_events.push_back(events);
					std::vector<std::string> variables;
					for (int e = 0; e < sdata->m_variableNames.getSize(); e++)
					{
						variables.push_back(sdata->m_variableNames[e].cString());
						if (global_variables.find(sdata->m_variableNames[e].cString()) != global_variables.end())
							global_variables[sdata->m_variableNames[e].cString()]++;
						else
							global_variables[sdata->m_variableNames[e].cString()] = 1;
					}
					behaviors_variables.push_back(variables);
					for (int n = 0; n < objects.getSize(); n++)
					{
						if (objects[n].m_class == &hkbStateMachineClass)
						{
							fsms.push_back((hkbStateMachine*)objects[n].m_object);
							fsm_events_index[(hkbStateMachine*)objects[n].m_object] = events_index;
						}
					}
				}
			}

			auto sets = creatures[i].getProjectSetFiles();
			auto data = creatures[i].getProjectAttackBlocks();

			std::multimap<std::string, std::string> events_text;
			std::multimap<std::string, std::string> variables_text;
			std::set<std::string> events_to_find;
			std::set<std::string> variables_to_find;
			for (int j = 0; j < sets.size(); j++)
			{

				auto events = data[j].getSwapEventsList().getStrings();
				auto variables = data[j].getHandVariableData().getVariables();
				auto attacks = data[j].getAttackData().attackData;

				if (!attacks.empty() || !variables.empty())
				{
					for (const auto& event : events)
					{
						events_to_find.insert(event);
					}
					for (const auto& variable : variables)
					{
						variables_to_find.insert(variable.variable_name);
					}
				}
			}

			for ( auto* hk_fsm : fsms)
			{
				std::map<int, std::string> states;
				for (int fsm_child_index = 0; fsm_child_index < hk_fsm->m_states.getSize(); ++fsm_child_index)
				{
					hkbStateMachineStateInfo* state = (hkbStateMachineStateInfo*)hk_fsm->m_states[fsm_child_index];
					states[state->m_stateId] = state->m_name.cString();;
				}

				//Visit Bindings
				if (NULL != hk_fsm->m_variableBindingSet)
				{
					auto& bindings = hk_fsm->m_variableBindingSet->m_bindings;
					for (int b = 0; b < bindings.getSize(); ++b)
					{
						auto& binding = bindings[b];
						auto variable_name = behaviors_variables.at(fsm_events_index.at(hk_fsm)).at(binding.m_variableIndex);
						if (variables_to_find.find(variable_name) != variables_to_find.end())
						{
							std::string binding_text = "binds path " +
								std::string(binding.m_memberPath.cString()) +
								" of " + std::string(hk_fsm->m_name.cString());
							variables_text.insert({ variable_name, binding_text });
						}
					}
				}

				//Visit FSM wildcard transitions
				if (NULL != hk_fsm->m_wildcardTransitions)
				{
					auto& transitions = hk_fsm->m_wildcardTransitions->m_transitions;
					for (int t = 0; t < transitions.getSize(); t++)
					{
						auto& transition = transitions[t];
						std::string set_event = behaviors_events.at(fsm_events_index.at(hk_fsm)).at(transition.m_eventId);
						if (events_to_find.find(set_event) != events_to_find.end())
						{
							std::string transition_text = "transitions " + 
								std::string(hk_fsm->m_name.cString()) + " into " + 
								states[transition.m_toStateId] +
								+ " nested " + std::to_string(transition.m_toNestedStateId);
							events_text.insert({ set_event, transition_text });
						}
					}
				}

				//Visit parent FSM states
				for (int fsm_child_index = 0; fsm_child_index < hk_fsm->m_states.getSize(); ++fsm_child_index)
				{
					hkbStateMachineStateInfo* state = hk_fsm->m_states[fsm_child_index];
					if (NULL != state->m_transitions)
					{
						auto& transitions = state->m_transitions->m_transitions;
						for (int t = 0; t < transitions.getSize(); t++)
						{
							auto& transition = transitions[t];
							std::string set_event = behaviors_events.at(fsm_events_index.at(hk_fsm)).at(transition.m_eventId);
							if (events_to_find.find(set_event) != events_to_find.end())
							{
								std::string transition_text = "transitions " +
									std::string(hk_fsm->m_name.cString()) +
									" from " + state->m_name.cString() +
									" into " + states[transition.m_toStateId] +
									" nested " + std::to_string(transition.m_toNestedStateId);
								events_text.insert({ set_event, transition_text });
							}
						}
					}
				}
			}
			
			for (int j = 0; j < sets.size(); j++)
			{

				auto events = data[j].getSwapEventsList().getStrings();
				auto variables = data[j].getHandVariableData().getVariables();
				auto attacks = data[j].getAttackData().attackData;

				if (!attacks.empty() || !variables.empty())
				{
					std::cout << "\tset: " << sets[j] << std::endl;
					std::cout << "\tevents: " << std::endl;
					for (const auto& event : events)
					{
						std::cout << "\t\t" << event << std::endl;
						std::pair <std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> ret;
						ret = events_text.equal_range(event);
						for (std::multimap<std::string, std::string>::iterator it = ret.first; it != ret.second; ++it)
						{
							std::cout << "\t\t\t" << it->second << std::endl;
						}
					}
					if (!variables.empty())
					{
						std::cout << "\tvariables: " << std::endl;
						for (const auto& variable : variables)
						{
							std::cout << "\t\t" << variable.variable_name << " (" << variable.value_min << " -> " << variable.value_max << ")" << std::endl;
							std::pair <std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> ret;
							ret = variables_text.equal_range(variable.variable_name);
							for (std::multimap<std::string, std::string>::iterator it = ret.first; it != ret.second; ++it)
							{
								std::cout << "\t\t\t" << it->second << std::endl;
							}
						}
					}
					if (!attacks.empty())
					{
						std::cout << "\tattacks: " << std::endl;
						for (const auto& attack : attacks)
						{
							std::cout << "\t\t" << attack.eventName << std::endl;
						}
					}
				}
			}
		}
	}

	int debug = 1;

	//for (const auto& dirEntry : fs::directory_iterator(cache_root_folder))
	//{
	//	if (fs::is_directory(dirEntry))
	//	{
	//		std::string multi_file_txt = dirEntry.path().filename().string();
	//		multi_file_txt = multi_file_txt.substr(0, multi_file_txt.size() - 4); //data
	//		fs::path txt_file = dirEntry.path() / (multi_file_txt + ".txt");
	//		if (fs::exists(txt_file))
	//		{
	//			//Multiproject
	//			std::ifstream infile(txt_file);
	//			std::string line;
	//			std::set
	//			while (std::getline(infile, line))
	//			{
	//				std::istringstream iss(line);

	//				// process pair (a,b)
	//			}
	//			std::cout << txt_file << std::endl;
	//		}

	//	}

	//}
	/*vector<fs::path> cache_files;
	find_files(cache_root_folder, ".txt", cache_files);
	fs::create_directories(dest_folder);
	for (const auto& cache_file : cache_files)
	{
		std::ifstream t(cache_file.string());
		std::string str;

		t.seekg(0, std::ios::end);
		str.reserve(t.tellg());
		t.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());

		for (const auto& entry : decoding_map)
			str = _replace_all(str, entry.first, entry.second);

		fs::path relative = relative_to(cache_file, cache_root_folder);
		fs::path output = dest_folder / relative;
		fs::create_directories(output.parent_path());
		std::ofstream o(output.string());
		o << str;
		o.close();
	}*/



	//if (!fs::exists(source_havok_project_cache) || !fs::is_regular_file(source_havok_project_cache)) {
	//	Log::Error("Invalid cache skeleton file: %s", source_havok_project_cache.c_str());
	//	return false;
	//}

	//if (!fs::exists(source_havok_project_folder) || !fs::is_directory(source_havok_project_folder)) {
	//	Log::Error("Invalid havok project path: %s", source_havok_project_folder.c_str());
	//	return false;
	//}

	//if (output_havok_project_name.empty()) {
	//	Log::Error("Invalid target project name: %s", output_havok_project_name.c_str());
	//	return false;
	//}

	//fs::path animDataPath = fs::path(source_havok_project_cache).parent_path().parent_path() / "animationdatasinglefile.txt";
	//fs::path animDataSetPath = fs::path(source_havok_project_cache).parent_path().parent_path() / "animationsetdatasinglefile.txt";
	//Log::Info("Loading cache, loading %s and %s", animDataPath.string().c_str(), animDataSetPath.string().c_str());
	//AnimationCache cache(animDataPath, animDataSetPath);
	//Log::Info("Loaded. Creating target dir: %s", output_havok_project_name.c_str());
	//fs::path output = fs::path(".") / output_havok_project_name;
	//fs::create_directories(output);
	//vector<fs::path> projects;
	//find_files_non_recursive(source_havok_project_folder, ".hkx", projects);

	//fs::path project_file;
	//string cache_name = fs::path(source_havok_project_cache).filename().replace_extension("").string();
	//int distance = std::numeric_limits<int>::max();
	//for (const auto& file : projects) {
	//	int actual_distance = GeneralizedLevensteinDistance
	//	(
	//		file.filename().replace_extension("").string(),
	//		cache_name
	//	);
	//	if (actual_distance < distance) {
	//		project_file = file;
	//		distance = actual_distance;
	//	}
	//}

	//Log::Info("Project copied. Retargeting project: %s", project_file.string().c_str());
	//InitializeHavok();
	//hkRootLevelContainer* root = NULL;
	//hkbProjectData* hkroot = wrapper.load<hkbProjectData>(project_file, root);
	//if (hkroot == NULL)
	//{
	//	Log::Error("Unable to load havok project file: %s", project_file.string().c_str());
	//	return false;
	//}
	//std::map<string, string, ci_less> retarget_map;
	//std::map<string, string, ci_less> retarget_movt_map;
	//hkbProjectStringData* sdata = hkroot->m_stringData;
	//fs::path old_char_file = source_havok_project_folder / fs::path(sdata->m_characterFilenames[0].cString());
	//fs::path new_char_name;
	//if (sdata->m_characterFilenames.getSize() <= 0)
	//{
	//	Log::Error("Havok project file contains no character file, exiting");
	//	return false;
	//}
	//string old_name = fs::path(source_havok_project_folder).filename().string();//hk_ch_root->m_stringData->m_name;
	//camel(old_name);
	//new_char_name = replace_all(string(sdata->m_characterFilenames[0].cString()), old_name, output_havok_project_name);
	//retarget_map[string(sdata->m_characterFilenames[0].cString())] = new_char_name.string();
	//Log::Info("Project's character: %s, new name: %s", sdata->m_characterFilenames[0], new_char_name.string().c_str());
	//sdata->m_characterFilenames[0] = new_char_name.string().c_str();
	//wrapper.write(root, output / (output_havok_project_name + "project.hkx"));

	//Log::Info("Project file retargeted. loading %s", old_char_file.string().c_str());
	//root = NULL;
	//hkbCharacterData* hk_ch_root = wrapper.load<hkbCharacterData>(old_char_file, root);
	//if (hk_ch_root == NULL)
	//{
	//	Log::Error("Unable to load havok character file: %s", old_char_file.string().c_str());
	//	return false;
	//}

	//hk_ch_root->m_stringData->m_name = output_havok_project_name.c_str();

	//string old_behavior_name = hk_ch_root->m_stringData->m_behaviorFilename.cString();
	//fs::path new_behavior_relative_path = fs::path(old_behavior_name).parent_path() / fs::path(replace_all(old_behavior_name, old_name, output_havok_project_name)).filename();
	//retarget_map[old_behavior_name] = new_behavior_relative_path.string();
	//fs::path old_behavior_file = fs::path(source_havok_project_folder) / old_behavior_name;
	//hk_ch_root->m_stringData->m_behaviorFilename = new_behavior_relative_path.string().c_str();
	//fs::path old_rig_path = fs::path(source_havok_project_folder) / string(hk_ch_root->m_stringData->m_rigName);
	////Just upgrade
	//hkRootLevelContainer* rig_root = NULL;
	//if (NULL == wrapper.load<hkaAnimationContainer>(old_rig_path, rig_root))
	//{
	//	Log::Error("Unable to load skeleton file: %s", old_rig_path.string().c_str());
	//	return false;
	//}
	//else {
	//	fs::create_directories(fs::path(fs::path(output) / string(hk_ch_root->m_stringData->m_rigName)).parent_path());
	//	wrapper.write(rig_root, output / string(hk_ch_root->m_stringData->m_rigName));
	//}

	////Check paired
	//for (int i = 0; i < hk_ch_root->m_stringData->m_animationNames.getSize(); i++)
	//{
	//	string name = hk_ch_root->m_stringData->m_animationNames[i];
	//	if (name.find("..") == 0) {
	//		Log::Warn("found an animation outside of the creature folder: %s", name.c_str());
	//		//these animations are not in the current copied folder
	//		//do they contain the old name?
	//		if (lower_find(name, old_name) != string::npos) {
	//			string new_name = name;
	//			new_name = replace_all(new_name, old_name, output_havok_project_name);
	//			Log::Info("Will substitute %s references with %s", name.c_str(), new_name.c_str());
	//			hk_ch_root->m_stringData->m_animationNames[i] = new_name.c_str();
	//			retarget_map[name] = new_name;
	//			if (fs::exists(fs::path(source_havok_project_folder) / name) &&
	//				fs::is_regular_file(fs::path(source_havok_project_folder) / name))
	//			{
	//				Log::Info("Found %s", (fs::path(source_havok_project_folder) / name).string().c_str());					
	//				hkRootLevelContainer* root = NULL;
	//				hkArray<hkVariant> objects;
	//				hkRefPtr<hkaAnimationContainer> hkroot = wrapper.load<hkaAnimationContainer>(fs::path(source_havok_project_folder) / name, root, objects);
	//				//retarget events
	//				for (int o = 0; o < objects.getSize(); o++)
	//				{
	//					if (objects[o].m_class == &hkaSplineCompressedAnimationClass) {
	//						hkaSplineCompressedAnimation* anim = (hkaSplineCompressedAnimation*)objects[o].m_object;
	//						for (int e = 0; e < anim->m_annotationTracks.getSize(); e++) {
	//							auto& annotation = anim->m_annotationTracks[e];
	//							for (int a = 0; a < annotation.m_annotations.getSize(); a++) {
	//								auto& a_object = annotation.m_annotations[a];
	//								string text = a_object.m_text;
	//								text = replace_all(text, old_name, output_havok_project_name);
	//								a_object.m_text = text.c_str();
	//							}
	//						}
	//					}
	//				}
	//				
	//				if (hkroot == NULL)
	//				{
	//					Log::Error("Unable to load havok project file: %s", project_file.string().c_str());
	//				}
	//				else {
	//					string out = new_name;
	//					transform(out.begin(), out.end(), out.begin(), ::tolower);
	//					fs::create_directories(fs::path(new_name).parent_path());
	//					wrapper.write(root, out);
	//				}
	//				
	//				Log::Info("Copied (and upgraded) into %s", fs::canonical(new_name).string().c_str());
	//			}
	//			else {
	//				Log::Error("COULD NOT FIND %s, copy it and rename manually (and upgrade if LE format)", (fs::path(source_havok_project_folder) / name).string().c_str());
	//			}
	//		}
	//	}
	//	else {
	//		//Just upgrade
	//		hkRootLevelContainer* root = NULL;
	//		hkArray<hkVariant> objects;
	//		hkRefPtr<hkaAnimationContainer> hkroot = wrapper.load<hkaAnimationContainer>(fs::path(source_havok_project_folder) / name, root, objects);
	//		//retarget events
	//		for (int o = 0; o < objects.getSize(); o++)
	//		{
	//			if (objects[o].m_class == &hkaSplineCompressedAnimationClass) {
	//				hkaSplineCompressedAnimation* anim = (hkaSplineCompressedAnimation*)objects[o].m_object;
	//				for (int e = 0; e < anim->m_annotationTracks.getSize(); e++) {
	//					auto& annotation = anim->m_annotationTracks[e];
	//					for (int a = 0; a < annotation.m_annotations.getSize(); a++) {
	//						auto& a_object = annotation.m_annotations[a];
	//						string text = a_object.m_text;
	//						text = replace_all(text, old_name, output_havok_project_name);
	//						a_object.m_text = text.c_str();
	//					}
	//				}
	//			}
	//		}
	//		
	//		if (hkroot == NULL)
	//		{
	//			Log::Error("Unable to load animation file file: %s", (fs::path(source_havok_project_folder) / name).string().c_str());
	//		}
	//		else {
	//			string out = (output / name).string();
	//			transform(out.begin(), out.end(), out.begin(), ::tolower);
	//			fs::create_directories(fs::path(out).parent_path());
	//			wrapper.write(root, out);
	//		}
	//	}
	//	// Calculate relative path for crc
	//	fs::path abs = fs::canonical(fs::path(source_havok_project_folder) / name).parent_path();
	//	fs::path base = source_havok_project_folder;
	//	while (base.has_parent_path())
	//	{
	//		base = base.parent_path();
	//		if (base.filename().string() == "meshes")
	//			break;
	//	}
	//	if (base.string().find("meshes") == string::npos) {
	//		Log::Error("Cannot find meshes folder into the source_havok_project_folder! Please use the original extracted bsa path fully!");
	//		return false;
	//	}
	//	fs::path relative = "meshes" / relative_to(abs, base);
	//	string to_crc = relative.string();
	//	transform(to_crc.begin(), to_crc.end(), to_crc.begin(), ::tolower);
	//	string::size_type n = 0;
	//	while ((n = to_crc.find('/', n)) != string::npos)
	//	{
	//		to_crc.replace(n, 1, "\\");
	//		n += 1;
	//	}
	//	if (to_crc.at(to_crc.size() - 1) == '\\')
	//		to_crc = to_crc.substr(0, to_crc.size() - 1);
	//	long long crc = stoll(HkCRC::compute(to_crc),NULL,16);
	//	string crc_str = to_string(crc);
	//	//new
	//	string lower_output = output_havok_project_name;
	//	transform(lower_output.begin(), lower_output.end(), lower_output.begin(), ::tolower);
	//	string lower_project = fs::path(source_havok_project_folder).filename().string();
	//	transform(lower_project.begin(), lower_project.end(), lower_project.begin(), ::tolower);
	//	string new_to_crc = to_crc;
	//	new_to_crc = replace_all(new_to_crc, lower_project, lower_output);
	//	long long new_crc = stoll(HkCRC::compute(new_to_crc), NULL, 16);
	//	string new_crc_str = to_string(new_crc);
	//	retarget_map[crc_str] = new_crc_str;

	//}
	//fs::create_directories(fs::path(fs::path(output) / new_char_name).parent_path());
	//wrapper.write(root, output / new_char_name);
	//fs::path old_behavior_dir = source_havok_project_folder / fs::path(old_behavior_name).parent_path();
	//vector<fs::path> behaviors;
	//set<string> retarget_SOUN;
	//set<string> retarget_MOVT;
	//find_files_non_recursive(old_behavior_dir, ".hkx", behaviors);
	//for (const auto& file : behaviors) {
	//	hkArray<hkVariant> objects;
	//	hkRootLevelContainer* root = NULL;
	//	hkRefPtr<hkbBehaviorGraph> bhkroot = wrapper.load<hkbBehaviorGraph>(file, root, objects);
	//	Log::Info("Graph: %s", bhkroot->m_name);
	//	Log::Info("Retargeting Events:");
	//	for (int i = 0; i < bhkroot->m_data->m_stringData->m_eventNames.getSize(); i++)
	//	{
	//		string event_name = bhkroot->m_data->m_stringData->m_eventNames[i];
	//		if (lower_find(event_name,old_name) != string::npos)
	//		{
	//			string new_name = event_name;
	//			new_name = replace_all(new_name, old_name, output_havok_project_name);
	//			Log::Info("Will substitute %s references with %s", event_name.c_str(), new_name.c_str());
	//			if (event_name.find("SoundPlay.") == 0)
	//				retarget_SOUN.insert(event_name.substr(sizeof("SoundPlay.")-1, event_name.size()));
	//			retarget_map[string(bhkroot->m_data->m_stringData->m_eventNames[i])] = new_name;
	//			bhkroot->m_data->m_stringData->m_eventNames[i] = new_name.c_str();
	//		}
	//		else if (event_name.find("SoundPlay.") == 0) {
	//			string new_name = "SoundPlay." + output_havok_project_name + event_name.substr(event_name.find(".") + 1, event_name.length() - event_name.find(".") - 1);
	//			Log::Info("Will substitute %s references with %s", event_name.c_str(), new_name.c_str());
	//			retarget_SOUN.insert(event_name.substr(sizeof("SoundPlay.") - 1, event_name.size()));
	//			retarget_map[string(bhkroot->m_data->m_stringData->m_eventNames[i])] = new_name;
	//			bhkroot->m_data->m_stringData->m_eventNames[i] = new_name.c_str();
	//		}
	//	}
	//	Log::Info("Retargeting Variables:");
	//	for (int i = 0; i < bhkroot->m_data->m_stringData->m_variableNames.getSize(); i++)
	//	{
	//		string variable_name = bhkroot->m_data->m_stringData->m_variableNames[i];
	//		if (lower_find(variable_name,old_name) != string::npos)
	//		{
	//			string new_name = variable_name;
	//			new_name = replace_all(new_name, old_name, output_havok_project_name);
	//			Log::Info("Will substitute %s references with %s", variable_name.c_str(), new_name.c_str());
	//			if (variable_name.find("iState_") == 0) {
	//				retarget_MOVT.insert(variable_name.substr(sizeof("iState_") - 1, variable_name.size()));
	//			}
	//			retarget_map[string(bhkroot->m_data->m_stringData->m_variableNames[i])] = new_name;
	//			retarget_movt_map[variable_name.substr(sizeof("iState_") - 1, variable_name.size())] = new_name.substr(sizeof("iState_") - 1, new_name.size());
	//			bhkroot->m_data->m_stringData->m_variableNames[i] = new_name.c_str();				
	//		} else if (variable_name.find("iState_") == 0) {
	//			//hackity hack, damn mudcrab
	//			auto first_underscore_pos = variable_name.find("_");
	//			auto second_underscore_pos = variable_name.find("_", first_underscore_pos+1);
	//			if (second_underscore_pos != string::npos)
	//			{
	//				string new_name = "iState_" + output_havok_project_name + variable_name.substr(second_underscore_pos, variable_name.size() - second_underscore_pos);
	//				Log::Info("Will substitute %s references with %s", variable_name.c_str(), new_name.c_str());
	//				retarget_map[string(bhkroot->m_data->m_stringData->m_variableNames[i])] = new_name;
	//				retarget_movt_map[variable_name.substr(sizeof("iState_") - 1, variable_name.size())] = new_name.substr(sizeof("iState_") - 1, new_name.size());
	//				retarget_MOVT.insert(variable_name.substr(sizeof("iState_") - 1, variable_name.size()));
	//			}
	//		}

	//	}
	//	for (const auto& object : objects)
	//	{
	//		if (hkbClipGenerator::staticClass().getSignature() == object.m_class->getSignature())
	//		{
	//			hkRefPtr<hkbClipGenerator> clip = (hkbClipGenerator*)object.m_object;
	//			
	//			auto it = retarget_map.find(string(clip->m_animationName));
	//			if (it != retarget_map.end())
	//			{
	//				Log::Info("Retargeting clip: %s, animation: %s to %s", clip->m_name, clip->m_animationName, it->second.c_str());
	//				clip->m_animationName = it->second.c_str();
	//			}
	//			string clip_name = clip->m_name;
	//			clip_name = replace_all(clip_name, old_name, output_havok_project_name);
	//			clip->m_name = clip_name.c_str();
	//		}
	//		if (BSSynchronizedClipGenerator::staticClass().getSignature() == object.m_class->getSignature())
	//		{
	//			hkRefPtr<BSSynchronizedClipGenerator> clip = (BSSynchronizedClipGenerator*)object.m_object;

	//			string clip_name = clip->m_name;
	//			clip_name = replace_all(clip_name, old_name, output_havok_project_name);
	//			clip->m_name = clip_name.c_str();
	//		}
	//		if (hkbExpressionDataArray::staticClass().getSignature() == object.m_class->getSignature())
	//		{
	//			hkRefPtr<hkbExpressionDataArray> expression = (hkbExpressionDataArray*)object.m_object;
	//			for (int i = 0; i < expression->m_expressionsData.getSize(); i++)
	//			{
	//				auto data = expression->m_expressionsData[i];
	//				string expression_string = expression->m_expressionsData[i].m_expression;
	//				if (lower_find(expression_string,old_name) != string::npos)
	//				{
	//					string new_name = expression_string;
	//					new_name = replace_all(new_name, old_name, output_havok_project_name);
	//					Log::Info("Will substitute %s expression with %s", expression_string.c_str(), new_name.c_str());
	//					expression->m_expressionsData[i].m_expression = new_name.c_str();
	//				}
	//			}
	//		}
	//	}
	//	string relative = (file.parent_path().filename() / file.filename()).string();
	//	auto it = retarget_map.find(string(relative));
	//	fs::path behavior_output_file = output;
	//	if (it != retarget_map.end())
	//		behavior_output_file = behavior_output_file / it->second;
	//	else
	//		behavior_output_file = behavior_output_file / relative;
	//	fs::create_directories(behavior_output_file.parent_path());
	//	wrapper.write(root, behavior_output_file);
	//}

	////Now adjust the cache
	//string output_havok_project_lower_name = output_havok_project_name;
	//transform(output_havok_project_lower_name.begin(), output_havok_project_lower_name.end(), output_havok_project_lower_name.begin(), ::tolower);
	//auto cache_ptr = cache.cloneCreature(cache_name, output_havok_project_lower_name + "project");
	////retarget caches
	//auto block = cache_ptr->block.toASCII();
	//block = replace_all(block, old_name, output_havok_project_name);
	//cache_ptr->block.clear();
	//cache_ptr->block.fromASCII(block);
	//auto set_block = cache_ptr->sets.getBlock();
	//set_block = replace_all(set_block, old_name, output_havok_project_name);
	////adjust crc
	//for (const auto& it : retarget_map) {
	//	string token = it.first;
	//	set_block = replace_all(set_block, token, it.second);
	//}
	//cache_ptr->sets.clear();
	//cache_ptr->sets.parseBlock(scannerpp::Scanner(set_block));

	//cache.save_creature(output_havok_project_name + "project", cache_ptr, "animationdatasinglefile.txt", "animationsetdatasinglefile.txt");


	//Games& games = Games::Instance();
	//Games::Game tes5 = Games::TES5;

	//fs::path tes5_data;

	//if (!games.isGameInstalled(tes5)) {
	//	if (skyrim_le_folder.empty())
	//	{
	//		Log::Error("This command only works on TES5, and doesn't seem to be installed. Be sure to run the game at least once.");
	//		Log::Error("Please use -s parameter to point over an installation");
	//		return false;
	//	}
	//	else {
	//		Log::Info("Using Skyrim LE installed at: %s", skyrim_le_folder.c_str());
	//		tes5_data = fs::path(skyrim_le_folder) / "Data";
	//	}
	//}
	//else {
	//	tes5_data = games.data(tes5);
	//}

	//Collection skyrimCollection = Collection((char * const)(tes5_data.string().c_str()), 3);
	//ModFlags masterFlags = ModFlags(0xA);
	//ModFlags skyblivionFlags = ModFlags(0xA);
	//ModFile* esm = skyrimCollection.AddMod("Skyrim.esm", masterFlags);
	//ModFile* update = skyrimCollection.AddMod("Update.esm", masterFlags);
	//ModFile* dawnguard = skyrimCollection.AddMod("Dawnguard.esm", masterFlags);
	//ModFile* Hearthfires = skyrimCollection.AddMod("Hearthfires.esm", masterFlags);
	//ModFile* Dragonborn = skyrimCollection.AddMod("Dragonborn.esm", masterFlags);
	//ModFlags espFlags = ModFlags(0x1818);
	//espFlags.IsNoLoad = false;
	//espFlags.IsFullLoad = true;
	//espFlags.IsMinLoad = false;
	//espFlags.IsCreateNew = true;
	//ModFile* skyrimMod = skyrimCollection.AddMod("template.esp", espFlags);

	//char * argvv[4];
	//argvv[0] = new char();
	//argvv[1] = new char();
	//argvv[2] = new char();
	//argvv[3] = new char();
	//logger.init(4, argvv);

	//skyrimCollection.Load();
	//auto it = skyrimCollection.FormID_ModFile_Record.equal_range(REV32(IDLE));

	//map<string, Sk::MOVTRecord*> movts;
	//map<string, Sk::SNDRRecord*> sndrs;
	//map<string, Sk::IDLERecord*> idles;
	//map<string, Sk::SOUNRecord*> souns;
	//for (auto idle_record_it = skyrimCollection.FormID_ModFile_Record.begin(); idle_record_it != skyrimCollection.FormID_ModFile_Record.end(); idle_record_it++)
	//{
	//	Record* record = idle_record_it->second;
	//	if (record->GetType() == REV32(MOVT)) {
	//		Sk::MOVTRecord* movt = dynamic_cast<Sk::MOVTRecord*>(record);
	//		if (retarget_MOVT.find(string(movt->MNAM.value)) != retarget_MOVT.end())
	//		{
	//			Log::Info("Found MOVT to retarget: %s", movt->EDID.value);
	//			movts[string(movt->EDID.value)] = movt;
	//		}
	//	}
	//	if (record->GetType() == REV32(SNDR)) {
	//		Sk::SNDRRecord* sndr = dynamic_cast<Sk::SNDRRecord*>(record);
	//		string SDless = string(sndr->EDID.value).substr(0, string(sndr->EDID.value).size() - 2);
	//		if (retarget_SOUN.find(SDless) != retarget_SOUN.end() || retarget_SOUN.find(string(sndr->EDID.value)) != retarget_SOUN.end())
	//		{
	//			Log::Info("Found SOUN to retarget: %s", sndr->EDID.value);
	//			sndrs[string(sndr->EDID.value)] = sndr;
	//		}
	//	}
	//	if (record->GetType() == REV32(SOUN)) {
	//		Sk::SOUNRecord* soun = dynamic_cast<Sk::SOUNRecord*>(record);
	//		string SDless = string(soun->EDID.value).substr(0, string(soun->EDID.value).size() - 2);
	//		if (retarget_SOUN.find(SDless) != retarget_SOUN.end() || retarget_SOUN.find(string(soun->EDID.value)) != retarget_SOUN.end())
	//		{
	//			Log::Info("Found SNDR to retarget: %s", soun->EDID.value);
	//			souns[string(soun->EDID.value)] = soun;
	//		}
	//	}
	//	if (record->GetType() == REV32(IDLE)) {
	//		Sk::IDLERecord* idle = dynamic_cast<Sk::IDLERecord*>(record);
	//		vector<Sk::IDLERecord*> to_add;
	//		char* this_behavior = NULL;
	//		Sk::IDLERecord* parent = idle;
	//		while (parent != NULL) {
	//			if (!parent->DNAM.IsLoaded())
	//				parent->DNAM.Load();
	//			auto behavior = parent->DNAM.value;
	//			if (behavior != NULL && !string(behavior).empty() &&
	//				lower_find(behavior, old_name) == std::string::npos)
	//			{
	//				to_add.clear();
	//				break;
	//			}
	//			else if (behavior != NULL && !string(behavior).empty() &&
	//				lower_find(behavior, old_name) != std::string::npos)
	//			{

	//				this_behavior = behavior;
	//			}
	//			auto references = parent->ANAM;
	//			to_add.push_back(parent);
	//			auto find_it = skyrimCollection.FormID_ModFile_Record.find(references.value.parent);
	//			if (find_it == skyrimCollection.FormID_ModFile_Record.end() ||					
	//				find_it->second->GetType() != REV32(IDLE)) {
	//				break;
	//			}				
	//			parent = dynamic_cast<Sk::IDLERecord*>(find_it->second);
	//		}
	//		for (const auto& this_idle : to_add) {
	//			if (this_idle->DNAM.value == NULL || strcmp(this_idle->DNAM.value, this_behavior) != 0)
	//			{
	//				if (this_idle->DNAM.value != NULL)
	//					delete this_idle->DNAM.value;
	//				this_idle->DNAM.value = new char[strlen(this_behavior) + 1];
	//				strcpy(this_idle->DNAM.value, this_behavior);
	//			}
	//			if (idles.insert({ string(this_idle->EDID.value),this_idle }).second)
	//				Log::Info("Found IDLE to retarget: %s", this_idle->EDID.value);;
	//		}
	//	}
	//}

	//std::map<FORMID, FORMID> retargeted;
	//std::vector<Sk::IDLERecord*> new_records;

	//for (auto& movt_it : movts) {
	//	auto movt = movt_it.second;
	//	std::string new_edid = retarget_movt_map[movt->MNAM.value];
	//	std::string new_MNAM = retarget_movt_map[movt->MNAM.value];

	//	// declaring character array 
	//	char* char_array = new char[new_edid.size() + 1];

	//	// copying the contents of the 
	//	// string to char array 
	//	strcpy(char_array, new_edid.c_str());

	//	Record* to_copy = static_cast<Record*>(movt);
	//	Record* result = skyrimCollection.CopyRecord(to_copy, skyrimMod, NULL, NULL, char_array, 0);
	//	Sk::MOVTRecord* copied = dynamic_cast<Sk::MOVTRecord*>(result);

	//	char* mnam_char = new char[new_MNAM.size() + 1];
	//	strcpy(mnam_char, new_MNAM.c_str());
	//	copied->EDID.value = char_array;
	//	copied->MNAM.value = mnam_char;
	//	copied->ESPED = movt->ESPED;
	//	copied->INAM = movt->INAM;

	//	result->IsChanged(true);
	//	result->IsLoaded(true);
	//	FormIDMasterUpdater checker(skyrimMod->FormIDHandler);
	//	checker.Accept(result->formID);
	//}

	//map<FORMID, FORMID> retargeted_sndr;

	//for (auto& sndr_it : sndrs) {
	//	auto sndr = sndr_it.second;
	//	std::string new_edid;
	//	if (lower_find(sndr->EDID.value, old_name) != string::npos)
	//	{
	//		new_edid = replace_all(sndr->EDID.value, old_name, output_havok_project_name);
	//	}
	//	else {
	//		new_edid = output_havok_project_name + sndr->EDID.value;
	//	}
	//	if (!ends_with(new_edid, "SD"))
	//		new_edid += "SD";

	//	// declaring character array 
	//	char* char_array = new char[new_edid.size() + 1];

	//	// copying the contents of the 
	//	// string to char array 
	//	strcpy(char_array, new_edid.c_str());

	//	Record* to_copy = static_cast<Record*>(sndr);
	//	Record* result = skyrimCollection.CopyRecord(to_copy, skyrimMod, NULL, NULL, char_array, 0);
	//	Sk::SNDRRecord* copied = dynamic_cast<Sk::SNDRRecord*>(result);
	//	copied->EDID.value = char_array;
	//	copied->CNAM = sndr->CNAM;
	//	copied->GNAM = sndr->GNAM;
	//	copied->SNAM = sndr->SNAM;
	//	copied->FNAM = sndr->FNAM;
	//	copied->ANAM = sndr->ANAM;
	//	if (sndr->ONAM.value != NULL)
	//		copied->ONAM = sndr->ONAM;
	//	copied->CTDA = sndr->CTDA;
	//	copied->LNAM = sndr->LNAM;
	//	copied->BNAM = sndr->BNAM;

	//	result->IsChanged(true);
	//	result->IsLoaded(true);
	//	FormIDMasterUpdater checker(skyrimMod->FormIDHandler);
	//	checker.Accept(result->formID);

	//	retargeted_sndr[to_copy->formID] = result->formID;

	//}

	//for (auto& soun_it : souns) {
	//	auto soun = soun_it.second;
	//	std::string new_edid;
	//	if (lower_find(soun->EDID.value, old_name) != string::npos)
	//	{
	//		new_edid = replace_all(soun->EDID.value, old_name, output_havok_project_name);
	//	}
	//	else {
	//		new_edid = output_havok_project_name + soun->EDID.value;
	//	}

	//	// declaring character array 
	//	char* char_array = new char[new_edid.size() + 1];

	//	// copying the contents of the 
	//	// string to char array 
	//	strcpy(char_array, new_edid.c_str());

	//	Record* to_copy = static_cast<Record*>(soun);
	//	Record* result = skyrimCollection.CopyRecord(to_copy, skyrimMod, NULL, NULL, char_array, 0);
	//	Sk::SOUNRecord* copied = dynamic_cast<Sk::SOUNRecord*>(result);
	//	copied->EDID.value = char_array;
	//	copied->OBND = soun->OBND;
	//	copied->FNAM = soun->FNAM;
	//	copied->SNDD = soun->SNDD;
	//	copied->SDSC = soun->SDSC;

	//	copied->SDSC.value = retargeted_sndr[copied->SDSC.value];

	//	result->IsChanged(true);
	//	result->IsLoaded(true);
	//	FormIDMasterUpdater checker(skyrimMod->FormIDHandler);
	//	checker.Accept(result->formID);

	//}

	//for (auto& idle_it : idles) {
	//	auto idle = idle_it.second;
	//	std::string new_name = idle->EDID.value;
	//	std::string::size_type n = 0;

	//	if (lower_find(new_name,old_name) != std::string::npos)
	//	{
	//		new_name = replace_all(new_name, old_name, output_havok_project_name);
	//	}
	//	else {
	//		new_name = output_havok_project_name + new_name;
	//	}

	//	const int nn = new_name.length();

	//	// declaring character array 
	//	char* char_array = new char[nn + 1];

	//	// copying the contents of the 
	//	// string to char array 
	//	strcpy(char_array, new_name.c_str());

	//	Record* to_copy = static_cast<Record*>(idle);

	//	Record* result = skyrimCollection.CopyRecord(to_copy, skyrimMod, NULL, NULL, char_array, 0);
	//	result->IsChanged(true);
	//	Sk::IDLERecord* copied = dynamic_cast<Sk::IDLERecord*>(result);
	//	result->IsChanged(true);
	//	retargeted[idle->formID] = copied->formID;
	//	new_records.push_back(copied);
	//	copied->EDID.value = char_array;
	//	copied->ANAM = idle->ANAM;
	//	copied->CTDA = idle->CTDA;
	//	copied->DATA = idle->DATA;
	//	copied->ENAM = idle->ENAM;

	//	std::string temp = replace_all(idle->DNAM.value, old_name, output_havok_project_name);
	//	char* behavior_char = new char[temp.size() + 1];
	//	strcpy(behavior_char, temp.c_str());

	//	copied->DNAM.value = behavior_char;

	//	result->IsChanged(true);
	//	result->IsLoaded(true);
	//	FormIDMasterUpdater checker(skyrimMod->FormIDHandler);
	//	checker.Accept(result->formID);

	//}

	//for (int i = 0; i < new_records.size(); i++) {
	//	auto idle = new_records[i];
	//	Sk::IDLEANAM a = idle->ANAM.value;
	//	if (retargeted.find(idle->ANAM.value.parent) != retargeted.end())
	//		a.parent = retargeted[idle->ANAM.value.parent];
	//	if (retargeted.find(idle->ANAM.value.sibling) != retargeted.end())
	//		a.sibling = retargeted[idle->ANAM.value.sibling];
	//	idle->ANAM.value = a;
	//}

	//ModSaveFlags skSaveFlags = ModSaveFlags(2);
	//skSaveFlags.IsCleanMasters = true;
	//string esp_name = (output_havok_project_name + ".esp");
	//skyrimCollection.SaveMod(skyrimMod, skSaveFlags, (char* const)esp_name.c_str());

	//CloseHavok();
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