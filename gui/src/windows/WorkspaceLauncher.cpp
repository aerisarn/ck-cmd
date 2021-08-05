#include "WorkspaceLauncher.h"

#include "ui_WorkspaceLauncher.h"

#include <QFileDialog>

WorkspaceLauncher::WorkspaceLauncher(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::WorkspaceLauncher)
{
	ui->setupUi(this);
	connect(ui->selectFolderButton, &QPushButton::released, this, &WorkspaceLauncher::onSelectFolderButtonPressed);
}

void WorkspaceLauncher::onSelectFolderButtonPressed()
{
	_workspace_folder = 
		QFileDialog::getExistingDirectory
		(
			this, tr("Open Directory"),
			"",
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
		);
	ui->workspaceFolderLine->setText(_workspace_folder);
}


WorkspaceLauncher::~WorkspaceLauncher()
{
	delete ui;
}
