#include "GenericWidget.h"
#include <src/models/ValuesProxyModel.h>
#include <src/hkx/ItemsDelegate.h>

#include <QSpacerItem>
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
	verticalLayout->setAlignment(Qt::AlignTop);
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
	tableView->setMaximumHeight(rowTotalHeight);
}

QTableView* GenericWidget::makeFieldWidget
(
	const QString& labelText, 
	const std::vector<size_t>& rows, 
	const std::vector<QString>& columnLabels
)
{
	QLabel* label = new QLabel(this);
	label->setText(labelText);
	label->setMaximumHeight(23);

	verticalLayout->addWidget(label);

	QTableView* editor = new QTableView(this);
	editor->setSelectionMode(QAbstractItemView::NoSelection);
	editor->horizontalHeader()->setVisible(false);
	editor->horizontalHeader()->setStretchLastSection(true);
	editor->verticalHeader()->setMinimumSectionSize(20);
	editor->verticalHeader()->setDefaultSectionSize(20);
	editor->verticalHeader()->setVisible(false);
	editor->setShowGrid(false);
	editor->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	ValuesProxyModel* editModel = new ValuesProxyModel(&_model, rows, 1, _index, this);
	if (rows.size() > 1)
	{
		for (size_t i = 0; i < columnLabels.size(); ++i)
			editModel->setHeaderData(i, Qt::Horizontal, columnLabels[i]);
	}
	editor->setItemDelegate(new ItemsDelegate(*editModel, this));
	editor->setModel(editModel);
	editor->resizeRowsToContents();
	if (rows.size() > 1)
	{
		//editor->horizontalHeader()->setVisible(true);
		editor->resizeColumnsToContents();
	}

	verticalResizeTableViewToContents(editor);
	return editor;
}

void GenericWidget::OnIndexSelected()
{
	buildReflectionTable();
	clearLayout(verticalLayout);

	QString last_group_name;
	std::vector<size_t> last_group;
	std::vector<QString> last_group_fields;

	for (const auto& member : _members)
	{
		int row = member.second.first;
		QModelIndex row_index = _model.index(row, 0, _index);
		auto type = _model.rowType(row_index);
		if (type != nullptr)
			continue;
		if (std::get<0>(member) == "name")
			continue;

		auto dot_index = std::get<0>(member).indexOf(".");
		if (dot_index != -1)
		{
			//this is an object;
			last_group.push_back(member.second.first);
			last_group_name = member.first.left(dot_index);
			last_group_fields.push_back(member.first.mid(dot_index));
			continue;
		}
		else {
			if (!last_group.empty())
			{
				QTableView* editor = makeFieldWidget(last_group_name, last_group, last_group_fields);
				verticalLayout->addWidget(editor);
				last_group.clear();
				last_group_fields.clear();
			}
			QTableView* editor = makeFieldWidget(member.first, { member.second.first }, last_group_fields);
			verticalLayout->addWidget(editor);
		}
	}
	if (!last_group.empty())
	{
		QTableView* editor = makeFieldWidget(last_group_name, last_group, last_group_fields);
		verticalLayout->addWidget(editor);
		last_group.clear();
		last_group_fields.clear();
	}
}