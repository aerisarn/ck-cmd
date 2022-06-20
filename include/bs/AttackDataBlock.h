#pragma once

#include "StringListBlock.h"

namespace AnimData {
	class AttackDataBlock {

	public: 
		std::string& getEventName() {
			return eventName;
		}

		void setEventName(std::string eventName) {
			this->eventName = eventName;
		}

		int getUnk1() {
			return mirrored;
		}

		void setUnk1(int unk1) {
			this->mirrored = unk1;
		}

		StringListBlock getClips() {
			return clips;
		}

		size_t getClipSize() {
			return clips.size();
		}

		std::string& getClip(int clip_index)
		{
			return clips[clip_index];
		}

		void setClips(StringListBlock clips) {
			this->clips = clips;
		}

		void addClip(const std::string& clip) {
			this->clips.append(clip);
		}

		void removeClip(int index) {
			this->clips.remove(index);
		}

		std::string eventName = "";
		int mirrored = 0;
		StringListBlock clips;

	};
}
