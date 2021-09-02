#ifndef VALUESWIDGET_H
#define VALUESWIDGET_H

#include "DockWidget.h"
#include <QItemDelegate>
#include <src/models/HkxItemTableModel.h>
#include <src/hkx/ResourceManager.h>
#include <src/log.h>


QT_BEGIN_NAMESPACE
namespace Ui {
    class ValuesWidget;
}
QT_END_NAMESPACE

class ValuesWidget : public ::ads::CDockWidget
{
    Q_OBJECT

   const ckcmd::HKX::ResourceManager & _manager;
public:
    explicit ValuesWidget(const ckcmd::HKX::ResourceManager& manager, QWidget* parent = 0);
    ~ValuesWidget();

public slots:
    void setVariant(int file_index, hkVariant* v, hkVariant* parent);
    void modelHasSetNewHkxItemPointer(ckcmd::HKX::HkxItemPointer old_value, ckcmd::HKX::HkxItemPointer new_value, int file, hkVariant* variant);

signals:
    void HkxItemPointerChanged(ckcmd::HKX::HkxItemPointer old_value, ckcmd::HKX::HkxItemPointer new_value, int file, hkVariant* variant);

private:
    Ui::ValuesWidget* ui;
};

#endif //VALUESWIDGET_H