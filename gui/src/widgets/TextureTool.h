#pragma once

#include <src/models/NIFTreeModel.h>

#include <QAction>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

#include "ui_TextureTool.h"
#include <memory>

class TextureTool : public QDialog, private Ui::TextureTool
{
	Q_OBJECT

	std::shared_ptr<ckcmd::HKX::NIFTreeModel> _model;
	std::shared_ptr<QSortFilterProxyModel> _proxyModel;
	static QString _lastOpenedFolder;
	QAction* _visibleAction;

	void populateValues(const QModelIndex& current);

	void SetShaderType(Niflib::BSLightingShaderPropertyRef property);
	void SetTextures(Niflib::BSLightingShaderPropertyRef property);
	void SetFlags(Niflib::BSLightingShaderPropertyRef property);

private slots:

	void on_folderSelectButton_clicked(bool checked);
	void on_folderLineEdit_textChanged(const QString& text);
	void on_nifView_selectionChanged(const QModelIndex& current, const QModelIndex& previous);
	void on_viewModeCheckBox_toggled(bool checked);

public:

	TextureTool(QWidget* parent = NULL);

	QAction* toggleViewAction();

	virtual void accept() override;
};