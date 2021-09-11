#pragma once

#include <src/hkx/HkxItemReference.h>
#include <src/hkx/BehaviorBuilder.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemEvent : public HkxItemReference<BehaviorBuilder> {
		public:
			HkxItemEvent() = default;
			~HkxItemEvent() = default;
			HkxItemEvent(const HkxItemEvent&) = default;
			HkxItemEvent& operator=(const HkxItemEvent&) = default;

			HkxItemEvent(BehaviorBuilder* builder, size_t index) : HkxItemReference(builder, index) {}

			virtual QString getValue() const override {
				return _builder->getEvent(_index);
			}

			virtual QStringList getValues() const override {
				return _builder->getEvents();
			}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }

		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemEvent);