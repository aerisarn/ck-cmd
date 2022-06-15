#pragma once

#include <QWidget>
#include <src/models/ProjectTreeModel.h>

namespace ckcmd {

	class ModelWidget : public QWidget
	{
	protected:
		HKX::ProjectTreeModel& _model;
	public:

		ModelWidget(HKX::ProjectTreeModel& model, QWidget* parent = 0);
	};
}