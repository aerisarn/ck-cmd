#include "GenericWidget.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

using namespace ckcmd;
using namespace ckcmd::HKX;

GenericWidget::GenericWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent) :
	ModelWidget(model, parent)
{
	this->resize(510, 372);
	verticalLayout = new QVBoxLayout(this);
}

void clearLayout(QLayout* layout, bool deleteWidgets = true)
{
	while (QLayoutItem* item = layout->takeAt(0))
	{
		QWidget* widget;
		if ((deleteWidgets)
			&& (widget = item->widget())) {
			delete widget;
		}
		if (QLayout* childLayout = item->layout()) {
			clearLayout(childLayout, deleteWidgets);
		}
		delete item;
	}
}

void GenericWidget::OnIndexSelected()
{
	buildReflectionTable();
	clearLayout(verticalLayout);
	for (const auto& member : _members)
	{
		QLabel* label = new QLabel(this);
		label->setText(std::get<0>(member));

		verticalLayout->addWidget(label);

		QLineEdit* value = new QLineEdit(this);
		value->setText(data(std::get<0>(member).toUtf8().constData(), 0).toString());

		verticalLayout->addWidget(value);
	}
}