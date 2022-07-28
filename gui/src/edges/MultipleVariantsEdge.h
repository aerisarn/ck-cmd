#pragma once

#include <src/edges/Edge.h>

namespace ckcmd {
	namespace HKX {

		class MultipleVariantsEdge : public Edge
		{
		private:
			virtual std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> _additional_variants() const;
			virtual std::vector<QString> _additional_variants_names() const;

		protected:
			virtual std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> additional_variants() const { return {}; };
			virtual std::vector<QString> additional_variants_names() const { return {}; };

		public:

			virtual int rows(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int columns(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childCount(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;

			virtual TypeInfo rowClass(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual bool isArray(int row, const ModelEdge& edge, ResourceManager& manager) const override;

			virtual bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager);
			virtual bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
			virtual bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
			virtual bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager);
		};
	}
}