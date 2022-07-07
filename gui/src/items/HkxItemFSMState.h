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

			HkxItemFSMState(size_t index) :
				HkxItemReference(index)
			{}

			size_t getStateId(int combo_index) const {
				return -1;
			}

			virtual operator QVariant() const override { QVariant v; v.setValue(*this); return v; }

			virtual AssetType assetType() { return AssetType::FSM_states; }
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemFSMState);