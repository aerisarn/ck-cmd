#pragma once

#include "HkxItemVisitor.h"
#include <QString>

namespace ckcmd {
	namespace HKX {

		//Decorate HkVariant
		class HkxVariant {

		public:

			HkxVariant(hkVariant& variant) : _variant(variant) {}

			void accept(HkxItemVisitor& visitor) const;
			void accept(HkxItemVisitor& visitor);

			QString name();
			QString className();
			bool setName(const QString& name);

		protected:
			hkVariant& _variant;
		};

	}
}