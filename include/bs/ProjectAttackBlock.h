#pragma once

#include "ClipAttacksBlock.h"
#include "ClipFilesCRC32Block.h"
#include "HandVariableData.h"

namespace AnimData {

	class ProjectAttackBlock : public BlockObject {

		std::string animVersion = "V3";
		//behavior data, check how!
		StringListBlock swapEventsList; //Used only when multiple sets of animations can swap istantly, list of events that can change the set
		//behavior data, check iLeftHandType / iRightHandType values!
		HandVariableData handVariableData; //in case of swap of equipped stuff, sets new values to the variable used by the attack set
		ClipAttacksBlock attackData;
		ClipFilesCRC32Block crc32Data;
	public:
		StringListBlock getSwapEventsList() {
			return swapEventsList;
		}

		void setSwapEventsList(StringListBlock swapEventsList) {
			this->swapEventsList = swapEventsList;
		}

		HandVariableData& getHandVariableData() {
			return handVariableData;
		}

		void setHandVariableData(HandVariableData handVariableData) {
			this->handVariableData = handVariableData;
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
			handVariableData.parseBlock(input);
			int numAttackBlocks = input.nextInt();
			attackData.setBlocks(numAttackBlocks);
			attackData.parseBlock(input);
			crc32Data.fromASCII(input);
		}
		
		std::string getBlock() override {
			std::string out = animVersion + "\n";
			out += swapEventsList.toASCII();
			out += handVariableData.getBlock();
			out += std::to_string(attackData.getBlocks()) + "\n";
			out += attackData.getBlock();
			out += crc32Data.toASCII();
			return out;
		}
	};
}
