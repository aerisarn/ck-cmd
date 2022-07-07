#pragma once

#include <src/items/HkxItemReference.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemEvent : public HkxItemReference {
		public:
			HkxItemEvent() = default;
			~HkxItemEvent() = default;
			HkxItemEvent(const HkxItemEvent&) = default;
			HkxItemEvent& operator=(const HkxItemEvent&) = default;

			HkxItemEvent(size_t index) : HkxItemReference(index) {}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }

			virtual AssetType assetType() { return AssetType::events; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemEvent);