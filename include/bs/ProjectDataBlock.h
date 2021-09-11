#pragma once

#include "ClipMovementData.h"

namespace AnimData {
	class ProjectDataBlock : public Block {

		std::vector<ClipMovementData> movementData; // = new ArrayList<>();

	public: 
		std::vector<ClipMovementData>& getMovementData() {
			return movementData;
		}

		void setMovementData(std::vector<ClipMovementData> movementData) {
			this->movementData = movementData;
		}

		void parseBlock(scannerpp::Scanner& input) override {
			while (input.hasNextLine()) {
				ClipMovementData b;
				b.parseBlock(input);
				movementData.push_back(b);
				input.nextLine();
			}
		}

		std::string getBlock() override {
			std::string out = "";
			for (ClipMovementData data : movementData)
				out += data.getBlock();
			return out;
		}

	};
}
