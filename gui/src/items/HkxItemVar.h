#pragma once

#include <src/items/HkxItemReference.h>

#define VARIABLE_ROLE_INDEX 1
#define VARIABLE_FLAGS_INDEX 2
#define VARIABLE_TYPE_INDEX 3
//#define VARIABLE_MIN_INDEX 4
//#define VARIABLE_MAX_INDEX 5
#define VARIABLE_VALUE_INDEX 4

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

			virtual AssetType assetType() { return AssetType::variables; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemVar);