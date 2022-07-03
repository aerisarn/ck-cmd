#pragma once

#include <src/edges/Edge.h>

#include <hkbBehaviorReferenceGenerator_0.h>

namespace ckcmd {
	namespace HKX {

		class BehaviorReferenceModel : public Edge
		{
		public:
			virtual std::vector<const hkClass*> handled_hkclasses() const override;

			virtual int childCount(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
		};
	}
}
