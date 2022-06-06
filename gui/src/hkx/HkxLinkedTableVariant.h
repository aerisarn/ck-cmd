#pragma once

#include "HkxVariant.h"

namespace ckcmd {
	namespace HKX {

		//Decorate HkVariant
		class HkxTableVariant : private HkxVariant {

		public:

			HkxTableVariant(hkVariant& variant) : HkxVariant(variant) {}

			size_t rows();
			size_t columns();

			size_t data(int row, int column);
		};

	}
}