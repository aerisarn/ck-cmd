#include "GenericWidget.h"
#include <src/models/ValuesProxyModel.h>

#include <QtWidgets/QLabel>
#include <QTableView>
#include <QLineEdit>
#include <QHeaderView>
#include <QScrollBar>

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

void verticalResizeTableViewToContents(QTableView* tableView)
{
	int rowTotalHeight = 0;

	// Rows height
	int count = tableView->verticalHeader()->count();	
	for (int i = 0; i < count; ++i) {
		// 2018-03 edit: only account for row if it is visible
		if (!tableView->verticalHeader()->isSectionHidden(i)) {
			rowTotalHeight += tableView->verticalHeader()->sectionSize(i);
		}
	}
	if (count == 0)
		rowTotalHeight = 23;

	// Check for scrollbar visibility
	if (tableView->horizontalScrollBar()->isVisible())
	{
		rowTotalHeight += tableView->horizontalScrollBar()->height();
	}

	// Check for header visibility
	if (!tableView->horizontalHeader()->isHidden())
	{
		rowTotalHeight += tableView->horizontalHeader()->height();
	}
	tableView->setMinimumHeight(rowTotalHeight);
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

		QTableView* editor = new QTableView(this);
		editor->horizontalHeader()->setVisible(false);
		editor->horizontalHeader()->setStretchLastSection(true);
		editor->verticalHeader()->setMinimumSectionSize(20);
		editor->verticalHeader()->setDefaultSectionSize(20);
		editor->verticalHeader()->setVisible(false);
		editor->setShowGrid(false);
		editor->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
		QModelIndex data_index = _model.index(member.second.first, 1, _index);
		QString value = data_index.data().toString();
		ValuesProxyModel* editModel = new ValuesProxyModel(&_model, member.second.first, 1, _index, this);
		editor->setModel(editModel);
		editor->resizeRowsToContents();
		editor->adjustSize();

		verticalResizeTableViewToContents(editor);

		QString valuecheck = data(std::get<0>(member).toUtf8().constData(), 0).toString();

		if (value != valuecheck)
			__debugbreak();

		verticalLayout->addWidget(editor);
	}
	//this->adjustSize();
}