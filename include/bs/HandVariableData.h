#pragma once

#include "IBlockObject.h"
#include <list>

namespace AnimData {
	class HandVariableData : public IBlockObject {
	public:
		struct Data {
			std::string variable_name;
			std::string value_min;
			std::string value_max;
		};
	private:
		std::vector<Data> variables;
	public:


		std::string getBlock() override {
			std::string out = "";
			out += std::to_string(variables.size());
			for (const auto& data : variables)
			{
				out += data.variable_name + "\n";
				out += data.value_min + "\n";
				out += data.value_max + "\n";
			}
			return out;
		}

		void parseBlock(scannerpp::Scanner& input) override {
			if (input.hasNextLine())
			{
				int blocks = std::atoi(input.nextLine().c_str());
				for (int i = 0; i < blocks; i++)
				{
					variables.push_back(
						{
							input.nextLine(),
							input.nextLine(),
							input.nextLine()
						}
					);
				}
			}
		}


	};
}
