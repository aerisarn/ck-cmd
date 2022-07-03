#pragma once

#include <src/models/ModelEdge.h>

namespace ckcmd {
	namespace HKX {

		class IEdge
		{
		public:
			virtual std::vector<NodeType> handled_types() const = 0;
			virtual std::vector<const hkClass*> handled_hkclasses() const = 0;

			virtual int rows(const ModelEdge& edge, ResourceManager& manager) const = 0;
			virtual int columns(int row, const ModelEdge& edge, ResourceManager& manager) const = 0;
			virtual int childCount(const ModelEdge& edge, ResourceManager& manager) const = 0;
			virtual std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager) const = 0;
			virtual int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const = 0;
			virtual ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const = 0;
			virtual QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const = 0;

			virtual bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager) = 0;
			virtual bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager) = 0;
			virtual bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager) = 0;
			virtual bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager) = 0;
		};
	}
}