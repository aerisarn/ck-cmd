#include "ModelWidget.h"

#include <QItemDelegate>

using namespace ckcmd;
using namespace ckcmd::HKX;

ModelWidget::ModelWidget(ProjectModel& model, QWidget* parent) :
	_model(model),
	_delegate(new QItemDelegate(this)),
	QWidget(parent)
{
}

void ModelWidget::buildReflectionTable()
{
	int rows = _model.rowCount(_index);
	int columns = 0; // _model.columnCount(_index);

	int i = dataBindingRowStart();
	int j = dataBindingColumnStart();

	for (i; i < rows; ++i)
	{
		auto row_index = _model.index(i, j, _index);
		columns = _model.rowColumns(row_index);
		_members.insert(
			{
				row_index.data().toString(),
				{i, columns}
			}
		);
	}
}

void ModelWidget::doBindings()
{
	const auto& binding_table = bindingTable();
	for (const auto& binding : binding_table)
	{
		bind(std::get<0>(binding), std::get<1>(binding), std::get<2>(binding));
	}
}

void ModelWidget::bind(QWidget* widget, const QByteArray& property_name, size_t row_index)
{
	_bindings.insert({ row_index, {widget, property_name} });
	populate(row_index, widget, property_name);
}

void ModelWidget::bind(QWidget* widget, size_t row_index)
{
	_bindings.insert({ row_index, {widget, {}} });
	populate(row_index, widget, {});
}

void ModelWidget::populate(int index, QWidget* widget, const QByteArray& propertyName)
{
	QModelIndex rowIndex = _model.index(index, dataBindingColumnStart(), _index);
	int values = _model.rowColumns(rowIndex);
	QVariant value;
	QModelIndex valueIndex = _model.index(index, dataBindingColumnStart() + 1, _index);
	if (values > 1)
	{
		QVariantList valueList;
		for (int j = dataBindingColumnStart() + 1; j < values; ++j)
		{
			QModelIndex subValueIndex = _model.index(index, j, _index);
			valueList.push_back(subValueIndex.data());
		}
		value = valueList;
	}
	else if (values == 1) {
		value = valueIndex.data();
	}
	if (propertyName.isEmpty())
	{
		_delegate->setEditorData(widget, valueIndex);
	}
	else {
		widget->setProperty(propertyName, value);
	}
}

void ModelWidget::populate(int index)
{
	auto widgets = _bindings.equal_range(index);
	for (auto widget_it = widgets.first; widget_it != widgets.second; widget_it++)
	{
		auto* widget = widget_it->second.first;
		auto& property_name = widget_it->second.second;
		populate(index, widget, property_name);
	}
}

void ModelWidget::populate()
{
	for (auto widget_it = _bindings.begin(); widget_it != _bindings.end(); widget_it++)
	{
		auto index = widget_it->first;
		auto* widget = widget_it->second.first;
		auto& property_name = widget_it->second.second;
		populate(index, widget, property_name);
	}
}

void ModelWidget::setIndex(const QModelIndex& index) 
{ 
	_index = index;
	if (doDataBinding())
	{
		if (!bindings_done)
		{
			buildReflectionTable();
			doBindings();
			bindings_done = true;
		}
		populate();
	}
	OnIndexSelected(); 
}

size_t ModelWidget::memberModelRow(const QString& memberName)
{
	return _members[memberName].first;
}

void ModelWidget::bind(QWidget* widget, const QString& memberName)
{
	bind(widget, memberModelRow(memberName));
}
void ModelWidget::bind(QWidget* widget, const QByteArray& property_name, const QString& memberName)
{
	bind(widget, property_name, memberModelRow(memberName));
}