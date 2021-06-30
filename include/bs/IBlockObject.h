#pragma once

#include <string>

#include "Scanner.h"

namespace AnimData {
	class IBlockObject {
	public:

		virtual void parseBlock(scannerpp::Scanner& input) = 0;

		virtual void parseBlock(std::string blockContent) {
			parseBlock(scannerpp::Scanner(blockContent));
		}

		virtual std::string getBlock() = 0;
	};
}
