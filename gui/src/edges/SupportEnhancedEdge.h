#pragma once

#include <src/models/ModelEdge.h>
#include <src/edges/MultipleVariantsEdge.h>

namespace ckcmd {
	namespace HKX {

		class SupportEnhancedEdge : public MultipleVariantsEdge
		{
		protected:
			virtual int supports() const = 0;
			virtual const char* supportName(int support_index) const = 0;
			virtual NodeType supportType(int support_index) const = 0;

		public:

			virtual int rows(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int columns(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childCount(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual std::pair<int, int> dataStart() const override;

			virtual TypeInfo rowClass(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual bool isArray(int row, const ModelEdge& edge, ResourceManager& manager) const override;

			virtual bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager) override;
			virtual bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager) override;
			virtual bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager) override;
			virtual bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager) override;
		};
	}
}