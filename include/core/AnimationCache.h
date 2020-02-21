#pragma once

#include "stdafx.h"

#include <core/log.h>
#include <core/bsa.h>
#include <bs/AnimDataFile.h>
#include <bs/AnimSetDataFile.h>
#include <filesystem>
#include <memory>
#include <algorithm>

namespace fs = std::experimental::filesystem;

/**
* Created by Bet on 10.03.2016.
*/
class HkCRC {
	//int crc_order = 32;
	//std::string crc_poly = "4C11DB7";
	//std::string initial_value = "000000";
	//std::string final_value = "000000";

	static int reflectByte(int c)
	{
		int outbyte = 0;
		int i = 0x01;
		int j;

		for (j = 0x80; j>0; j >>= 1)
		{
			int val = c & i;
			if (val > 0) outbyte |= j;
			i <<= 1;
		}
		return (outbyte);
	}

	static void reflect(int* crc, int bitnum, int startLSB)
	{
		int i, j, k, iw, jw, bit;

		for (k = 0; k + startLSB<bitnum - 1 - k; k++) {

			iw = 7 - ((k + startLSB) >> 3);
			jw = 1 << ((k + startLSB) & 7);
			i = 7 - ((bitnum - 1 - k) >> 3);
			j = 1 << ((bitnum - 1 - k) & 7);

			bit = crc[iw] & jw;
			if ((crc[i] & j)>0) crc[iw] |= jw;
			else crc[iw] &= (0xff - jw);
			if ((bit)>0) crc[i] |= j;
			else crc[i] &= (0xff - j);
		}
		//return(crc);
	}
public:
	
	static std::string compute(std::string input) {
		// computes crc value
		int i, j, k, bit, datalen, len, flag, counter, c, order, ch, actchar;
		std::string data, output;
		int* mask = new int[8];
		const char hexnum[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
		int polynom[] = { 0,0,0,0,4,193,29,183 };
		int init[] = { 0,0,0,0,0,0,0,0 };
		int crc[] = { 0,0,0,0,0,0,0,0,0 };
		int xor[] = { 0,0,0,0,0,0,0,0 };
		order = 32;

		// generate bit mask
		counter = order;
		for (i = 7; i>0; i--)
		{
			if (counter >= 8) mask[i] = 255;
			else mask[i] = (1 << counter) - 1;
			counter -= 8;
			if (counter<0) counter = 0;
		}
		data = input;
		datalen = data.length();
		len = 0;
		for (i = 0; i<datalen; i++)
		{
			c = (int)data.at(i);
			if (data.at(i) == '%')				// unescape byte by byte (%00 allowed)
			{
				if (i>datalen - 3)
					Log::Error("Invalid data sequence");

				try {
					ch = (int)data.at(++i);
					c = (int)data.at(++i);
					c = (c & 15) | ((ch & 15) << 4);
				}
				catch (...) {
					Log::Error("Invalid data sequence");
					return "failure";
				}

			}
			c = reflectByte(c);
			for (j = 0; j<8; j++)
			{
				bit = 0;
				if ((crc[7 - ((order - 1) >> 3)] & (1 << ((order - 1) & 7)))>0) bit = 1;
				if ((c & 0x80)>0) bit ^= 1;
				c <<= 1;
				for (k = 0; k<8; k++)		// rotate all (max.8) crc bytes
				{
					crc[k] = ((crc[k] << 1) | (crc[k + 1] >> 7)) & mask[k];
					if ((bit)>0) crc[k] ^= polynom[k];
				}
			}
			len++;
		}
		reflect(crc, order, 0);
		for (i = 0; i<8; i++) crc[i] ^= xor[i];
		output = "";
		flag = 0;
		for (i = 0; i<8; i++)
		{
			actchar = crc[i] >> 4;
			if (flag>0 || actchar>0)
			{
				output += hexnum[actchar];
				flag = 1;
			}

			actchar = crc[i] & 15;
			if (flag>0 || actchar>0 || i == 7)
			{
				output += hexnum[actchar];
				flag = 1;
			}
		}
		return output;
	}
};

struct CacheEntry
{
	string name;
	AnimData::ProjectBlock& block;
	AnimData::ProjectDataBlock& movements;

