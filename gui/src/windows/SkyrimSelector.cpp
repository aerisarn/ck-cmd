#include "SkyrimSelector.h"

#include "ui_SkyrimSelector.h"

#include <QFileDialog>

SkyrimSelector::SkyrimSelector(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::SkyrimSelector)
{
	ui->setupUi(this);
	connect(ui->selectFolderButton, &QPushButton::released, this, &SkyrimSelector::onSelectFolderButtonPressed);
}

void SkyrimSelector::onSelectFolderButtonPressed()
{
	_skyrim_folder =
		QFileDialog::getExistingDirectory
		(
			this, tr("Open Directory"),
			"",
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
		);
	ui->skyrimFolderLine->setText(_skyrim_folder);
}


SkyrimSelector::~SkyrimSelector()
{
	delete ui;
}
