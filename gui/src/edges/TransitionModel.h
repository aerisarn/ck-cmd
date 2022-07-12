#pragma once

#include <src/models/ModelEdge.h>
#include <src/edges/MultipleVariantsEdge.h>

#include <hkbStateMachineTransitionInfo_1.h>

namespace ckcmd {
	namespace HKX {

		class TransitionModel : public MultipleVariantsEdge {
			
			hkbStateMachineTransitionInfo* variant(const ModelEdge& edge) const;
			virtual std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> additional_variants() const override;
		};
	}
}