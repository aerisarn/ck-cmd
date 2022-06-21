#include "AnimationChooser.h"

#include <QSortFilterProxyModel>

AnimationChooser::AnimationChooser(QAbstractItemModel* model, QWidget* parent) :
	QDialog(parent)
{
	setupUi(this);

	QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
	model->setHeaderData(0, Qt::Horizontal, "Name", Qt::DisplayRole);
	proxyModel->setSourceModel(model);
	proxyModel->setHeaderData(0, Qt::Horizontal, "Name", Qt::DisplayRole);
	tableView->setModel(proxyModel);
	proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	connect(listFilterLineEdit, &QLineEdit::textChanged, proxyModel, &QSortFilterProxyModel::setFilterFixedString);
}

void AnimationChooser::accept()
{
	_results.clear();
	if (tableView->currentIndex().isValid())
	{
		auto& clipIndexList = tableView->selectionModel()->selectedIndexes();
		for (auto& index : clipIndexList)
			_results << index.data().toString();
	}
	QDialog::accept();
}

QStringList AnimationChooser::getIndex(
	QAbstractItemModel* viewModel,
	QWidget* parent,
	bool* ok)
{
	std::unique_ptr<AnimationChooser> dialog = std::make_unique<AnimationChooser>(viewModel, parent);
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