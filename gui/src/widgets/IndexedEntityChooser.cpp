#include "IndexedEntityChooser.h"

//using namespace ckcmd;
//using namespace ckcmd::HKX;

IndexedEntityChooser::IndexedEntityChooser(QAbstractItemModel* model, QWidget* parent) :
	QDialog(parent)
{
	setupUi(this);

	listView->setModel(model);
	listView->setFocus();
	listView->setCurrentIndex(model->index(0, 0, QModelIndex()));
}

void IndexedEntityChooser::accept()
{
	if (selectRadioButton->isChecked() && listView->currentIndex().isValid())
	{
		_result = listView->currentIndex();
	}
	else 
	{
		if (!lineEdit->text().isEmpty())
		{
			int new_row_index = listView->model()->rowCount();
			if (listView->model()->insertRow(new_row_index)) {
				_result = listView->model()->index(new_row_index, 0);
				listView->model()->setData(_result, lineEdit->text());
			}
			else {
				//error can't insert
			}
		}
		else {
			//error name empty
		}
	}
	QDialog::accept();
}

QModelIndex IndexedEntityChooser::getIndex(
	QAbstractItemModel* viewModel,
	QWidget* parent,
	const QString& title,
	const QString& selectLabel,
	const QString& createLabel,
	bool* ok)
{
	std::unique_ptr<IndexedEntityChooser> dialog = std::make_unique<IndexedEntityChooser>(viewModel, parent);
	dialog->setWindowTitle(title);
	dialog->selectRadioButton->setText(selectLabel);
	dialog->createRadioButton->setText(createLabel);
	const int ret = dialog->exec();
	if (ok)
		*ok = !!ret;
	if (ret) {
		return dialog->selectedIndex();
	}
	else {
		return QModelIndex();
	}
}
