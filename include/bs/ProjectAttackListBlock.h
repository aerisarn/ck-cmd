#pragma once

#include "ProjectAttackBlock.h"

namespace AnimData {
	class ProjectAttackListBlock : public BlockObject {

		StringListBlock projectFiles;
		std::list<ProjectAttackBlock> projectAttackBlocks;

	public: 
		StringListBlock getProjectFiles() {
			return projectFiles;
		}
		void setProjectFiles(StringListBlock projectFiles) {
			this->projectFiles = projectFiles;
		}
		std::list<ProjectAttackBlock> getProjectAttackBlocks() {
			return projectAttackBlocks;
		}
		void setProjectAttackBlocks(std::list<ProjectAttackBlock> projectAttackBlocks) {
			this->projectAttackBlocks = projectAttackBlocks;
		}

		void putProjectAttack(const string& project_file, ProjectAttackBlock& project_attack)
		{
			vector<string>& projects = projectFiles.getStrings();
			projects.push_back(project_file);
			projectAttackBlocks.push_back(project_attack);
			projectFiles.setStrings(projects);
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
