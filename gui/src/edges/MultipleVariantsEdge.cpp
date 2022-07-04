#include <src/edges/MultipleVariantsEdge.h>

#include <src/hkx/HkxLinkedTableVariant.h>

using namespace ckcmd::HKX;

int MultipleVariantsEdge::rows(const ModelEdge& edge, ResourceManager& manager) const
{
	int count = 0;
	hkVariant* variant = edge.childItem<hkVariant>();
	for (size_t i = 0; i < additional_variants().size(); ++i)
	{
		hkVariant* v = additional_variants().at(i)(edge, manager, variant);
		if (v != nullptr)
		{
			count += HkxTableVariant(*v).rows();
		}
	}
	return count + Edge::rows(edge, manager);
}

int MultipleVariantsEdge::columns(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	int count = Edge::rows(edge, manager);
	if (row < count)
		return Edge::columns(row, edge, manager);

	hkVariant* variant = edge.childItem<hkVariant>();
	int row_index = row - count;
	for (size_t i = 0; i < additional_variants().size(); ++i)
	{
		if (row_index < 0)
			break;
		hkVariant* v = additional_variants().at(i)(edge, manager, variant);
		if (v != nullptr)
		{
			int variant_rows = HkxTableVariant(*v).rows();
			if (row_index < variant_rows)
				return 1  + HkxTableVariant(*v).columns(row_index);
			row_index -= variant_rows;
		}
	}
	return 0;
}

int MultipleVariantsEdge::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	int count = 0;
	hkVariant* variant = edge.childItem<hkVariant>();
	for (size_t i = 0; i < additional_variants().size(); ++i)
	{
		hkVariant* v = additional_variants().at(i)(edge, manager, variant);
		if (v != nullptr)
		{
			count += HkxLinkedTableVariant(*v).links().size();
		}
	}
	return count + Edge::childCount(edge, manager);
}

std::pair<int, int> MultipleVariantsEdge::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	int count = Edge::childCount(edge, manager);
	if (index < count)
		return Edge::child(index, edge, manager);

	int rows = Edge::rows(edge, manager);
	hkVariant* variant = edge.childItem<hkVariant>();
	int child_index = index - count;
	for (size_t i = 0; i < additional_variants().size(); ++i)
	{
		if (child_index < 0)
			break;
		hkVariant* v = additional_variants().at(i)(edge, manager, variant);
		if (v != nullptr)
		{
			auto variant_links = HkxLinkedTableVariant(*v).links();
			if (index < (int)variant_links.size())
				return { rows + variant_links.at(index)._row, variant_links.at(index)._column };
			rows += HkxLinkedTableVariant(*v).rows();
			child_index -= variant_links.size();
		}
	}
	return {MODELEDGE_INVALID, MODELEDGE_INVALID};
}

int MultipleVariantsEdge::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	int count = Edge::rows(edge, manager);
	if (row < count)
		return Edge::childIndex(row, column, edge, manager);

	hkVariant* variant = edge.childItem<hkVariant>();
	int row_index = row - count;
	int child_index = Edge::childCount(edge, manager);
	for (size_t i = 0; i < additional_variants().size(); ++i)
	{
		if (row_index < 0)
			break;
		hkVariant* v = additional_variants().at(i)(edge, manager, variant);
		if (v != nullptr)
		{
			int variant_rows = HkxTableVariant(*v).rows();
			if (row_index < variant_rows)
			{
				auto links = HkxLinkedTableVariant(*v).links();
				for (int l = 0; l < (int)links.size(); ++l)
				{
					if (links.at(l)._row == row_index && links.at(l)._column == column)
						return child_index;
				}
				++child_index;
			}
			row_index -= variant_rows;
			child_index += HkxLinkedTableVariant(*v).links().size();
		}
	}
	return MODELEDGE_INVALID;
}

