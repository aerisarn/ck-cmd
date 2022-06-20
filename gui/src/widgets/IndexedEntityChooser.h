#pragma once

#include <QDialog>

//#include <src/widgets/ModelDialog.h>
//#include <src/models/SelectionProxyModel.h>

#include "ui_IndexedEntityChooser.h"


class IndexedEntityChooser : public QDialog, private Ui::IndexedEntityChooser
{
	QModelIndex _result;

public Q_SLOTS:
    virtual void accept() override;

public:
	IndexedEntityChooser(QAbstractItemModel* viewModel, QWidget* parent = 0);
	QModelIndex selectedIndex() const {return _result;}

	static QModelIndex getIndex(
		QAbstractItemModel* viewModel,
		QWidget* parent,
		const QString& title,
		const QString& selectLabel,
		const QString& createLabel,
		bool* ok);

};