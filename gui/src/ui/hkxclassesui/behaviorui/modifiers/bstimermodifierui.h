#ifndef BSTIMERMODIFIERUI_H
#define BSTIMERMODIFIERUI_H

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
class BSTimerModifier;
class hkbVariableBindingSet;

class BSTimerModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    BSTimerModifierUI();
    BSTimerModifierUI& operator=(const BSTimerModifierUI&) = delete;
    BSTimerModifierUI(const BSTimerModifierUI &) = delete;
    ~BSTimerModifierUI() = default;
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
    void setResetAlarm();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BSTimerModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *alarmTimeSeconds;
    QLineEdit *alarmEventPayload;
    CheckBox *resetAlarm;
};
}
#endif // BSTIMERMODIFIERUI_H
