#include "FBXImport.h"

#include <QFileDialog>

FBXImport::FBXImport(QWidget* parent) :
	_model(nullptr),
	_lastOpenedFolder("")
{
	setupUi(this);
}

void FBXImport::on_pathSelectButton_clicked(bool checked)
{
	auto fileName = QFileDialog::getOpenFileName(this,
		tr("Open FBX File"), _lastOpenedFolder, tr("FBX Files (*.fbx)"));
	if (!fileName.isEmpty())
	{
		pathLineEdit->setText(fileName);
		fs::path folder = fs::path(fileName.toUtf8().constData()).parent_path();
		_lastOpenedFolder = folder.string().c_str();
	}
}

void FBXImport::on_pathLineEdit_textChanged(const QString& text)
{
	std::string std_text = text.toUtf8().constData();
	if (fs::exists(std_text) && fs::is_regular_file(std_text))
	{
		_model = std::make_shared< ckcmd::HKX::FBXTreeModel>(std_text);
		FbxTreeView->setModel(_model.get());
	}
}