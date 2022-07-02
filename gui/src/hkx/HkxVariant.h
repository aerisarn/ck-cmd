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

			QString name() const;
			QString className() const;
			bool setName(const QString& name);

		protected:
			hkVariant& _variant;
		};

	}
}