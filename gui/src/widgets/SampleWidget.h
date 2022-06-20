#pragma once

#include <src/widgets/ModelWidget.h>
#include <src/models/ProjectModel.h>

#include <QWidget>

#include <ui_SampleWidget.h>

class SampleWidget : public ckcmd::ModelWidget, private Ui::SampleWidget 
{
public:
	SampleWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent = 0);

	virtual const StaticBindingTable& bindingTable() const override
	{
		return {};
	}

	virtual void OnIndexSelected() override;
};