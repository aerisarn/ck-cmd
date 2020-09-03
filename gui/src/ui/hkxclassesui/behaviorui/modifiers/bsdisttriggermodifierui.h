#ifndef BSDISTTRIGGERMODIFIERUI_H
#define BSDISTTRIGGERMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"


class QGridLayout;
class TableWidget;
class LineEdit;
class QLineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class QuadVariableWidget;

namespace UI {

class HkxObject;
class BSDistTriggerModifier;
class hkbVariableBindingSet;

class BSDistTriggerModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSDistTriggerModifierUI();
    BSDistTriggerModifierUI& operator=(const BSDistTriggerModifierUI&) = delete;
    BSDistTriggerModifierUI(const BSDistTriggerModifierUI &) = delete;
    ~BSDistTriggerModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events);
    void variableRenamed(const QString & name, int index);
    void eventRenamed(const QString & name, int index);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setTargetPosition();
    void setDistance();
    void setDistanceTrigger();
    void setTriggerEventId(int index, const QString & name);
    void setTriggerEventPayload();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSDistTriggerModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    QuadVariableWidget *targetPosition;
    DoubleSpinBox *distance;
    DoubleSpinBox *distanceTrigger;
    QLineEdit *triggerEventPayload;
};
}

#endif // BSDISTTRIGGERMODIFIERUI_H
