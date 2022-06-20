#include "ModelDialog.h"

using namespace ckcmd;
using namespace ckcmd::HKX;

ModelDialog::ModelDialog(HKX::ProjectModel& model, const QModelIndex& index, QWidget* parent) :
	_model(model),
	_project_index(model.getProjectIndex(index)),
	_file_index(model.getFileIndex(index)),
	QDialog(parent)
{
}