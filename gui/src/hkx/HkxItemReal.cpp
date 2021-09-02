#include "HkxItemReal.h"
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPainter>
#include <QStylePainter>
#include <QDoubleSpinBox>

using namespace ckcmd::HKX;

QWidget* HkxItemReal::HkxItemRealTemplateHolder::getTemplate(size_t rows, size_t columns) {
	auto it = _templates.find({ rows, columns });
	if (it == _templates.end())
	{
		auto _template = new QWidget();
		QGridLayout* _layout = new QGridLayout(_template);
		for (size_t row = 0; row < rows; row++)
		{
			for (size_t column = 0; column < columns; column++)
			{
				QLabel* label = new QLabel("NAN");
				label->setObjectName(QString("%1,%2").arg(row).arg(column));
				_layout->addWidget(
					label,
					row,
					column
				);
			}
		}
		_template->adjustSize();
		_templates[{ rows, columns }] = _template;
		return _template;
	}
	return it->second;
}

HkxItemReal::HkxItemRealTemplateHolder::~HkxItemRealTemplateHolder() {
	for (auto& item : _templates)
	{
		delete item.second;
	}
}


QWidget* HkxItemReal::fillTemplate() const
{
	size_t rows = _value.size();
	size_t columns = _value.at(0).size();
	QWidget* template_ = HkxItemRealTemplateHolder::GetInstance().getTemplate(rows, columns);
	for (size_t row = 0; row < rows; row++)
	{
		for (size_t column = 0; column < columns; column++)
		{
			QLabel* label = template_->findChild<QLabel*>(QString("%1,%2").arg(row).arg(column));
			QString s = QString::number(_value.at(row).at(column), 'f', 4);
			label->setText(s);
		}
	}
	template_->adjustSize();
	return template_;
}

HkxItemReal::HkxItemReal(const std::vector<std::vector<float>>& value) : _value(value)
{
}

float HkxItemReal::value(size_t row, size_t column)
{
	return _value.at(row).at(column);
}

void HkxItemReal::setValue(size_t row, size_t column, float value)
{
	_value[row][column] = value;
}

QSize HkxItemReal::WidgetSizeHint(const QFontMetrics& metrics) const
{
	return fillTemplate()->size();
}

void HkxItemReal::paint(QPainter* painter, const QStyleOptionViewItem& option) const
{
	painter->save();
	painter->translate(option.rect.topLeft());
	fillTemplate()->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);
	painter->restore();
}

QWidget* HkxItemReal::CreateEditor(QWidget* parent) const
{
	size_t rows = _value.size();
	size_t columns = _value.at(0).size();
	auto _template = new QWidget(parent);
	_template->setAutoFillBackground(true);
	QGridLayout* _layout = new QGridLayout(_template);
	for (size_t row = 0; row < rows; row++)
	{
		for (size_t column = 0; column < columns; column++)
		{
			QDoubleSpinBox* cell_editor = new QDoubleSpinBox();
			cell_editor->setMaximum(std::numeric_limits<int>::max());
			cell_editor->setMinimum(std::numeric_limits<int>::min());
			cell_editor->setDecimals(4);
			cell_editor->setObjectName(QString("%1,%2").arg(row).arg(column));
			_layout->addWidget(
				cell_editor,
				row,
				column
			);
		}
	}
	_template->adjustSize();
	return _template;
}

void HkxItemReal::FillEditor(QWidget* editor) const
{
	size_t rows = _value.size();
	size_t columns = _value.at(0).size();
	for (size_t row = 0; row < rows; row++)
	{
		for (size_t column = 0; column < columns; column++)
		{
			QDoubleSpinBox* label = editor->findChild<QDoubleSpinBox*>(QString("%1,%2").arg(row).arg(column));
			auto this_value = _value.at(row).at(column);
			label->setValue(this_value);
		}
	}
	editor->adjustSize();
}

void HkxItemReal::FillFromEditor(QWidget* editor)
{
	size_t rows = _value.size();
	size_t columns = _value.at(0).size();
	for (size_t row = 0; row < rows; row++)
	{
		for (size_t column = 0; column < columns; column++)
		{
			QDoubleSpinBox* label = editor->findChild<QDoubleSpinBox*>(QString("%1,%2").arg(row).arg(column));
			_value[row][column] = label->value();
		}
	}
}