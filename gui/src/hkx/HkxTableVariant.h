#pragma once

#include "HkxItemVisitor.h"

namespace ckcmd {
	namespace HKX {

		//Decorate HkVariant
		class HkxTableVariant {

		public:

			HkxTableVariant(hkVariant& variant) : _variant(variant) {}

			void accept(HkxItemVisitor& visitor) const;
			void accept(HkxItemVisitor& visitor);

		private:
			hkVariant& _variant;
		};

	}
}