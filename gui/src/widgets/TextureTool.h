#pragma once

#include <src/models/NIFTreeModel.h>

#include <QAction>
#include <QItemSelectionMOdel>

#include "ui_TextureTool.h"
#include <memory>

class TextureTool : public QDialog, private Ui::TextureTool
{
	Q_OBJECT

	std::shared_ptr<ckcmd::HKX::NIFTreeModel> _model;
	static QString _lastOpenedFolder;
	QAction* _visibleAction;

	void populateValues(const QModelIndex& current);

private slots:

	void on_folderSelectButton_clicked(bool checked);
	void on_folderLineEdit_textChanged(const QString& text);
	void on_nifView_selectionChanged(const QModelIndex& current, const QModelIndex& previous);

public:

	TextureTool(QWidget* parent = NULL);

	QAction* toggleViewAction();
};