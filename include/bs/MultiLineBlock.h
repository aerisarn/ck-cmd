#pragma once

#include "BlockObject.h"
#include <regex>

namespace AnimData {

	class MultiLineBlock : public BlockObject {

		int linesPerBlock = 1;
	public:
		MultiLineBlock() {}

		MultiLineBlock(int linesPerBlock) {
			this->linesPerBlock = linesPerBlock;
		}

		virtual std::string toASCII() {
			std::string out = "";
			std::string blockContent = getBlock();
			if (!blockContent.empty()) {
				int lines = std::count(blockContent.begin(), blockContent.end(), '\n');
				int blockLines = lines / linesPerBlock;
				out += std::to_string(blockLines) + "\n";
				out += blockContent;
			}
			else
			{
				out += "0\n";
			}
			return out;
		}

		virtual void fromASCII(std::string ASCIIBlock) {
			fromASCII(scannerpp::Scanner(ASCIIBlock));
		}

		virtual void fromASCII(scannerpp::Scanner& input) {
			int numASCIIlines = input.nextInt();
			std::string blockContent = "";
			for (int i = 0; i < numASCIIlines*linesPerBlock; i++) {
				blockContent += input.nextLine() + "\n";
			}
			if (numASCIIlines > 0) {
				try {
					parseBlock(blockContent);
				}
				catch (...) {

				}
			}
		}

	};
}
