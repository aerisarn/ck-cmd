#pragma once

#include <src/items/HkxItemReference.h>

namespace ckcmd {
	namespace HKX {

		class HkxItemFSMState : public HkxItemReference {
			size_t _fsm_index;
		public:
			HkxItemFSMState() = default;
			~HkxItemFSMState() = default;
			HkxItemFSMState(const HkxItemFSMState&) = default;
			HkxItemFSMState& operator=(const HkxItemFSMState&) = default;

			HkxItemFSMState(size_t fsm_index, size_t index) :
				HkxItemReference(index),
				_fsm_index(fsm_index)
			{}

			size_t getStateId(int combo_index) const {
				return -1;
			}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }

		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemFSMState);