#include "HkxItemFlags.h"
#include <QLayout>
#include <QCheckBox>
#include <QPainter>
#include <QStylePainter>

using namespace ckcmd::HKX;

HkxItemFlags::Templates HkxItemFlags::_templates;

QWidget* HkxItemFlags::getTemplate() const
{
	if (_storage == 1)
		return _templates.eight_bit_widget;
	if (_storage == 2)
		return _templates.sixteen_bit_widget;
	return _templates.thirtytwo_bit_widget;
}

QString HkxItemFlags::flag_literal(size_t bit_index) const
{
	char* name = new char[256];
	_enum_class->getNameOfValue(bit_index, (const char**)&name);
	return QString::fromStdString(name);
}


void HkxItemFlags::fillTemplate() const
{
	QWidget* template_ = getTemplate();
	for (int i = 0; i < _storage * 8; i++) {
		QCheckBox* checkbox = template_->findChild<QCheckBox*>(QString("%1").arg(i));
		if (i < _enum_class->getNumItems()) {
			QString name = _enum_class->getItem(i).getName();
			checkbox->setText(name);
		}
		else {
			checkbox->setText(QString("Unknown %1").arg(i));
		}
		if (_value & (1 << i))
		{
			checkbox->setChecked(true);
		}
		else {
			checkbox->setChecked(false);
		}
	}
	template_->resize(template_->minimumSizeHint());
}

HkxItemFlags::HkxItemFlags(int value, const hkClassEnum* enum_class, size_t storage) : 
	_value(value), 
	_enum_class(enum_class),
	_storage(storage)
{
	if (!_templates.initialized)
	{
		_templates.initialized = true;
		_templates.InitTemplates();
	}
}

int HkxItemFlags::value() { return _value; }
void HkxItemFlags::setValue(int value) { _value = value; }


QString HkxItemFlags::value_literal()
{
	char* name = new char[256];
	_enum_class->getNameOfValue(_value, (const char**)&name);
	return QString::fromStdString(name);
}

QStringList HkxItemFlags::enumValues() {
	QStringList results;
	for (int i = 0; i < _enum_class->getNumItems(); i++)
	{
		char* name = new char[256];
		_enum_class->getNameOfValue(i, (const char**)&name);
		results.push_back(QString::fromStdString(name));
	}
	return results;
}

QSize HkxItemFlags::WidgetSizeHint(const QFontMetrics& metrics) const
{
	fillTemplate();
	QSize result = getTemplate()->size();
	return result;
}

void HkxItemFlags::paint(QPainter* painter, const QStyleOptionViewItem& option) const
{
	fillTemplate();
	painter->save();
	painter->translate(option.rect.topLeft());
	getTemplate()->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);
	painter->restore();
}


void HkxItemFlags::Templates::InitTemplates() {

	eight_bit_widget = new QWidget();
	sixteen_bit_widget = new QWidget();
	thirtytwo_bit_widget = new QWidget();

	QVBoxLayout* eight_layout = new QVBoxLayout(eight_bit_widget);
	QVBoxLayout* sixteen_layout = new QVBoxLayout(sixteen_bit_widget);
	QVBoxLayout* thirtytwo_layout = new QVBoxLayout(thirtytwo_bit_widget);

	QCheckBox* checkbox;
	for (int i = 0; i < 8; i++)
	{
		checkbox = new QCheckBox(QString("Unknown %1").arg(i));
		checkbox->setObjectName(QString("%1").arg(i));
		eight_layout->addWidget(checkbox);
		checkbox = new QCheckBox(QString("Unknown %1").arg(i));
		checkbox->setObjectName(QString("%1").arg(i));
		sixteen_layout->addWidget(checkbox);
		checkbox = new QCheckBox(QString("Unknown %1").arg(i));
		checkbox->setObjectName(QString("%1").arg(i));
		thirtytwo_layout->addWidget(checkbox);
	}
	for (int i = 8; i < 16; i++)
	{
		checkbox = new QCheckBox(QString("Unknown %1").arg(i));
		checkbox->setObjectName(QString("%1").arg(i));
		sixteen_layout->addWidget(checkbox);
		checkbox = new QCheckBox(QString("Unknown %1").arg(i));
		checkbox->setObjectName(QString("%1").arg(i));
		thirtytwo_layout->addWidget(checkbox);
	}
	for (int i = 16; i < 32; i++)
	{
		checkbox = new QCheckBox(QString("Unknown %1").arg(i));
		checkbox->setObjectName(QString("%1").arg(i));
		thirtytwo_layout->addWidget(checkbox);
	}

	eight_bit_widget->setAttribute(Qt::WA_DontShowOnScreen, true);
	eight_bit_widget->show();
	sixteen_bit_widget->setAttribute(Qt::WA_DontShowOnScreen, true);
	sixteen_bit_widget->show();
	thirtytwo_bit_widget->setAttribute(Qt::WA_DontShowOnScreen, true);
	thirtytwo_bit_widget->show();
}
