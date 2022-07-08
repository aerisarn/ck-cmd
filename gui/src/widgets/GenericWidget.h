#pragma once

#include <src/widgets/ModelWidget.h>

#include <QTableView>
#include <QtWidgets/QVBoxLayout>

class GenericWidget : public ckcmd::ModelWidget
{
	QVBoxLayout* verticalLayout;

	void makeFieldWidget
	(
		const QString& labelText,
		const std::vector<size_t>& rows,
		const std::vector<QString>& columnLabels,
		bool isArray
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