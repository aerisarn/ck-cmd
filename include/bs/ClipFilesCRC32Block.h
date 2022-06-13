#pragma once

#include "TriBlock.h"
#include <list>

namespace AnimData {
	class ClipFilesCRC32Block : public TriBlock {

		std::vector<std::string> strings;
	public:
		void setStrings(std::vector<std::string> strings) {
			this->strings = strings;
		}

		std::vector<std::string>& getStrings() {
			return strings;
		}


		std::string getBlock() override {
			std::string out = "";
			if (strings.size() == 0) return "";
			for (std::string s : strings) {
				out += s + "\n";
			}
			return out;
		}

		void parseBlock(scannerpp::Scanner& input) {
			while (input.hasNextLine()) {
				strings.push_back(input.nextLine());
			}
		}

	};
}
