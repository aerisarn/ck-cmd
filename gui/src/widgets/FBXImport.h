#pragma once

#include <src/models/FBXTreeModel.h>

#include "ui_FBXimport.h"
#include <memory>

class FBXImport : public QDialog, private Ui::FBXImport
{
	Q_OBJECT

	std::shared_ptr<ckcmd::HKX::FBXTreeModel> _model;
	static QString _lastOpenedFolder;
	static QString _lastImportFolder;
	fs::path _selection;
	fs::path _destination;

private slots:

	void on_pathSelectButton_clicked(bool checked);
	void on_destinationPathSelectButton_clicked(bool checked);
	void on_pathLineEdit_textChanged(const QString& text);

public:

	FBXImport(const fs::path& destination_folder, QWidget* parent = NULL);
	std::pair<fs::path, fs::path> selected() { return { _selection, _destination }; }

	static std::pair<fs::path, fs::path> getPath(
		const fs::path& path,
		QWidget* parent,
		bool* ok);
};