#pragma once

#include <src/widgets/ModelWidget.h>

#include <QtWidgets/QVBoxLayout>

class GenericWidget : public ckcmd::ModelWidget
{
	QVBoxLayout* verticalLayout;

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