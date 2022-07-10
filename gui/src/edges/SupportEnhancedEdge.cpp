#include <src/edges/SupportEnhancedEdge.h>

using namespace ckcmd::HKX;

int SupportEnhancedEdge::rows(const ModelEdge& edge, ResourceManager& manager) const
{
	return supports() + MultipleVariantsEdge::rows(edge, manager);
}

int SupportEnhancedEdge::columns(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	if (row > 0 && row < supports() + 1)
		return 1;
	return MultipleVariantsEdge::columns(row == 0 ? row : row - supports(), edge, manager);
}

int SupportEnhancedEdge::childCount(const ModelEdge& edge, ResourceManager& manager) const
{
	return supports() + MultipleVariantsEdge::childCount(edge, manager);
}

std::pair<int, int> SupportEnhancedEdge::child(int index, const ModelEdge& edge, ResourceManager& manager) const
{
	if (index < supports())
		return {index + 1, 0};
	std::pair<int, int> child_index = MultipleVariantsEdge::child(index - supports(), edge, manager);
	child_index.first += supports();
	return child_index;
}

int SupportEnhancedEdge::childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	if (row > 0 && row < supports() + 1)
	{
		return row - 1;
	}
	auto index = MultipleVariantsEdge::childIndex(row == 0 ? row : row - supports(), column, edge, manager);
	return index == MODELEDGE_INVALID ? MODELEDGE_INVALID : supports() + index;
}

ModelEdge SupportEnhancedEdge::child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	if (row > 0 && row < supports() + 1)
	{
		return ModelEdge(edge, edge.project(), edge.file(), row, 0, edge.subindex(), edge.childItem<hkVariant>(), supportType(row - 1));
	}
	auto child_edge = MultipleVariantsEdge::child(row == 0 ? row : row - supports(), column, edge, manager);
	child_edge.addSupport(supports());
	return child_edge;
}

QVariant SupportEnhancedEdge::data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const
{
	if (row > 0 && row < supports() + 1 && column == 0)
	{
		return supportName(row - 1);
	}
	return MultipleVariantsEdge::data(row == 0 ? row : row - supports(), column, edge, manager);
}

std::pair<int, int> SupportEnhancedEdge::dataStart() const
{
	return { 1 + supports(), 1 };
}

bool SupportEnhancedEdge::setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager)
{
	if (row > 0 && row < supports() + 1 && column == 0)
	{
		return false;
	}
	return MultipleVariantsEdge::setData(row == 0 ? row : row - supports(), column, edge, data, manager);
}

bool SupportEnhancedEdge::addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	if (row_start > 0 && row_start < supports() + 1)
	{
		return false;
	}
	return MultipleVariantsEdge::addRows(row_start == 0 ? row_start : row_start - supports(), count, edge, manager);
}

bool SupportEnhancedEdge::removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager)
{
	if (row_start > 0 && row_start < supports() + 1)
	{
		return false;
	}
	return MultipleVariantsEdge::removeRows(row_start == 0 ? row_start : row_start - supports(), count, edge, manager);
}

bool SupportEnhancedEdge::changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager)
{
	if (row > 0 && row < supports() + 1)
	{
		return false;
	}
	return MultipleVariantsEdge::changeColumns(row == 0 ? row : row - supports(), column_start, delta, edge, manager);
}

TypeInfo SupportEnhancedEdge::rowClass(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	if (row > 0 && row < supports() + 1)
	{
		return { hkClassMember::Type::TYPE_VOID, nullptr };
	}
	return MultipleVariantsEdge::rowClass(row == 0 ? row : row - supports(), edge, manager);
}

bool SupportEnhancedEdge::isArray(int row, const ModelEdge& edge, ResourceManager& manager) const
{
	if (row > 0 && row < supports() + 1)
	{
		return false;
	}
	return MultipleVariantsEdge::isArray(row == 0 ? row : row - supports(), edge, manager);
}