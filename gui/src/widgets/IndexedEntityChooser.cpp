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
	else {
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

