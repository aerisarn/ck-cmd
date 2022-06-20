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
	IndexedEntityChooser(QAbstractItemModel* model, QWidget* parent = 0);
	QModelIndex selectedIndex() const {return _result;}

};