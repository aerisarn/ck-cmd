#pragma once

#include <src/items/HkxItemReference.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemRagdollBone : public HkxItemReference {
		public:
			HkxItemRagdollBone() = default;
			~HkxItemRagdollBone() = default;
			HkxItemRagdollBone(const HkxItemRagdollBone&) = default;
			HkxItemRagdollBone& operator=(const HkxItemRagdollBone&) = default;

			HkxItemRagdollBone(size_t index) : HkxItemReference(index) {}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }

		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemRagdollBone);