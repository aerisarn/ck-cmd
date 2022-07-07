#pragma once

#include <src/items/HkxItemReference.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemBone : public HkxItemReference {
		public:
			HkxItemBone() = default;
			~HkxItemBone() = default;
			HkxItemBone(const HkxItemBone&) = default;
			HkxItemBone& operator=(const HkxItemBone&) = default;

			HkxItemBone(size_t index) : HkxItemReference(index) {}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }

			virtual AssetType assetType() { return AssetType::bones; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemBone);