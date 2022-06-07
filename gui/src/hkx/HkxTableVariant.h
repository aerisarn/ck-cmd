#pragma once

#include "HkxVariant.h"
#include <QVariant>

namespace ckcmd {
	namespace HKX {

		//Decorate HkVariant
		class HkxTableVariant : protected HkxVariant {

		public:

			HkxTableVariant(hkVariant& variant) : HkxVariant(variant) {}

			size_t rows();
			size_t columns();

			QString name();

			QVariant data(int row, int column);
		};

	}
}