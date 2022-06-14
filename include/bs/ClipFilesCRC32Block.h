#pragma once

#include "TriBlock.h"
#include <list>

namespace AnimData {
	class ClipFilesCRC32Block : public TriBlock {

		std::vector<std::string> strings;
	public:

		ClipFilesCRC32Block() {}

		ClipFilesCRC32Block(const std::vector<std::string>& tri_strings) { strings = tri_strings; }

		void reserve(size_t tri_blocks)
		{
			strings.reserve(tri_blocks * 3);
		}

		void append(const std::string& path_crc, const std::string& name_crc)
		{
			strings.push_back(path_crc);
			strings.push_back(name_crc);
			strings.push_back("7891816");
		}


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
