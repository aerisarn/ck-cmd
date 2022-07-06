#include <src/edges/Edge.h>

#include <src/hkx/HkxLinkedTableVariant.h>

using namespace ckcmd::HKX;

std::vector<NodeType> Edge::handled_types() const
{
	return { NodeType::HavokNative };
}

std::vector<const hkClass*> Edge::handled_hkclasses() const
{ 
	return {}; 
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
				auto child_variant = manager.findVariant(edge.file(), links.at(i)._ref);
				if (child_variant != nullptr)
					return ModelEdge(
						edge,
						edge.project(),
						edge.file(),
						row,
						column,
						edge.subindex(),
						child_variant,
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

std::pair<int, int> Edge::dataStart() const
{
	return { 1, 1 };
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

bool Edge::canAdd(const ModelEdge& this_edge, const ModelEdge& another_edge, ResourceManager& manager) const
{
	hkVariant* parent_variant = this_edge.childItem<hkVariant>();
	hkVariant* child_variant = another_edge.childItem<hkVariant>();
	if (nullptr != parent_variant && nullptr != child_variant)
	{

	}
	return false;
}

const hkClass* Edge::rowClass(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant && row > 0)
	{
		return HkxTableVariant(*variant).rowClass(row - 1);
	}
	return nullptr;
}

bool Edge::isArray(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	hkVariant* variant = edge.childItem<hkVariant>();
	if (nullptr != variant && row > 0)
	{
		auto arrayrows = HkxTableVariant(*variant).arrayrows();
		return std::find(arrayrows.begin(), arrayrows.end(), row - 1) != arrayrows.end();
	}
	return false;
}