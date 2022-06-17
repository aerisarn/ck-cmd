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

			size_t columns(int row_index);

			QStringList rowNames();

			QVariant data(int row, int column);
			bool setData(int row, int column, const QVariant& variant);
		};

	}
}