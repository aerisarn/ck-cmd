#pragma once

#include "StringListBlock.h"

namespace AnimData {
	class AttackDataBlock {

	public: 
		std::string getEventName() {
			return eventName;
		}

		void setEventName(std::string eventName) {
			this->eventName = eventName;
		}

		int getUnk1() {
			return unk1;
		}

		void setUnk1(int unk1) {
			this->unk1 = unk1;
		}

		StringListBlock getClips() {
			return clips;
		}

		void setClips(StringListBlock clips) {
			this->clips = clips;
		}

		void addClip(const std::string& clip) {
			this->clips.append(clip);
		}

		std::string eventName = "";
		int unk1 = 0;
		StringListBlock clips;

	};
}
