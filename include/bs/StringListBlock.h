#pragma once

#include "Block.h"
#include <list>

namespace AnimData {
	class StringListBlock : public Block {

		std::vector<std::string> strings;

	public:

		StringListBlock() {}

		StringListBlock(size_t size) { strings.reserve(size); }
		
		StringListBlock(const std::vector<std::string>& _strings) :
			strings(_strings)
		{
		}

		void append(const std::string& file) {
			strings.push_back(file);
		}

		void remove(int index) {
			strings.erase(strings.begin() + index);
		}

		void setStrings(std::vector<std::string> strings) {
			this->strings = strings;
		}

		virtual std::vector<std::string> getStrings() {
			return strings;
		}

		virtual size_t size() {
			return strings.size();
		}

		virtual void resize(size_t new_size) {
			return strings.resize(new_size);
		}

		virtual void reserve(size_t new_size) {
			return strings.reserve(new_size);
		}

		virtual std::string& operator[](int index) {
			return strings[index];
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
