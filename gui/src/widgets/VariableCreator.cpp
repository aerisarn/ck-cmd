#include "VariableCreator.h"

#include <QSortFilterProxyModel>

VariableCreator::VariableCreator(QWidget* parent) :
	QDialog(parent)
{
	setupUi(this);
	_results = { "", VariableType::type_invalid };
}

void VariableCreator::accept()
{
	_results = { 
		variableNameLineEdit->text(),
		static_cast<VariableType>(variableTypeComboBox->currentIndex())
	};
	QDialog::accept();
}

std::pair<QString, VariableCreator::VariableType> VariableCreator::getNewVariable(
	QWidget* parent,
	bool* ok)
{
	std::unique_ptr<VariableCreator> dialog = std::make_unique<VariableCreator>(parent);
	const int ret = dialog->exec();
	if (ok)
		*ok = !!ret;
	if (ret) {
		return dialog->selected();
	}
	else {
		return {};
	}
}