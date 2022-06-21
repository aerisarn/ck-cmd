#pragma once

#include "ProjectAttackBlock.h"

namespace AnimData {
	class ProjectAttackListBlock : public BlockObject {

		StringListBlock projectFiles;
		std::vector<ProjectAttackBlock> projectAttackBlocks;

	public: 
		void clear() {
			projectFiles.clear();
			projectAttackBlocks.clear();
		}
		StringListBlock getProjectFiles() {
			return projectFiles;
		}
		void setProjectFiles(StringListBlock projectFiles) {
			this->projectFiles = projectFiles;
		}
		void addProjectFile(const std::string& projectFile) {
			this->projectFiles.append(projectFile);
		}
		void removeProjectFile(int i) {
			this->projectFiles.remove(i);
		}
		std::vector<ProjectAttackBlock>& getProjectAttackBlocks() {
			return projectAttackBlocks;
		}
		void setProjectAttackBlocks(const std::vector<ProjectAttackBlock>& projectAttackBlocks) {
			this->projectAttackBlocks = projectAttackBlocks;
		}

		void putProjectAttack(const string& project_file, ProjectAttackBlock& project_attack)
		{
			//vector<string>& projects = projectFiles.getStrings();
			//projects.push_back(project_file);
			projectAttackBlocks.push_back(project_attack);
			projectFiles.append(project_file);
			//projectFiles.setStrings(projects);
		}

		void removeProjectAttack(int index)
		{
			projectFiles.remove(index);
			projectAttackBlocks.erase(projectAttackBlocks.begin() + index);
		}

		int getBlockEvents(int set_index)
		{
			return projectAttackBlocks.at(set_index).getSwapEventsListSize();
		}

		const std::string& getBlockEvent(int set_index, int event_index)
		{
			return projectAttackBlocks.at(set_index).getSwapEvent(event_index);
		}

		void addBlockEvent(int set_index, const std::string& event)
		{
			projectAttackBlocks.at(set_index).addSwapEvent(event);
		}

		void removeBlockEvent(int set_index, int event_index)
		{
			projectAttackBlocks.at(set_index).removeSwapEvent(event_index);
		}

		int getBlockVariables(int set_index)
		{
			return projectAttackBlocks.at(set_index).getHandVariableData().getVariables().size();
		}

		std::string getBlockVariable(int set_index, int variable_index)
		{
			return projectAttackBlocks.at(set_index).getHandVariableData().getVariables().at(variable_index).variable_name;
		}

		int getBlockVariableMin(int set_index, int variable_index)
		{
			return projectAttackBlocks.at(set_index).getHandVariableData().getVariables().at(variable_index).value_min;
		}

		int getBlockVariableMax(int set_index, int variable_index)
		{
			return projectAttackBlocks.at(set_index).getHandVariableData().getVariables().at(variable_index).value_max;
		}

		void setBlockVariableMin(int set_index, int variable_index, int value)
		{
			projectAttackBlocks.at(set_index).getHandVariableData().getVariables()[variable_index].value_min = value;
		}

		void setBlockVariableMax(int set_index, int variable_index, int value)
		{
			projectAttackBlocks.at(set_index).getHandVariableData().getVariables()[variable_index].value_max = value;
		}

		void addBlockVariable(int set_index, const std::string& name, int min, int max)
		{
			projectAttackBlocks.at(set_index).getHandVariableData().getVariables().push_back({ name, min, max });
		}

		void removeBlockVariable(int set_index, int variable_index)
		{
			projectAttackBlocks[set_index].getHandVariableData().getVariables().erase(
				projectAttackBlocks.at(set_index).getHandVariableData().getVariables().begin() + variable_index
			);
		}

		size_t getBlockAttacks(int set_index)
		{
			return projectAttackBlocks.at(set_index).getAttacks();
		}

		std::string& getBlockAttackEvent(int set_index, int attack_index)
		{
			return projectAttackBlocks.at(set_index).getAttackEvent(attack_index);
		}

		void setBlockAttackEvent(int set_index, int attack_index, const std::string& attack_event)
		{
			projectAttackBlocks.at(set_index).setAttackEvent(attack_index, attack_event);
		}

		size_t getBlockAttackClips(int set_index, int attack_index)
		{
			return projectAttackBlocks.at(set_index).getAttackClips(attack_index);
		}

		const std::string& getBlockAttackClip(int set_index, int attack_index, int clip_index)
		{
			return projectAttackBlocks.at(set_index).getAttackClip(attack_index, clip_index);
		}

		void addBlockAttackClip(int set_index, int attack_index, const std::string& clip_generator_name)
		{
			projectAttackBlocks.at(set_index).addAttackClip(attack_index, clip_generator_name);
		}

		void removeBlockAttackClip(int set_index, int attack_index, int clip_generator_index)
		{
			projectAttackBlocks.at(set_index).removeAttackClip(attack_index, clip_generator_index);
		}

		void addBlockAttack(int set_index, const std::string& attack_event)
		{
			projectAttackBlocks[set_index].addAttack(attack_event);
		}

		void removeBlockAttack(int set_index, int attack_index)
		{
			projectAttackBlocks[set_index].removeAttack(attack_index);
		}

		void parseBlock(scannerpp::Scanner& input) override {
			projectFiles.fromASCII(input);
			for (std::string p : projectFiles.getStrings()) {
				ProjectAttackBlock pb;
				pb.parseBlock(input);
				projectAttackBlocks.push_back(pb);
			}
		}

		std::string getBlock() override {
			std::string out = projectFiles.toASCII();
			for (ProjectAttackBlock p : projectAttackBlocks) {
				out += p.getBlock();
			}
			return out;
		}
	};
}
