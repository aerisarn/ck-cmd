#pragma once

#include <src/widgets/ModelWidget.h>

#include "ui_HkTopNavigatorWidget.h"

class hkbNode;

class TopInfoWidget : public ckcmd::ModelWidget, private Ui::HkTopNavigator
{
    Q_OBJECT

private slots:
    void on_nameLineEdit_textChanged(const QString& text);
//    void resizeNameToContent(const QString& text);
//    void on_treeView_doubleClicked(const QModelIndex& index);
//    void on_treeView_customContextMenuRequested(const QPoint& pos);
//    void on_treeView_selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

public:
    explicit TopInfoWidget(ckcmd::HKX::ProjectTreeModel& model, QWidget* parent = 0);

    void setIndex(const QModelIndex& index);

    QSize sizeHint() const override;

private:

    QModelIndex _selected;
};