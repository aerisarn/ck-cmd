#pragma once

#include <src/models/ModelEdge.h>
#include <src/edges/MultipleVariantsEdge.h>

#include <hkbStateMachineStateInfo_4.h>

namespace ckcmd {
	namespace HKX {

		class StateMachineStateModel : public MultipleVariantsEdge {

			virtual std::vector<NodeType> handled_types() const override { return {}; }
			virtual std::vector<const hkClass*> handled_hkclasses() const override;

			hkbStateMachineStateInfo* variant(const ModelEdge& edge) const;
			virtual std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> additional_variants() const override;
			virtual std::vector<QString> additional_variants_names() const override;
		};
	}
}