ModelEdge MultipleVariantsEdge::child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	int count = Edge::rows(edge, manager);
	if (row < count)
		return Edge::child(row, column, edge, manager);

	hkVariant* variant = edge.childItem<hkVariant>();
	int row_index = row - count;
	for (size_t i = 0; i < additional_variants().size(); ++i)
	{
		if (row_index < 0)
			break;
		hkVariant* v = additional_variants().at(i)(edge, manager, variant);
		if (v != nullptr)
		{
			int variant_rows = HkxTableVariant(*v).rows();
			if (row_index < variant_rows)
			{
				auto links = HkxLinkedTableVariant(*v).links();
				for (int l = 0; l < (int)links.size(); ++l)
				{
					if (links.at(l)._row == row_index && links.at(l)._column == column)
					{
						auto child_variant_index = manager.findIndex(edge.file(), links.at(i)._ref);
						if (child_variant_index != -1)
							return ModelEdge(
								edge,
								edge.project(),
								edge.file(),
								row,
								column,
								edge.subindex(),
								manager.at(edge.file(), child_variant_index),
								NodeType::HavokNative
							);
					}
				}
			}
			row_index -= variant_rows;
		}
	}
	return ModelEdge();
}

QVariant MultipleVariantsEdge::data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	int count = Edge::rows(edge, manager);
	if (row < count)
		return Edge::data(row, column, edge, manager);

	hkVariant* variant = edge.childItem<hkVariant>();
	int row_index = row - count;
	for (size_t i = 0; i < additional_variants().size(); ++i)
	{
		if (row_index < 0)
			break;
		hkVariant* v = additional_variants().at(i)(edge, manager, variant);
		if (v != nullptr)
		{
			int variant_rows = HkxTableVariant(*v).rows();
			if (row_index < variant_rows)
			{
				if (column == 0)
				{
					return HkxTableVariant(*v).rowName(row_index);
				}
				return HkxTableVariant(*v).data(row_index, column - 1);
			}
			row_index -= variant_rows;
		}
	}
	return "Invalid";
}

bool MultipleVariantsEdge::setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager)
{
	if (row == 0 && column == 0)
	{
		hkVariant* variant = edge.childItem<hkVariant>();
		int result = HkxTableVariant(*variant).setName(data.toString());
		if (!result)
		{
			for (size_t i = 0; i < additional_variants().size(); ++i)
			{
				hkVariant* v = additional_variants().at(i)(edge, manager, variant);
				if (v != nullptr)
				{
					result = HkxTableVariant(*v).setName(data.toString());
					if (result)
						return result;
				}
			}
		}
	}
	else {
		int count = Edge::rows(edge, manager);
		if (row < count)
			return Edge::setData(row, column, edge, data, manager);

		hkVariant* variant = edge.childItem<hkVariant>();
		int row_index = row - count;
		for (size_t i = 0; i < additional_variants().size(); ++i)
		{
			if (row_index < 0)
				break;
			hkVariant* v = additional_variants().at(i)(edge, manager, variant);
			if (v != nullptr)
			{
				int variant_rows = HkxTableVariant(*v).rows();
				if (row_index < variant_rows)
				{
					if (column == 0)
						return false;
					return HkxTableVariant(*v).setData(row_index, column - 1, data);
				}
				row_index -= variant_rows;
			}
		}
	}
	return false;
}

bool MultipleVariantsEdge::addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	return false;
}

bool MultipleVariantsEdge::removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	return false;
}

bool MultipleVariantsEdge::changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager)
{
	int count = Edge::rows(edge, manager);
	if (row < count)
		return Edge::changeColumns(row, column_start, delta, edge, manager);

	hkVariant* variant = edge.childItem<hkVariant>();
	int row_index = row - count;
	for (size_t i = 0; i < additional_variants().size(); ++i)
	{
		if (row_index < 0)
			break;
		hkVariant* v = additional_variants().at(i)(edge, manager, variant);
		if (v != nullptr)
		{
			int variant_rows = HkxTableVariant(*v).rows();
			if (row_index < variant_rows)
			{
				return HkxTableVariant(*v).resizeColumns(row_index, column_start - 1, delta);
			}
			row_index -= variant_rows;
		}
	}
	return false;
}

const hkClass* MultipleVariantsEdge::rowClass(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	int count = Edge::rows(edge, manager);
	if (row < count)
		return Edge::rowClass(row, edge, manager);

	hkVariant* variant = edge.childItem<hkVariant>();
	int row_index = row - count;
	for (size_t i = 0; i < additional_variants().size(); ++i)
	{
		if (row_index < 0)
			break;
		hkVariant* v = additional_variants().at(i)(edge, manager, variant);
		if (v != nullptr)
		{
			int variant_rows = HkxTableVariant(*v).rows();
			if (row_index < variant_rows)
			{
				return HkxTableVariant(*v).rowClass(row_index);
			}
			row_index -= variant_rows;
		}
	}
	return nullptr;
}