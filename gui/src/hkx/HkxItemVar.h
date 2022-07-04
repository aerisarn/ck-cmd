#pragma once

#include <src/hkx/HkxItemReference.h>
#include <src/hkx/BehaviorBuilder.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemVar : public HkxItemReference<BehaviorBuilder> {
		public:
			HkxItemVar() = default;
			~HkxItemVar() = default;
			HkxItemVar(const HkxItemVar&) = default;
			HkxItemVar& operator=(const HkxItemVar&) = default;

			HkxItemVar(BehaviorBuilder* builder, size_t index) : HkxItemReference(builder, index) {}

			virtual QString getValue() const override {
				return _builder->getVariable(_index);
			}

			virtual QStringList getValues() const override {
				return _builder->getVariables();
			}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemVar);