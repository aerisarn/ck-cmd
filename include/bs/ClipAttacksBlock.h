#pragma once

#include "QuadBlock.h"
#include "AttackDataBlock.h"
#include <list>

namespace AnimData {
	class ClipAttacksBlock : public QuadBlock {

		int blocks = 0;
	public:
		std::vector<AttackDataBlock> attackData;

		int getBlocks() {
			return blocks;
		}

		void setBlocks(int blocks) {
			this->blocks = blocks;
		}

		std::vector<AttackDataBlock> getAttackData() {
			return attackData;
		}

		size_t getAttacks()
		{
			return attackData.size();
		}

		void addAttack(const std::string& attack_event)
		{
			AttackDataBlock block;
			block.eventName = attack_event;
			attackData.push_back(block);
			blocks = attackData.size();
		}

		void removeAttack(int index)
		{
			attackData.erase(attackData.begin() + index);
			blocks = attackData.size();
		}

		std::string& getAttackEvent(int attack_index)
		{
			return attackData.at(attack_index).getEventName();
		}

		void setAttackEvent(int attack_index, const std::string& attack_event)
		{
			attackData[attack_index].eventName = attack_event;
		}

		size_t getAttackClips(int attack_index)
		{
			return attackData.at(attack_index).getClipSize();
		}

		std::string& getAttackClip(int attack_index, int clip_index)
		{
			return attackData.at(attack_index).getClip(clip_index);
		}

		void addAttackClip(int attack_index, const std::string& clip_generator_name)
		{
			attackData[attack_index].addClip(clip_generator_name);
		}

		void removeAttackClip(int attack_index, int clip_index)
		{
			attackData[attack_index].removeClip(clip_index);
		}

		void setAttackData(std::vector<AttackDataBlock> attackData) {
			this->blocks = attackData.size();
			this->attackData = attackData;
		}

		void parseBlock(scannerpp::Scanner& input) override {
			for (int i = 0; i < blocks; i++) {
				AttackDataBlock ad;
				ad.eventName = input.nextLine();
				ad.mirrored = input.nextInt();
				ad.clips.fromASCII(input);
				attackData.push_back(ad);
			}
		}

		std::string getBlock() override {
			if (attackData.empty()) return "";
			std::string out = "";
			for (AttackDataBlock ad : attackData) {
				out += ad.eventName + "\n";
				out += std::to_string(ad.mirrored) + "\n";
				out += ad.clips.toASCII();
			}
			return out;
		}

	};
}
