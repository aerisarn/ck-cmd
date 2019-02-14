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
				std::regex m("(\r\n)|(\n)|(\r)");
				//Matcher m = Pattern.compile("(\r\n)|(\n)|(\r)").matcher(blockContent);
				int lines = 1;
				while (std::regex_match(blockContent, m)) {
					lines++;
				}
				int blockLines = lines / linesPerBlock + lines%linesPerBlock;

				out += blockLines - 1 + "\n";
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
			//std::string temp = input.nextLine();
			std::string blockContent = "";
			for (int i = 0; i < numASCIIlines*linesPerBlock; i++) {
				blockContent += input.nextLine() + "\n";
			}
			if (numASCIIlines > 0) {
				try {
					parseBlock(blockContent);
				}
				catch (...) {
					//e.printStackTrace();
				}
			}
		}

	};
}
