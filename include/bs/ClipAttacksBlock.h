#pragma once

#include "QuadBlock.h"
#include "AttackDataBlock.h"
#include <list>

namespace AnimData {
	class ClipAttacksBlock : public QuadBlock {

		int blocks = 0;
	public:
		std::list<AttackDataBlock> attackData; // = new ArrayList<>();

		int getBlocks() {
			return blocks;
		}

		void setBlocks(int blocks) {
			this->blocks = blocks;
		}

		std::list<AttackDataBlock> getAttackData() {
			return attackData;
		}

		void setAttackData(std::list<AttackDataBlock> attackData) {
			this->attackData = attackData;
		}

		void parseBlock(scannerpp::Scanner& input) override {
			for (int i = 0; i < blocks; i++) {
				AttackDataBlock ad; // = new AttackDataBlock();
				ad.eventName = input.nextLine();
				ad.unk1 = input.nextInt();// input.nextLine();
				ad.clips.fromASCII(input);
				attackData.push_back(ad);
			}
		}

		std::string getBlock() override {
			if (attackData.empty()) return "";
			std::string out = "";
			for (AttackDataBlock ad : attackData) {
				out += ad.eventName + "\n";
				out += ad.unk1 + "\n";
				out += ad.clips.toASCII();
			}
			return out;
		}

	};
}
