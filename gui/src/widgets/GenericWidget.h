#pragma once

#include <src/widgets/ModelWidget.h>

#include <QTableView>
#include <QtWidgets/QVBoxLayout>

class GenericWidget : public ckcmd::ModelWidget
{
	QVBoxLayout* verticalLayout;

	QTableView* makeFieldWidget
	(
		const QString& labelText,
		const std::vector<size_t>& rows,
		const std::vector<QString>& columnLabels
	);

public:
	GenericWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent = 0);

	StaticBindingTable dataBindingtable = {
	};

	virtual const StaticBindingTable& bindingTable() const override
	{
		return dataBindingtable;
	}

	virtual void OnIndexSelected() override;
};