#pragma once

#include <src/hkx/HkxItemReference.h>
#include <src/hkx/BehaviorBuilder.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemFSMState : public HkxItemReference<BehaviorBuilder> {
			size_t _fsm_index;
		public:
			HkxItemFSMState() = default;
			~HkxItemFSMState() = default;
			HkxItemFSMState(const HkxItemFSMState&) = default;
			HkxItemFSMState& operator=(const HkxItemFSMState&) = default;

			HkxItemFSMState(BehaviorBuilder* builder, size_t fsm_index, size_t index) :
				HkxItemReference<BehaviorBuilder>(builder, index),
				_fsm_index(fsm_index)
			{}

			QString getValue() const {
				return _builder->getFSMState(_fsm_index, _index);
			}

			virtual QStringList getValues() const override {
				return _builder->getFSMStates(_fsm_index);
			}

			size_t getStateId(int combo_index) const {
				return _builder->getFSMStateId(_fsm_index, combo_index);
			}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }

		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemFSMState);