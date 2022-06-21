#include "AttackClipChooser.h"

AttackClipChooser::AttackClipChooser(QAbstractItemModel* attackEventModel, QAbstractItemModel* clipModel, QWidget* parent) :
	QDialog(parent)
{
	setupUi(this);

	attackEventsListView->setModel(attackEventModel);
	clipListView->setModel(clipModel);
}

void AttackClipChooser::accept()
{
	if (attackEventsListView->currentIndex().isValid() &&
		clipListView->currentIndex().isValid())
	{
		_choosen_clip.clear();
		_choosen_attack_event = attackEventsListView->currentIndex().data().toString();
		auto& clipIndexList = clipListView->selectionModel()->selectedIndexes();
		for (auto& index : clipIndexList)
			_choosen_clip << index.data().toString();
	}
	QDialog::accept();
}

std::pair<QString, QStringList> AttackClipChooser::getResult(
	QAbstractItemModel* attackEventModel,
	QAbstractItemModel* clipModel,
	QWidget* parent,
	bool* ok)
{
	std::unique_ptr<AttackClipChooser> dialog = std::make_unique<AttackClipChooser>(attackEventModel, clipModel, parent);
	const int ret = dialog->exec();
	if (ok)
		*ok = !!ret;
	if (ret) {
		return dialog->selection();
	}
	else {
		return {};
	}
}
