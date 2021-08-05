#ifndef VALUESWIDGET_H
#define VALUESWIDGET_H

#include "DockWidget.h"
#include <QItemDelegate>
#include <src/models/HkxItemTableModel.h>
#include <src/log.h>


QT_BEGIN_NAMESPACE
namespace Ui {
    class ValuesWidget;
}
QT_END_NAMESPACE

class ValuesWidget : public ::ads::CDockWidget
{
    Q_OBJECT


public:
    explicit ValuesWidget(QWidget* parent = 0);
    ~ValuesWidget();

public slots:
    void setVariant(hkVariant* v);

private:
    Ui::ValuesWidget* ui;
};

#endif //VALUESWIDGET_H