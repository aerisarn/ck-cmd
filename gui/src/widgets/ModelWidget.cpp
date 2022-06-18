#include "ModelWidget.h"

#include <QItemDelegate>
#include <QMetaObject>
#include <QMetaProperty>

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
		bind(std::get<0>(binding), std::get<1>(binding), std::get<2>(binding), std::get<3>(binding));
	}
}

void ModelWidget::doPropertyChange(int row, int column, QString value)
{
	auto index = _model.index(row, column, _index);
	_model.setData(index, value, Qt::EditRole);
}

void ModelWidget::bind(QWidget* widget, const QByteArray& property_name, size_t row_index, size_t column_index)
{
	QByteArray _property_name = property_name;
	auto meta_sender = widget->metaObject();
	auto meta_receiver = _widget_signal_map.metaObject();
	if (_property_name.isEmpty())
		_property_name = meta_sender->userProperty().name();
	const int index = meta_sender->indexOfProperty(_property_name);
	if (index != -1)
	{
		const QMetaProperty to_bind_property = meta_sender->property(index);
		if (to_bind_property.hasNotifySignal()) {
			const QMetaMethod notifySignal = to_bind_property.notifySignal();
			auto receive_slot_index = meta_receiver->indexOfSlot(QMetaObject::normalizedSignature("receiveValue(QString)"));
			const QMetaMethod receiveSlot = meta_receiver->method(receive_slot_index);
			//debug
			QString sig = QString("2%1").arg(notifySignal.methodSignature().constData());
			bool ok = connect(widget, notifySignal, &_widget_signal_map, receiveSlot);
			if (!ok)
				__debugbreak();
		}
	}
	_widget_signal_map.addMapping(widget, row_index, column_index);
	_bindings.insert({ {row_index, column_index}, {widget, _property_name} });
	populate(row_index, column_index, widget, _property_name);
}

void ModelWidget::bind(QWidget* widget, size_t row_index, size_t column_index)
{
	bind(widget, {}, row_index, column_index);
}

void ModelWidget::populate(int row_index, int column_index, QWidget* widget, const QByteArray& propertyName)
{
	QModelIndex rowIndex = _model.index(row_index, dataBindingColumnStart() + column_index, _index);
	int values = _model.rowColumns(rowIndex);
	QVariant value;
	QModelIndex valueIndex = _model.index(row_index, dataBindingColumnStart() + column_index, _index);
	value = valueIndex.data();
	if (propertyName.isEmpty())
	{
		_delegate->setEditorData(widget, valueIndex);
	}
	else {
		widget->setProperty(propertyName, value);
	}
}

void ModelWidget::populate(int row, int column)
{
	auto widgets = _bindings.equal_range({ row, column });
	for (auto widget_it = widgets.first; widget_it != widgets.second; widget_it++)
	{
		auto* widget = widget_it->second.first;
		auto& property_name = widget_it->second.second;
		populate(row, column, widget, property_name);
	}
}

void ModelWidget::populate()
{
	for (auto widget_it = _bindings.begin(); widget_it != _bindings.end(); widget_it++)
	{
		auto index = widget_it->first;
		auto* widget = widget_it->second.first;
		auto& property_name = widget_it->second.second;
		populate(index.first, index.second, widget, property_name);
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
			bool ok = connect(&_widget_signal_map, &ModelWidgetSignalMapper::sendValue, this, &ModelWidget::doPropertyChange);
			if (!ok)
				__debugbreak();
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
	bind(widget, memberModelRow(memberName), 0);
}
void ModelWidget::bind(QWidget* widget, const QByteArray& property_name, const QString& memberName, int column_index)
{
	bind(widget, property_name, memberModelRow(memberName), column_index);
}