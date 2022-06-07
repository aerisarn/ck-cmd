#pragma once

#include "HkxTableVariant.h"
#include "LinkGetter.h"
#include <vector>

namespace ckcmd {
	namespace HKX {

		//Decorate HkVariant
		class HkxLinkedTableVariant : public HkxTableVariant {

		public:

			HkxLinkedTableVariant(hkVariant& variant) : HkxTableVariant(variant) {}

			std::vector<Link> links();
		};

	}
}