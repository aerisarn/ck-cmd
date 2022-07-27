#pragma once

#include <src/edges/IEdge.h>

namespace ckcmd {
	namespace HKX {

		class Edge : public IEdge
		{
		public:
			virtual std::vector<NodeType> handled_types() const override;
			virtual std::vector<const hkClass*> handled_hkclasses() const override;

			virtual int rows(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int columns(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childCount(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual std::pair<int, int> dataStart(const ModelEdge& edge) const override;

			virtual TypeInfo rowClass(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual bool canAdd(const ModelEdge& this_edge, const ModelEdge& another_edge, ResourceManager& manager) const override;
			virtual bool isArray(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual std::vector<std::tuple<QString, TypeInfo, bool, size_t>> bindables(ModelEdge& edge, ResourceManager& manager) const override;

			virtual bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager) override;
			virtual bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager) override;
			virtual bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager) override;
			virtual bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager) override;
		};
	}
}

