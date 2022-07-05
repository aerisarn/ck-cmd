#pragma once

#include <map>
#include <array>

namespace AnimData {

	struct root_movement_t {
		float duration;
		std::map<float, std::array<float, 3>> translations;
		std::map<float, std::array<float, 4>> rotations;
	};
}