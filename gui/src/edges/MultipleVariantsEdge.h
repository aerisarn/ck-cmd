#pragma once

#include <src/edges/Edge.h>

namespace ckcmd {
	namespace HKX {

		class MultipleVariantsEdge : public Edge
		{
			virtual std::vector<std::function<hkVariant* (hkVariant*)>> additional_variants() const = 0;

		public:

			virtual int rows(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int columns(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childCount(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;

			virtual bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager);
			virtual bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
			virtual bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
			virtual bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager);
		};
	}
}