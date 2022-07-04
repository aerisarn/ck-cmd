#pragma once

#include <src/hkx/HkxItemReference.h>
#include <src/hkx/SkeletonBuilder.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemRagdollBone : public HkxItemReference<SkeletonBuilder> {
		public:
			HkxItemRagdollBone() = default;
			~HkxItemRagdollBone() = default;
			HkxItemRagdollBone(const HkxItemRagdollBone&) = default;
			HkxItemRagdollBone& operator=(const HkxItemRagdollBone&) = default;

			HkxItemRagdollBone(SkeletonBuilder* builder, size_t index) : HkxItemReference<SkeletonBuilder>(builder, index) {}

			QString getValue() const {
				return _builder->getRagdollBone(_index);
			}

			virtual QStringList getValues() const override {
				return _builder->getRagdollBones();
			}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }

		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemRagdollBone);