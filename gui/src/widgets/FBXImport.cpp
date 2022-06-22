#include "FBXImport.h"

#include <QFileDialog>

QString FBXImport::_lastImportFolder = "";
QString FBXImport::_lastOpenedFolder = "";

FBXImport::FBXImport(const fs::path& destination_folder, QWidget* parent) :
	_model(nullptr)
{
	setupUi(this);

	if (_lastImportFolder.isEmpty())
		_lastImportFolder = destination_folder.string().c_str();
	
	destinationPathLineEdit->setText(_lastImportFolder);
	_destination = _lastImportFolder.toUtf8().constData();
}

void FBXImport::on_pathSelectButton_clicked(bool checked)
{
	auto fileName = QFileDialog::getOpenFileName(this,
		tr("Open FBX File"), _lastOpenedFolder, tr("FBX Files (*.fbx)"));
	if (!fileName.isEmpty())
	{
		pathLineEdit->setText(fileName);
		_selection = fileName.toUtf8().constData();
		fs::path folder = fs::path(_selection).parent_path();
		_lastOpenedFolder = folder.string().c_str();
	}
}

void FBXImport::on_destinationPathSelectButton_clicked(bool checked)
{

	auto dir = QFileDialog::getExistingDirectory(this,
		tr("Select destination folder"), _lastImportFolder);
	if (!dir.isEmpty())
	{
		destinationPathLineEdit->setText(dir);
		fs::path folder = fs::path(dir.toUtf8().constData());
		_lastImportFolder = folder.string().c_str();
		_destination = folder;
		destinationPathLineEdit->setText(_lastImportFolder);
	}
}

void FBXImport::on_pathLineEdit_textChanged(const QString& text)
{
	std::string std_text = text.toUtf8().constData();
	if (fs::exists(std_text) && fs::is_regular_file(std_text))
	{
		_model = std::make_shared< ckcmd::HKX::FBXTreeModel>(std_text);
		FbxTreeView->setModel(_model.get());
		FbxTreeView->setVisible(false);
		FbxTreeView->resizeColumnToContents(0);
		FbxTreeView->resizeColumnToContents(1);
		FbxTreeView->resizeColumnToContents(2);
		FbxTreeView->setVisible(true);
	}
}

std::pair<fs::path, fs::path> FBXImport::getPath(
	const fs::path& path,
	QWidget* parent,
	bool* ok)
{
	std::unique_ptr<FBXImport> dialog = std::make_unique<FBXImport>(path, parent);
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