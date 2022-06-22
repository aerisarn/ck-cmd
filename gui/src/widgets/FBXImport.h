#pragma once

#include <src/models/FBXTreeModel.h>

#include "ui_FBXimport.h"
#include <memory>

class FBXImport : public QDialog, private Ui::FBXImport
{
	Q_OBJECT

	std::shared_ptr<ckcmd::HKX::FBXTreeModel> _model;
	QString _lastOpenedFolder;
	fs::path _selection;

private slots:

	void on_pathSelectButton_clicked(bool checked);
	void on_pathLineEdit_textChanged(const QString& text);

public:

	FBXImport(QWidget* parent = NULL);
	fs::path selected() { return _selection; }

	static fs::path getPath(
		QWidget* parent,
		bool* ok);
};