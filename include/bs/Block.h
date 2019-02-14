#pragma once

#include "MultiLineBlock.h"

namespace AnimData {
	class Block : public MultiLineBlock {
	public: 
		Block() : MultiLineBlock(1) {}
	};
}
