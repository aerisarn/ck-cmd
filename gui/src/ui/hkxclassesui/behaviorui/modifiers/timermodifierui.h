#ifndef TIMERMODIFIERUI_H
#define TIMERMODIFIERUI_H

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
class hkbTimerModifier;
class hkbVariableBindingSet;

class TimerModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    TimerModifierUI();
    TimerModifierUI& operator=(const TimerModifierUI&) = delete;
    TimerModifierUI(const TimerModifierUI &) = delete;
    ~TimerModifierUI() = default;
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
    void setAlarmTimeSeconds();
    void setAlarmEventId(int index, const QString & name);
    void setAlarmEventPayload();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    hkbTimerModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *alarmTimeSeconds;
    QLineEdit *alarmEventPayload;
};
}
#endif // TIMERMODIFIERUI_H
