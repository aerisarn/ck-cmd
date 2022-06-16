#pragma once

#include <src/models/FBXTreeModel.h>

#include "ui_FBXimport.h"
#include <memory>

class FBXImport : public QWidget, private Ui::FBXImport
{
	Q_OBJECT

	std::shared_ptr<ckcmd::HKX::FBXTreeModel> _model;
	QString _lastOpenedFolder;

private slots:

	void on_pathSelectButton_clicked(bool checked);
	void on_pathLineEdit_textChanged(const QString& text);
	void on_cancelButton_clicked(bool checked);

public:

	FBXImport(QWidget* parent = NULL);

};