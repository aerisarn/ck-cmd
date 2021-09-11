#pragma once

#include "ClipGeneratorBlock.h"
#include "StringListBlock.h"

namespace AnimData {
	class ProjectBlock : public Block {

		bool hasProjectFiles = false;
		StringListBlock projectFiles;
		bool hasAnimationCache = false;
		std::list<ClipGeneratorBlock> clips;

	public:

		void clear() {
			hasProjectFiles = false;
			projectFiles.clear();
			hasAnimationCache = false;
			clips.clear();
		}

		bool getHasAnimationCache() {
			return hasAnimationCache;
		}

		StringListBlock getProjectFiles() {
			return projectFiles;
		}

		std::list<ClipGeneratorBlock>& getClips() {
			return clips;
		}

		void setClips(std::list<ClipGeneratorBlock> clips) {
			this->clips = clips;
		}

		bool isHasProjectFiles() {
			return hasProjectFiles;
		}

		void setHasProjectFiles(bool hasProjectFiles) {
			this->hasProjectFiles = hasProjectFiles;
		}

		bool isHasAnimationCache() {
			return hasAnimationCache;
		}

		void setHasAnimationCache(bool hasAnimationCache) {
			this->hasAnimationCache = hasAnimationCache;
		}

		void setProjectFiles(StringListBlock projectFiles) {
			this->projectFiles = projectFiles;
		}

		std::string getBlock() override {
			std::string out = "";
			out += (hasProjectFiles ? "1" : "0") + std::string("\n");
			out += projectFiles.toASCII();
			out += (hasAnimationCache ? "1" : "0") + std::string("\n");
			if (hasAnimationCache) {
				for (ClipGeneratorBlock clip : clips)
					out += clip.getBlock();
			}
			return out;
		}

		void parseBlock(scannerpp::Scanner& input) override {
			hasProjectFiles = input.nextInt() == 1;// input.nextLine();
			if (hasProjectFiles)
				projectFiles.fromASCII(input);
			hasAnimationCache = input.nextInt() == 1;// input.nextLine();
			if (hasAnimationCache) {
				while (input.hasNextLine()) {
					ClipGeneratorBlock b; // = new ClipGeneratorBlock();
					b.parseBlock(input);
					clips.push_back(b);
					input.nextLine();
				}
			}
		}
	};
}
