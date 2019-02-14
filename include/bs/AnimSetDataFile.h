#pragma once

#include "ProjectAttackListBlock.h"
#include "StringListBlock.h"


namespace AnimData {
	class AnimSetDataFile {

		StringListBlock projectsList; // = new StringListBlock();
		std::vector<ProjectAttackListBlock> projectAttacks; // = new ArrayList<>();
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
				ProjectAttackListBlock pa; // = new ProjectAttackListBlock();
				pa.parseBlock(input);
				projectAttacks.push_back(pa);
			}
			//System.out.println("Parsed " + projectsList.getStrings().size() + " projects");
			//System.out.println("and " + projectAttacks.size() + " Attacks Data");
		}

		std::string toString() {
			try {
				std::string out = projectsList.toASCII();
				for (ProjectAttackListBlock b : projectAttacks)
					out += b.getBlock();
				return out;
			}
			catch (...) {
				//e.printStackTrace();
			}
			return "";

		}
	};
}
