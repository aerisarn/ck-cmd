#pragma once

#include <src/models/ModelEdge.h>

namespace ckcmd {
	namespace HKX {

		class Edge
		{
		public:
			virtual std::vector<NodeType> handled_types() const;

			virtual int rows(const ModelEdge& edge, ResourceManager& manager) const;
			virtual int columns(int row, const ModelEdge& edge, ResourceManager& manager) const;
			virtual int childCount(const ModelEdge& edge, ResourceManager& manager) const;
			virtual std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager) const;
			virtual int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const;
			virtual ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const;
			virtual QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const;

			virtual bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager);
			virtual bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
			virtual bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
			virtual bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager);
		};
	}
}

