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
				char temp[260];
				sprintf(temp, "%f %f %f %f", translation.first, translation.second[0], translation.second[1], translation.second[2]);
				traslations.append(temp);
			}
			for (const auto& rotation : data.rotations)
			{
				char temp[260];
				sprintf(temp, "%f %f %f %f %f", rotation.first, rotation.second[0], rotation.second[1], rotation.second[2], rotation.second[3]);
				rotations.append(temp);
			}
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