	CacheEntry() : block(AnimData::ProjectBlock()) , movements(AnimData::ProjectDataBlock()){}
	CacheEntry(const string& name, AnimData::ProjectBlock& block, AnimData::ProjectDataBlock& movements) : name(name), block(block), movements(movements) {}

	bool hasCache() { return block.getHasAnimationCache(); }

	virtual void none() {}
};

struct CreatureCacheEntry : public CacheEntry
{
	AnimData::ProjectAttackListBlock& sets;

	CreatureCacheEntry() : sets(AnimData::ProjectAttackListBlock()) {}
	CreatureCacheEntry(const string& name, AnimData::ProjectBlock& block, AnimData::ProjectDataBlock& movements, AnimData::ProjectAttackListBlock& sets) :
		CacheEntry(name, block, movements), sets(sets) {}
};

struct AnimationCache {

	static constexpr const char* blocks_regex = "meshes\\\\animationdata\\\\.[^\\]+txt";
	static constexpr const char* movements_blocks_regex = "meshes\\\\animationdata\\\\boundanims\\\\.[^\\]+txt";
	static constexpr const char* attack_blocks_regex = "meshes\\\\animationsetdata\\\\.+\\\\.+txt";

	vector<CreatureCacheEntry> creature_entries;
	vector<CacheEntry>		   misc_entries;

	AnimData::AnimDataFile animationData;
	AnimData::AnimSetDataFile animationSetData;

	map<string, CacheEntry*> projects_index;

	CacheEntry* find(const string& name) {
		if (projects_index.find(name)!= projects_index.end())
			return projects_index[name];
		return NULL;
	}

	AnimationCache(const fs::path& animationDataPath, const  fs::path&  animationSetDataPath) {
		string animationDataContent;
		{
			std::ifstream t(animationDataPath.string());
			animationDataContent = string((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
		}
		string animationSetDataContent;
		{
			std::ifstream t(animationSetDataPath.string());
			animationSetDataContent = string((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
		}
		build(animationDataContent, animationSetDataContent);
	}

	AnimationCache(const string&  animationDataContent, const string&  animationSetDataContent) {
		build(animationDataContent, animationSetDataContent);
	}

	CreatureCacheEntry* cloneCreature(const std::string& source_project, const std::string& destination_project)
	{
		CacheEntry* source = find(source_project);
		if (source == NULL) return NULL;
		CreatureCacheEntry* creature = dynamic_cast<CreatureCacheEntry*>(source);
		if (creature == NULL) return NULL;
		AnimData::ProjectBlock block = creature->block;
		AnimData::ProjectDataBlock movements = creature->movements;
		AnimData::ProjectAttackListBlock sets = creature->sets;

		auto index = animationData.putProject(destination_project+".txt", block, movements);
		auto creature_index = animationSetData.putProjectAttackBlock(destination_project+"Data\\"+ destination_project+".txt", sets);

		creature_entries.push_back(
			CreatureCacheEntry(
				destination_project,
				animationData.getProjectBlock(index),
				animationData.getprojectMovementBlock(index),
				animationSetData.getProjectAttackBlock(creature_index)
			)
		);
		rebuildIndex();
		return dynamic_cast<CreatureCacheEntry*>(find(destination_project));
	}

	void save(const fs::path& animationDataPath, const  fs::path&  animationSetDataPath) {
		std::ofstream outstream;
		outstream.open(animationDataPath.string()); // append instead of overwrite
		outstream << animationData.toString();
		outstream.close();
		outstream.open(animationSetDataPath.string());
		outstream << animationSetData.toString();
		outstream.close();
	}

	void save_creature(const string& project, CacheEntry* project_entry,  const fs::path& animationDataPath, const  fs::path&  animationSetDataPath) {
		fs::create_directories("animationdata");

		if (project_entry) {

			std::ofstream outstream;
			outstream.open(fs::path("animationdata") / string(project + ".txt"));
			outstream << project_entry->block.getBlock();
			outstream.close();
			outstream.open(fs::path("animationdata") / string("dirlist.txt"));
			outstream << animationData.getProjectList().getBlock();
			outstream.close();
			if (project_entry->hasCache())
			{
				fs::create_directories(fs::path("animationdata")/"boundanims");
				outstream.open(fs::path("animationdata") / "boundanims" / string("anims_" + project + ".txt"));
				outstream << project_entry->movements.getBlock();
				outstream.close();
			}
			auto creature_ptr = dynamic_cast<CreatureCacheEntry*>(project_entry);
			if (NULL != creature_ptr)
			{
				fs::path set_data_directory = fs::path("animationsetdata") / string(project + "data");
				fs::create_directories(set_data_directory);
				outstream.open(fs::path("animationsetdata") / string("dirlist.txt"));
				outstream << animationSetData.getProjectsList().getBlock();
				outstream.close();
				auto sets = creature_ptr->sets;
				auto projects = sets.getProjectFiles().getStrings();
				auto data = sets.getProjectAttackBlocks();
				for (int i = 0; i < projects.size(); i++)
				{
					string outfile = (set_data_directory / projects[i]).string();
					transform(outfile.begin(), outfile.end(), outfile.begin(), ::tolower);
					outstream.open(set_data_directory / projects[i]);
					outstream << data[i].getBlock();
					outstream.close();
				}
				string outfile = (set_data_directory / string(project + ".txt")).string();
				transform(outfile.begin(), outfile.end(), outfile.begin(), ::tolower);
				outstream.open(outfile);
				for (int i = 0; i < projects.size(); i++)
				{
					outstream << projects[i] << endl;
				}
				outstream << endl;
				outstream.close();
			}

			save(animationDataPath, animationSetDataPath);
		}


	}

	void rebuildIndex()
	{
		for (auto& entry : creature_entries)
		{
			string lower = entry.name;
			transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return tolower(c); });
			projects_index[lower] = &entry;
		}
		for (auto& entry : misc_entries)
		{
			string lower = entry.name;
			transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return tolower(c); });
			projects_index[lower] = &entry;
		}
	}
	
