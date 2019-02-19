#pragma once

#include "ProjectBlock.h"
#include "ProjectDataBlock.h"
#include <map>

namespace AnimData {
	class AnimDataFile {

		StringListBlock projectsList; // = new StringListBlock();
		std::vector<ProjectBlock> projectBlockList; // = new ArrayList<ProjectBlock>();
		std::map<int, ProjectDataBlock> projectMovementBlockList; // = new HashMap<>();

	public: 
		StringListBlock getProjectList() {
			return projectsList;
		}

		std::vector<ProjectBlock> getProjectBlockList() {
			return projectBlockList;
		}

		ProjectBlock& getProjectBlock(int i) {
			return projectBlockList[i];
		}

		void putProject(const string& name, ProjectBlock& projectBlock) {
			std::vector<std::string>& projects = projectsList.getStrings();
			projects.push_back(name);
			projectsList.setStrings(projects);
			projectBlockList.push_back(projectBlock);
		}

		void putProject(string name, ProjectBlock& projectBlock, ProjectDataBlock& projectDataBlock) {
			std::vector<std::string>& projects = projectsList.getStrings();
			projects.push_back(name);
			projectsList.setStrings(projects);
			size_t next = projectBlockList.size();
			projectBlockList.push_back(projectBlock);
			projectMovementBlockList[next] = projectDataBlock;
		}

		ProjectDataBlock& getprojectMovementBlock(int i) {
			return projectMovementBlockList[i];
		}

		std::map<int, ProjectDataBlock> getProjectMovementBlockList() {
			return projectMovementBlockList;
		}

		void parse(std::string content) {
			scannerpp::Scanner input(content);
			projectsList.fromASCII(input);
			int i = 0;
			for (std::string project : projectsList.getStrings()) {
				StringListBlock n; // = new StringListBlock();
				n.fromASCII(input);
				ProjectBlock b; // = new ProjectBlock();
				b.IBlockObject::parseBlock(n.getBlock());
				projectBlockList.push_back(b);
				if (b.getHasAnimationCache()) {
					ProjectDataBlock n1; // = new ProjectDataBlock();
					n1.fromASCII(input);
					projectMovementBlockList[i] = n1;
				}
				i++;
			}
		}

		std::string toString() {
			try {
				std::string out = projectsList.toASCII();
				int i = 0;
				for (ProjectBlock b : projectBlockList) {
					out += b.toASCII();
					if (b.getHasAnimationCache()) {
						out += projectMovementBlockList[i].toASCII();
					}
					i++;
				}
				return out;
			}
			catch (...) {
				
			}
			return "";

		}

	};
}
