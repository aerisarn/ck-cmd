#pragma once

#include "Block.h"

namespace AnimData {
	class StringBlock : public Block {
	
		std::string string;

	public:
		std::string getString() {
			return string;
		}

		void setString(std::string string) {
			this->string = string;
		}

		std::string getBlock() {
			return string + "\n";
		}

		void parseBlock(scannerpp::Scanner& input) {
			string = input.nextLine();
		}
	};
}
