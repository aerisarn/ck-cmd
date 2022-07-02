#include <src/edges/Edge.h>

#include <src/hkx/HkxLinkedTableVariant.h>

using namespace ckcmd::HKX;

std::vector<NodeType> Edge::handled_types() const
{
	return { NodeType::HavokNative };
}

int Edge::rows(const ModelEdge& edge, ResourceManager& manager) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		return 1 + HkxLinkedTableVariant(*variant).rows();
	}
	return 0;
}

int Edge::columns(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (row == 0)
		return 1;
	if (nullptr != variant)
	{
		return 1 + HkxLinkedTableVariant(*variant).columns(row - 1);
	}
	return 0;
}

int Edge::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		return HkxLinkedTableVariant(*variant).links().size();
	}
	return 0;
}

std::pair<int, int> Edge::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto links = HkxLinkedTableVariant(*variant).links();
		int child_index = 0;
		for (size_t i = 0; i < links.size(); ++i)
		{
			if (child_index == index)
				return { links.at(i)._row + 1, links.at(i)._column + 1 };
			++child_index;
		}
	}
	return {MODELEDGE_INVALID, MODELEDGE_INVALID};
}

int Edge::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto links = HkxLinkedTableVariant(*variant).links();
		for (size_t i = 0; i < links.size(); ++i)
		{
			if (links.at(i)._row == row - 1 && links.at(i)._column == column - 1)
				return i;
		}
	}
	return MODELEDGE_INVALID;
}

ModelEdge Edge::child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant && edge.file() != -1)
	{
		auto links = HkxLinkedTableVariant(*variant).links();
		for (size_t i = 0; i < links.size(); ++i)
		{
			if (links.at(i)._row == row - 1 && links.at(i)._column == column - 1)
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
						manager.at(edge.file(),	child_variant_index),
						NodeType::HavokNative
					);
			}
		}
	}
	return ModelEdge();
}

QVariant Edge::data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		if (row == 0 && column == 0)
		{
			return HkxTableVariant(*variant).name();
		}
		else if (column == 0)
		{
			return HkxTableVariant(*variant).rowName(row - 1);
		}
		else
		{
			return HkxTableVariant(*variant).data(row - 1, column - 1);
		}
	}
	return "Invalid data";
}

bool Edge::setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		if (row == 0 && column == 0)
		{
			return HkxTableVariant(*variant).setName(data.toString());
		}
		else if (column == 0)
		{
			return false;
		}
		else
		{
			int rows = HkxTableVariant(*variant).rows();
			int columns = HkxTableVariant(*variant).columns(row - 1);
			if (row - 1 < rows && column - 1 < columns)
			{
				return HkxTableVariant(*variant).setData(row - 1, column - 1, data);
			}
		}
	}
	return false;
}

bool Edge::addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	return false;
}

bool Edge::removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	return false;
}

bool Edge::changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager)
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant)
	{
		auto arrayrows = HkxTableVariant(*variant).arrayrows();
		if (std::find(arrayrows.begin(), arrayrows.end(), row - 1) != arrayrows.end())
		{
			return HkxTableVariant(*variant).resizeColumns(row -1 , column_start - 1, delta);
		}
	}
	return false;
}