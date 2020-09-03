#ifndef EVENTDRIVENMODIFIERUI_H
#define EVENTDRIVENMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"

class BehaviorGraphView;
class TableWidget;
class LineEdit;
class QGridLayout;
class CheckBox;
class GenericTableWidget;

namespace UI {

class HkxObject;
class hkbEventDrivenModifier;
class hkbVariableBindingSet;

class EventDrivenModifierUI final: public QGroupBox
{
    Q_OBJECT
public:
    EventDrivenModifierUI();
    EventDrivenModifierUI& operator=(const EventDrivenModifierUI&) = delete;
    EventDrivenModifierUI(const EventDrivenModifierUI &) = delete;
    ~EventDrivenModifierUI() = default;
public:
    void loadData(HkxObject *data);
    void connectToTables(GenericTableWidget *modifiers, GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events);
    void variableRenamed(const QString & name, int index);
    void eventRenamed(const QString & name, int index);
    void modifierRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewModifiers(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName(const QString &newname);
    void setEnable();
    void setActivateEventId(int index, const QString & name);
    void setDeactivateEventId(int index, const QString & name);
    void setActiveByDefault();
    void setBindingVariable(int index, const QString & name);
    void eventTableElementSelected(int index, const QString &name);
    void viewSelected(int row, int column);
    void setModifier(int index, const QString & name);
private:
    void toggleSignals(bool toggleconnections);
    void selectTableToView(bool viewproperties, const QString & path);
private:
    static const QStringList headerLabels;
    BehaviorGraphView *behaviorView;
    hkbEventDrivenModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    CheckBox *activeByDefault;
};
}
#endif // EVENTDRIVENMODIFIERUI_H
