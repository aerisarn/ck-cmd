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

		int getProjectAttackBlock(const string& name) {
			const std::vector<std::string>& projects = projectsList.getStrings();
			for (size_t i = 0; i < projects.size(); i++)
			{
				if (projects[i] == name)
					return i;
			}
			return -1;
		}

		size_t putProjectAttackBlock(const string& name, const ProjectAttackListBlock& block)
		{
			std::vector<std::string>& projects = projectsList.getStrings();
			projects.push_back(name);
			projectsList.setStrings(projects);
			projectAttacks.push_back(block);
			return projectAttacks.size() - 1;
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
