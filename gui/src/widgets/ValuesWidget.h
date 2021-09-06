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

    ckcmd::HKX::CommandManager& _command_manager;
    const ckcmd::HKX::ResourceManager & _manager;
    Ui::ValuesWidget* ui;

public:
    explicit ValuesWidget(ckcmd::HKX::CommandManager& command_manager, const ckcmd::HKX::ResourceManager& manager, QWidget* parent = 0);
    ~ValuesWidget();

public slots:
    void setVariant(int file_index, hkVariant* v, hkVariant* parent);
    void modelHasSetNewHkxItemPointer(ckcmd::HKX::HkxItemPointer old_value, ckcmd::HKX::HkxItemPointer new_value, int file, hkVariant* variant);

signals:
    void HkxItemPointerChanged(ckcmd::HKX::HkxItemPointer old_value, ckcmd::HKX::HkxItemPointer new_value, int file, hkVariant* variant);
};

#endif //VALUESWIDGET_H