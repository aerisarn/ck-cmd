#pragma once

#include "stdafx.h"

#include <core/log.h>
#include <bs/AnimDataFile.h>
#include <bs/AnimSetDataFile.h>
#include <filesystem>

namespace fs = std::experimental::filesystem;

struct AnimationCache {
	AnimData::AnimDataFile animationData;
	AnimData::AnimSetDataFile animationSetData;

	map<string, int> data_map;
	map<string, int> set_data_map;

	AnimationCache(const string&  animationDataContent, const string&  animationSetDataContent) {
		animationData.parse(animationDataContent);
		animationSetData.parse(animationSetDataContent);
		int index = 0;
		for (string creature : animationSetData.getProjectsList().getStrings()) {
			string creature_project_name = fs::path(creature).filename().replace_extension("").string();
			//if (creature_project_name.find("Project") != string::npos)
			//	creature_project_name = creature_project_name.substr(0, creature_project_name.find("Project"));
			set_data_map[creature_project_name] = index++;
		}
		index = 0;
		for (string misc : animationData.getProjectList().getStrings()) {
			string misc_project_name = fs::path(misc).filename().replace_extension("").string();
			data_map[misc_project_name] = index++;
		}
		for (const auto& pair : set_data_map) {
			if (data_map.find(pair.first) == data_map.end())
				Log::Error("Unpaired set data: %s", pair.first);
		}
	}

	void printInfo() {
		Log::Info("Parsed correctly %d havok projects", data_map.size());
		Log::Info("Found %d creatures projects:", set_data_map.size());
		for (const auto& pair : set_data_map) {
			Log::Info("\tID:%d\tName: %s", pair.second, pair.first.c_str());
		}
	}
};