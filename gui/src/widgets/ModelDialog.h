#pragma once

#include <src/models/ProjectModel.h>

#include <QDialog>

namespace ckcmd {

	class ModelDialog : public QDialog
	{
		Q_OBJECT

	protected:
		HKX::ProjectModel& _model;
		QModelIndex _index;
		int _project_index;
		int _file_index;

	public:
		ModelDialog(HKX::ProjectModel& model, const QModelIndex& index, QWidget* parent = 0);

	};
}