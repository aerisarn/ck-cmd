#pragma once

#include "HkxItemVisitor.h"

namespace ckcmd {
	namespace HKX {

		//Decorate HkVariant
		class HkxVariant {

		public:

			HkxVariant(hkVariant& variant) : _variant(variant) {}

			void accept(HkxItemVisitor& visitor) const;
			void accept(HkxItemVisitor& visitor);

		protected:
			hkVariant& _variant;
		};

	}
}