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
		StringListBlock getSwapEventsList() const {
			return swapEventsList;
		}

		size_t getSwapEventsListSize() {
			return swapEventsList.size();
		}

		std::string& getSwapEvent(int index) {
			return swapEventsList[index];
		}

		void addSwapEvent(const std::string& string) {
			swapEventsList.append(string);
		}

		void removeSwapEvent(int index) {
			swapEventsList.remove(index);
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

		ClipAttacksBlock getAttackData() const {
			return attackData;
		}

		void setAttackData(ClipAttacksBlock attackData) {
			this->attackData = attackData;
		}

		size_t getAttacks()
		{
			return attackData.getAttacks();
		}

		void addAttack(const std::string& attack_event)
		{
			attackData.addAttack(attack_event);
		}

		void removeAttack(int index)
		{
			attackData.removeAttack(index);
		}

		std::string& getAttackEvent(int attack_index)
		{
			return this->attackData.getAttackEvent(attack_index);
		}

		void setAttackEvent(int attack_index, const std::string& attack_event)
		{
			this->attackData.setAttackEvent(attack_index, attack_event);
		}

		size_t getAttackClips(int attack_index)
		{
			return this->attackData.getAttackClips(attack_index);
		}

		std::string& getAttackClip(int attack_index, int clip_index)
		{
			return this->attackData.getAttackClip(attack_index, clip_index);
		}

		void addAttackClip(int attack_index, const std::string& clip_generator_name)
		{
			attackData.addAttackClip(attack_index, clip_generator_name);
		}

		void removeAttackClip(int attack_index, int clip_index)
		{
			attackData.removeAttackClip(attack_index, clip_index);
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
