#pragma once

#include "ProjectAttackListBlock.h"
#include "StringListBlock.h"


namespace AnimData {
	class AnimSetDataFile {

		StringListBlock projectsList;
		std::vector<ProjectAttackListBlock> projectAttacks;
	public:
		StringListBlock getProjectsList() { return projectsList; }
		std::vector<ProjectAttackListBlock> getProjectAttackList() { return projectAttacks; }

		ProjectAttackListBlock& getProjectAttackBlock(int i) {
			return projectAttacks[i];
		}

		void parse(std::string content) {
			scannerpp::Scanner input(content);
			projectsList.fromASCII(input);
			while (input.hasNextLine()) {
				ProjectAttackListBlock pa;
				pa.parseBlock(input);
				projectAttacks.push_back(pa);
			}
		}

		std::string toString() {
			try {
				std::string out = projectsList.toASCII();
				for (ProjectAttackListBlock b : projectAttacks)
					out += b.getBlock();
				return out;
			}
			catch (...) {

			}
			return "";

		}
	};
}
