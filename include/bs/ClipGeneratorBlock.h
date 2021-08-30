#pragma once

#include "StringListBlock.h"

namespace AnimData {
	class ClipGeneratorBlock : public BlockObject {

		std::string name = "";
		int cacheIndex = 0;
		std::string playbackSpeed = "0";
		std::string cropStartTime = "0";
		std::string cropEndTime = "0";
		StringListBlock events; // = new StringListBlock();
	public:
		
		std::string getName() const {
			return name;
		}

		void setName(std::string name) {
			this->name = name;
		}

		int getCacheIndex() {
			return cacheIndex;
		}
		
		void setCacheIndex(int cacheIndex) {
			this->cacheIndex = cacheIndex;
		}

		std::string getPlaybackSpeed() {
			return playbackSpeed;
		}

		void setPlaybackSpeed(std::string playbackSpeed) {
			this->playbackSpeed = playbackSpeed;
		}

		std::string getCropStartTime() {
			return cropStartTime;
		}

		void setCropStartTime(std::string cropStartTime) {
			this->cropStartTime = cropStartTime;
		}

		std::string getCropEndTime() {
			return cropEndTime;
		}

		void setCropEndTime(std::string unknown2) {
			this->cropEndTime = cropEndTime;
		}

		StringListBlock getEvents() {
			return events;
		}

		void setEvents(StringListBlock events) {
			this->events = events;
		}

		std::string getBlock() override {
			std::string out = name + "\n";
			out += std::to_string(cacheIndex) + "\n";
			out += playbackSpeed + "\n";
			out += cropStartTime + "\n";
			out += cropEndTime + "\n";
			out += events.toASCII() + "\n";
			return out;
		}

		void parseBlock(scannerpp::Scanner& input) override {
			name = input.nextLine();
			cacheIndex = input.nextInt();
			playbackSpeed = input.nextLine();
			cropStartTime = input.nextLine();
			cropEndTime = input.nextLine();
			events.fromASCII(input);
		}
	};
}
