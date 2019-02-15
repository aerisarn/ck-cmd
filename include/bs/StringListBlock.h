#pragma once

#include "Block.h"
#include <list>

namespace AnimData {
	class StringListBlock : public Block {

		std::list<std::string> strings;

	public:
		
		void setStrings(std::list<std::string> strings) {
			this->strings = strings;
		}

		virtual std::list<std::string> getStrings() {
			return strings;
		}

		void clear() {
			strings.clear();
		}

		virtual std::string getBlock() {
			std::string out = "";
			if (strings.size() == 0) return out;
			for (std::string s : strings) {
				out += s + "\n";
			}
			return out;
		}

		virtual void parseBlock(scannerpp::Scanner& input) {
			while (input.hasNextLine()) {
				strings.push_back(input.nextLine());
			}
		}
	};
}