	void build(const string&  animationDataContent, const string&  animationSetDataContent) {

		animationData.parse(animationDataContent);
		animationSetData.parse(animationSetDataContent);

		int index = 0;
		for (string project : animationData.getProjectList().getStrings()) {
			string sanitized_project_name = fs::path(project).filename().replace_extension("").string();
			string sanitized_creature_name = sanitized_project_name + "Data\\" + sanitized_project_name + ".txt";
			int creature_index = animationSetData.getProjectAttackBlock(sanitized_creature_name);
			if (creature_index != -1)
			{
				creature_entries.push_back(
					CreatureCacheEntry(
						sanitized_project_name,
						animationData.getProjectBlock(index),
						animationData.getprojectMovementBlock(index),
						animationSetData.getProjectAttackBlock(creature_index)
					)
				);
				
			}
			else {
				misc_entries.push_back(
					CacheEntry(
						sanitized_project_name,
						animationData.getProjectBlock(index),
						animationData.getprojectMovementBlock(index)
					)
				);
			}
			index++;
		}
		rebuildIndex();

		printInfo();
#ifdef __TEST__
		AnimData::AnimDataFile newAnimationData;

		for (auto& entry : creature_entries)
		{
			newAnimationData.putProject(entry.name + ".txt", entry.block, entry.movements);
		}
		for (auto& entry : misc_entries)
		{
			if (entry.hasCache())
				newAnimationData.putProject(entry.name + ".txt", entry.block, entry.movements);
			else 
				newAnimationData.putProject(entry.name + ".txt", entry.block);
		}

		string new_content = newAnimationData.toString();
		std::string::size_type pos = 0;
		while ((pos = new_content.find("\n", pos)) != std::string::npos)
		{
			new_content.replace(pos, 1, "\r\n");
			pos = pos + 2;
		}
		if (new_content != animationDataContent)
			Log::Error("round trip error!");

		AnimData::AnimSetDataFile newAnimationSetData;
		for (auto& entry : creature_entries)
		{
			newAnimationSetData.putProjectAttackBlock(entry.name + "Data\\" + entry.name  + ".txt", entry.sets);
		}
		string new_set_content = newAnimationSetData.toString();
		 pos = 0;
		while ((pos = new_set_content.find("\n", pos)) != std::string::npos)
		{
			new_set_content.replace(pos, 1, "\r\n");
			pos = pos + 2;
		}
		if (new_set_content != animationSetDataContent)
			Log::Error("round trip error!");
#endif
	}

