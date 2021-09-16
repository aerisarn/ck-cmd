#pragma once

#include "IBlockObject.h"
#include <list>

namespace AnimData {
	class HandVariableData : public IBlockObject {
	public:

		// CommonLibSSE/include/RE/A/AttackAnimationArrayMap.h, but different
		enum EquipType : std::uint32_t
		{
			kHandToHandMelee = 0,
			kOneHandSword = 1,
			kOneHandDagger = 2,
			kOneHandAxe = 3,
			kOneHandMace = 4,
			kTwoHandSword = 5,
			kTwoHandAxe = 6,
			kBow = 7,
			kStaff = 8,
			kSpell = 9,
			kShield = 10,
			kCrossbow = 11
		};

		struct Data {
			std::string variable_name;
			EquipType value_min;
			EquipType value_max;

			std::string EquipTypeString(const EquipType& key) {
				switch (key) {
				case kHandToHandMelee:
					return "H2H";
				case kOneHandSword:
					return "1HS";
				case kOneHandDagger:
					return "1HD";
				case kOneHandAxe:
					return "1HA";
				case kOneHandMace:
					return "1HM";
				case kTwoHandSword:
					return "2HS";
				case kTwoHandAxe:
					return "2HA";
				case kBow:
					return "BOW";
				case kStaff:
					return "STA";
				case kCrossbow:
					return "XBW";
				case kSpell:
					return "MAG";
				case kShield:
					return "SHD";
				default:
					return "UNK";
				}
				return "UNK";
			}

			std::string getHandString()
			{
				std::string out = variable_name + " " + EquipTypeString(value_min);
				unsigned int i = value_min;
				while (i < value_max)
				{
					out += "|" + EquipTypeString((EquipType)++i);
				}
				return out;
			}

		};

	private:
		std::vector<Data> variables;
	public:

		std::vector<Data>& getVariables() {
			return variables;
		}

		std::string getBlock() override {
			std::string out = "";
			out += std::to_string(variables.size());
			for (const auto& data : variables)
			{
				out += data.variable_name + "\n";
				out += std::to_string((int)data.value_min) + "\n";
				out += std::to_string((int)data.value_max) + "\n";
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
							(EquipType)std::atoi(input.nextLine().c_str()),
							(EquipType)std::atoi(input.nextLine().c_str())
						}
					);
				}
			}
		}


	};
}
