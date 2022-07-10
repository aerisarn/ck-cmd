#pragma once

#include "HkxVariant.h"
#include <src/hkx/HkxTypeInfo.h>
#include <QVariant>

namespace ckcmd {
	namespace HKX {

		//Decorate HkVariant
		class HkxTableVariant : public HkxVariant {

		public:

			HkxTableVariant(hkVariant& variant) : HkxVariant(variant) {}

			size_t rows();
			size_t columns();

			size_t columns(int row_index);
			QString rowName(int row_index);
			TypeInfo rowClass(int row_index);

			std::vector<int> arrayrows();

			QStringList rowNames();
			std::vector<TypeInfo> rowClasses();

			QVariant data(int row, int column);
			bool setData(int row, int column, const QVariant& variant);
			bool resizeColumns(int row, int column, int delta);
		};
	}
}