	size_t getNumCreatureProjects(){
		return creature_entries.size();
	}

	static void get_entries(
		CacheEntry& entry,
		CreatureCacheEntry& creature_entry,
		const string& cacheFile
	) {
		if (!fs::exists(cacheFile) || !fs::is_regular_file(cacheFile))
			return;

		fs::path name = cacheFile;
		name = name.filename().replace_extension("");
		entry.name = name.string();

		string block_content;
		ifstream t(cacheFile);
		t.seekg(0, std::ios::end);
		block_content.reserve(t.tellg());
		t.seekg(0, std::ios::beg);
		block_content.assign((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		scannerpp::Scanner p(block_content);
		entry.block.parseBlock(p);

		if (entry.block.getHasAnimationCache())
		{
			auto movement_path = fs::path(cacheFile).parent_path() / "boundanims" / string("anims_" + name.string() + ".txt");
			ifstream t(movement_path.string());
			string movement_content;
			t.seekg(0, std::ios::end);
			block_content.reserve(t.tellg());
			t.seekg(0, std::ios::beg);
			movement_content.assign((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
			scannerpp::Scanner p(movement_content);
			entry.movements.parseBlock(p);
		}
	}

	static void create_entry(
		CacheEntry& entry,
		const ckcmd::BSA::BSAFile& bsa_file,
		const string& name)
	{
		entry.name = name;

		string block_path = "meshes\\animationdata\\" + name + ".txt";
		string block_content = bsa_file.extract(block_path);
		scannerpp::Scanner p(block_content);
		entry.block.parseBlock(p);

		if (entry.block.getHasAnimationCache())
		{
			string movement_path = "meshes\\animationdata\\boundanims\\anims_" + name + ".txt";
			string movement_content = bsa_file.extract(movement_path);
			scannerpp::Scanner p(movement_content);
			entry.movements.parseBlock(p);

		}
	}

	static void create_creature_entry(
		CreatureCacheEntry& entry,
		const ckcmd::BSA::BSAFile& bsa_file, 
		const string& name)
	{
		create_entry(entry, bsa_file, name);

		string directory_path = "meshes\\animationsetdata\\" + entry.name + "Data\\"+ entry.name +".txt";
		AnimData::StringListBlock list; 
		string directory_content = bsa_file.extract(directory_path);
		scannerpp::Scanner p(directory_content);
		list.parseBlock(p);
		vector<string> project_files = list.getStrings();

		std::sort(project_files.begin(), project_files.end(),
			[](const string& lhs, const string& rhs) -> bool
		{
			string llhs = lhs.substr(0, lhs.size() - 4);
			string lrhs = rhs.substr(0, rhs.size() - 4);

			std::transform(llhs.begin(), llhs.end(), llhs.begin(), ::tolower);
			std::transform(lrhs.begin(), lrhs.end(), lrhs.begin(), ::tolower);

			std::string::size_type pos = 0;
			while ((pos = llhs.find("_", pos)) != std::string::npos)
			{
				llhs.replace(pos, 1, " ");
				pos = pos + 1;
			}
			pos = 0;
			while ((pos = lrhs.find("_", pos)) != std::string::npos)
			{
				lrhs.replace(pos, 1, " ");
				pos = pos + 1;
			}

			return lrhs > llhs;
		});

		for (const auto& project : project_files)
		{
			string sub_project_path = "meshes\\animationsetdata\\" + entry.name + "Data\\" + project;
			string sub_project_content = bsa_file.extract(sub_project_path);
			scannerpp::Scanner p(sub_project_content);
			AnimData::ProjectAttackBlock ab; ab.parseBlock(p);
			entry.sets.putProjectAttack(project, ab);
		}
	}

	bool iequals(const string& a, const string& b)
	{
		return std::equal(a.begin(), a.end(),
			b.begin(), b.end(),
			[](char a, char b) {
			return tolower(a) == tolower(b);
		});
	}

	void check_from_bsa(const ckcmd::BSA::BSAFile& bsa_file, const std::vector<string>& actors, const std::vector<string>& misc)
	{

		for (int i = 0; i < actors.size(); i++)
		{
			CreatureCacheEntry entry;
			CreatureCacheEntry& default_entry = creature_entries[i];
			create_creature_entry(entry, bsa_file, fs::path(actors[i]).filename().replace_extension("").string());
			if (!iequals(entry.name, default_entry.name) ||
				entry.block.getBlock() != default_entry.block.getBlock() ||
				entry.movements.getBlock() != default_entry.movements.getBlock() ||
				entry.sets.getBlock() != default_entry.sets.getBlock())
				Log::Info("Error");


			Log::Info("project creature: %s", entry.name.c_str());
			int getUnkEventList = 0;
			int getUnkEventData = 0;
			int crc32s = 0;
			size_t movements = entry.movements.getMovementData().size();
			set<string> paths;
			set<string> attacks;
			auto abs = entry.sets.getProjectAttackBlocks();
			Log::Info("animations sets: %d", abs.size());
			for (auto& ab : abs)
			{
				getUnkEventList += ab.getUnkEventList().getStrings().size();
				getUnkEventData += ab.getUnkEventData().getStrings().size();
				auto& atts = ab.getAttackData().getAttackData();
				auto& strings = ab.getCrc32Data().getStrings();
				for (auto & att : atts)
					attacks.insert(att.getEventName());
				std::list<std::string>::iterator it;
				int i = 0;
				string this_path;
				for (it = strings.begin(); it != strings.end(); ++it) {
					if (i % 3 == 0)
						this_path = *it;
					if (i % 3 == 1)
						paths.insert(this_path + *it);
					i++;
				}
			}
			Log::Info("attacks: %d", attacks.size());
			Log::Info("getUnkEventList: %d", getUnkEventList);
			Log::Info("getUnkEventData: %d", getUnkEventData);
			Log::Info("movements: %d", movements);
			Log::Info("paths: %d", paths.size());

		}
		for (int i = 0; i < misc.size(); i++)
		{
			CacheEntry entry;
			CacheEntry& default_entry = misc_entries[i];
			create_entry(entry, bsa_file, fs::path(misc[i]).filename().replace_extension("").string());
			if (!iequals(entry.name, default_entry.name) ||
				entry.block.getBlock() != default_entry.block.getBlock() ||
				entry.movements.getBlock() != default_entry.movements.getBlock())
				Log::Info("Error");
		}

	}

	void printInfo() {
		Log::Info("Parsed correctly %d havok projects", creature_entries.size() + misc_entries.size());
		Log::Info("Found %d creatures projects:", getNumCreatureProjects());
		//checkInfo();
	}

	//void checkInfo() {
	//	for (const auto& pair : set_data_map) {
	//		Log::Info("\tID:%d\tName: %s", pair.second, pair.first.c_str());

	//		int data_index = data_map[pair.first];
	//		int set_data_index = set_data_map[pair.first];

	//		AnimData::ProjectBlock& this_data = animationData.getProjectBlock(data_index);
	//		AnimData::ProjectDataBlock& this_movement_data = animationData.getprojectMovementBlock(data_index);
	//		AnimData::ProjectAttackListBlock& this_set_data = animationSetData.getProjectAttackBlock(set_data_index);
	//		Log::Info("\t\tHavok Files:%d", this_data.getProjectFiles().getStrings().size());
	//		list<string> projects = this_data.getProjectFiles().getStrings();
	//		for (auto& havok_file : projects)
	//			Log::Info("\t\t\t%s", havok_file.c_str());

	//		//Check CRC clips number
	//		size_t movements = this_movement_data.getMovementData().size();
	//		set<string> paths;
	//		
	//		for (auto& block : this_set_data.getProjectAttackBlocks())
	//		{
	//			auto& strings = block.getCrc32Data().getStrings();
	//			std::list<std::string>::iterator it;
	//			int i = 0;
	//			string this_path;
	//			for (it = strings.begin(); it != strings.end(); ++it) {
	//				if (i % 3 == 0)
	//					this_path = *it;
	//				if (i % 3 == 1)
	//					paths.insert(this_path + *it);
	//				i++;
	//			}
	//		}
	//		size_t crcs = paths.size();
	//		if (movements != crcs)
	//			Log::Info("Warning: unaddressed movement data!");

	//	}
	//}


	//bool hasCreatureProject (const string& project_name) {
	//	return set_data_map.find(project_name) != set_data_map.end();
	//}

};