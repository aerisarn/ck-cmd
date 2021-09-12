#pragma once

#include "ClipAttacksBlock.h"
#include "ClipFilesCRC32Block.h"
#include "UnkEventData.h"

namespace AnimData {

	class ProjectAttackBlock : public BlockObject {

		std::string animVersion = "V3";
		//behavior data, check how!
		StringListBlock swapEventsList; //Used only when multiple sets of animations can swap istantly, list of events that can change the set
		//behavior data, check iLeftHandType / iRightHandType values!
		UnkEventData unkEventData; //in case of swap of equipped stuff, sets new values to the variable used by the attack set
		ClipAttacksBlock attackData;
		ClipFilesCRC32Block crc32Data;
	public:
		StringListBlock getUnkEventList() {
			return swapEventsList;
		}

		void setUnkEventList(StringListBlock unkEventList) {
			this->swapEventsList = unkEventList;
		}

		UnkEventData getUnkEventData() {
			return unkEventData;
		}

		void setUnkEventData(UnkEventData unkEventData) {
			this->unkEventData = unkEventData;
		}

		ClipAttacksBlock getAttackData() {
			return attackData;
		}

		void setAttackData(ClipAttacksBlock attackData) {
			this->attackData = attackData;
		}
		
		ClipFilesCRC32Block& getCrc32Data() {
			return crc32Data;
		}
		
		void setCrc32Data(ClipFilesCRC32Block crc32Data) {
			this->crc32Data = crc32Data;
		}


		void parseBlock(scannerpp::Scanner& input) override {
			animVersion = input.nextLine();
			swapEventsList.fromASCII(input);
			unkEventData.fromASCII(input);
			int numAttackBlocks = input.nextInt();
			attackData.setBlocks(numAttackBlocks);
			attackData.parseBlock(input);
			crc32Data.fromASCII(input);
		}
		
		std::string getBlock() override {
			std::string out = animVersion + "\n";
			out += swapEventsList.toASCII();
			out += unkEventData.toASCII();
			out += std::to_string(attackData.getBlocks()) + "\n";
			out += attackData.getBlock();
			out += crc32Data.toASCII();
			return out;
		}
	};
}
