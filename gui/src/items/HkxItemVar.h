#pragma once

#include <src/items/HkxItemReference.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemVar : public HkxItemReference {
		public:
			HkxItemVar() = default;
			~HkxItemVar() = default;
			HkxItemVar(const HkxItemVar&) = default;
			HkxItemVar& operator=(const HkxItemVar&) = default;

			HkxItemVar(size_t index) : HkxItemReference(index) {}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemVar);