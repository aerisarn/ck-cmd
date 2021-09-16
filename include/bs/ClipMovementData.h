#pragma once

#include "StringListBlock.h"

namespace AnimData {

	class ClipMovementData : public BlockObject {

		int cacheIndex = 0;
		std::string duration = "0";
		StringListBlock traslations;
		StringListBlock rotations;

	public: 
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
	};
}
