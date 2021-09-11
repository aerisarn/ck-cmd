#pragma once

#include <src/hkx/HkxItemReference.h>
#include <src/hkx/SkeletonBuilder.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemBone : public HkxItemReference<SkeletonBuilder> {
		public:
			HkxItemBone() = default;
			~HkxItemBone() = default;
			HkxItemBone(const HkxItemBone&) = default;
			HkxItemBone& operator=(const HkxItemBone&) = default;

			HkxItemBone(SkeletonBuilder* builder, size_t index) : HkxItemReference<SkeletonBuilder>(builder, index) {}

			QString getValue() const {
				return _builder->getSkeletonBone(_index);
			}

			virtual QStringList getValues() const override {
				return _builder->getSkeletonBones();
			}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemBone);