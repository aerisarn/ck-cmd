#pragma once

#include "StringListBlock.h"
#include <array>

namespace AnimData {

	struct root_movement_t {
		float duration;
		std::map<float, std::array<float, 3>> translations;
		std::map<float, std::array<float, 4>> rotations;
	};

	class ClipMovementData : public BlockObject {

		int cacheIndex = 0;
		std::string duration = "0";
		StringListBlock traslations;
		StringListBlock rotations;

		void addTranslation(root_movement_t& data, const std::string& line) {
			float time, x, y, z;
			sscanf(line.c_str(), "%f %f %f %f\n", &time, &x, &y, &z);
			data.translations.insert({ time, { x, y, z } });
		}

		void addRotation(root_movement_t& data, const std::string& line) {
			float time, x, y, z, w;
			sscanf(line.c_str(), "%f %f %f %f %f\n", &time, &x, &y, &z, &w);
			data.rotations.insert({ time, { x, y, z, w} });
		}

	public: 

		ClipMovementData() {}

		ClipMovementData(root_movement_t data)
		{
			duration = std::to_string(data.duration);
			for (const auto& translation : data.translations)
			{
				char temp[260] = { 0 };
				sprintf(temp, "%s %s %s %s", 
					tes_float_cache_to_string(translation.first).c_str(),
					tes_float_cache_to_string(translation.second[0]).c_str(),
					tes_float_cache_to_string(translation.second[1]).c_str(),
					tes_float_cache_to_string(translation.second[2]).c_str()
				);
				traslations.append(temp);
			}
			for (const auto& rotation : data.rotations)
			{
				char temp[260] = { 0 };
				sprintf(temp, "%s %s %s %s %s", 
					tes_float_cache_to_string(rotation.first).c_str(), 
					tes_float_cache_to_string(rotation.second[0]).c_str(),
					tes_float_cache_to_string(rotation.second[1]).c_str(),
					tes_float_cache_to_string(rotation.second[2]).c_str(),
					tes_float_cache_to_string(rotation.second[3]).c_str()
				);
				rotations.append(temp);
			}
		}

		static std::string tes_float_cache_to_string(const float& t)
		{
			std::string str{ std::to_string(t) };
			int offset{ 1 };
			if (str.find_last_not_of('0') == str.find('.')) { offset = 0; }
			str.erase(str.find_last_not_of('0') + offset, std::string::npos);
			return str;
		}


		void parseBlock(scannerpp::Scanner& input) override {
			cacheIndex = input.nextInt();
			duration = input.nextLine();
			traslations.fromASCII(input);
			rotations.fromASCII(input);
		}

		std::string getBlock() override {
			std::string out = std::to_string(cacheIndex) + "\n";
			out += duration + "\n";
			out += traslations.toASCII();
			out += rotations.toASCII() + "\n";
			return out;
		}

		int getCacheIndex() const {
			return cacheIndex;
		}

		void setCacheIndex(int cacheIndex) {
			this->cacheIndex = cacheIndex;
		}

		std::string getDuration() {
			return duration;
		}

		void setDuration(std::string duration) {
			this->duration = duration;
		}

		StringListBlock getTraslations() {
			return traslations;
		}

		void setTraslations(StringListBlock traslations) {
			this->traslations = traslations;
		}

		StringListBlock getRotations() {
			return rotations;
		}

		void setRotations(StringListBlock rotations) {
			this->rotations = rotations;
		}

		root_movement_t getMovement()
		{
			root_movement_t out;
			out.duration = std::stof(duration);
			for (size_t s = 0; s < traslations.size(); s++)
			{
				addTranslation(out, traslations[s]);
			}
			for (size_t s = 0; s < rotations.size(); s++)
			{
				addRotation(out, rotations[s]);
			}
			return out;
		}
	};
}
