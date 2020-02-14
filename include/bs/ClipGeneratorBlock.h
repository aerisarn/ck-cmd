#pragma once

#include "StringListBlock.h"

namespace AnimData {
	class ClipGeneratorBlock : public BlockObject {

		std::string name = "";
		int cacheIndex = 0;
		std::string playbackSpeed = "0";
		std::string unknown = "0";
		std::string unknown2 = "0";
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

		std::string getUnknown() {
			return unknown;
		}

		void setUnknown(std::string unknown) {
			this->unknown = unknown;
		}

		std::string getUnknown2() {
			return unknown2;
		}

		void setUnknown2(std::string unknown2) {
			this->unknown2 = unknown2;
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
			out += unknown + "\n";
			out += unknown2 + "\n";
			out += events.toASCII() + "\n";
			return out;
		}

		void parseBlock(scannerpp::Scanner& input) override {
			name = input.nextLine();
			cacheIndex = input.nextInt();
			playbackSpeed = input.nextLine();
			unknown = input.nextLine();
			unknown2 = input.nextLine();
			events.fromASCII(input);
		}
	};
}
