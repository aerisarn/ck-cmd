#pragma once

#include "TriBlock.h"
#include <list>

namespace AnimData {
	class UnkEventData : public TriBlock {
		std::list<std::string> strings; // = new ArrayList<String>();
	public:

		void setStrings(std::list<std::string> strings) {
			this->strings = strings;
		}

		std::list<std::string> getStrings() {
			return strings;
		}


		std::string getBlock() override {
			std::string out = "";
			for (std::string s : strings) {
				out += s + "\n";
			}
			return out;
		}

		void parseBlock(scannerpp::Scanner& input) override {
			while (input.hasNextLine()) {
				strings.push_back(input.nextLine());
			}
		}


	};
}
