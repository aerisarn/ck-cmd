#pragma once

#include <QWidget>
#include <src/models/ProjectModel.h>

namespace ckcmd {

	class ModelWidget : public QWidget
	{
	protected:
		HKX::ProjectModel& _model;
		QModelIndex _index;
	
		virtual void OnIndexSelected() = 0;

	public:
		ModelWidget(HKX::ProjectModel& model, QWidget* parent = 0);

		void setIndex(const QModelIndex& index);
	};
}