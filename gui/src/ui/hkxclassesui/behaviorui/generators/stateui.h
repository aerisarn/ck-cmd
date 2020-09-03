#ifndef STATEUI_H
#define STATEUI_H

#include <QStackedWidget>

#include "src/utility.h"
#include "src/hkxclasses/hkxobject.h"


class BehaviorGraphView;


class QGridLayout;
class TableWidget;
class SpinBox;
class LineEdit;
class ComboBox;
class QPushButton;
class DoubleSpinBox;
class CheckBox;
class QGroupBox;
class GenericTableWidget;

namespace UI {
	
class HkxObject;
class TransitionsUI;
class hkbStateMachineStateInfo;
class EventUI;

class StateUI final: public QStackedWidget
{
    Q_OBJECT
    friend class StateMachineUI;
public:
    StateUI();
    StateUI& operator=(const StateUI&) = delete;
    StateUI(const StateUI &) = delete;
    ~StateUI() = default;
public:
    void loadData(HkxObject *data, int stateindex);
    void connectToTables(GenericTableWidget *generators, GenericTableWidget *events);
    void eventRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
    void generatorRenamed(const QString & name, int index);
    void setReturnPushButonVisability(bool visible);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void stateNameChanged(const QString & newName, int index);
    void stateIdChanged(int stateIndex, int newID, const QString & statename);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void returnToParent(bool reloadData);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void setStateId(int id);
    void setProbability();
    void setEnable();
    void eventTableElementSelected(int index, const QString & name);
    void viewSelectedChild(int row, int column);
    void returnToWidget();
    void transitionRenamed(const QString &name, int index);
    void generatorTableElementSelected(int index, const QString &name);
private:
    void toggleSignals(bool toggleconnections);
    void removeEvent(HkxSharedPtr & eventarray, int index);
    void addEvent(HkxSharedPtr & eventarray);
    void setGenerator(int index, const QString &name);
    void variableTableElementSelected(int index, const QString &name);
    void addEnterEvent();
    void removeEnterEvent(int index);
    void addExitEvent();
    void removeExitEvent(int index);
    void addTransition();
    void removeTransition(int index);
    void loadDynamicTableRows();
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        EVENT_PAYLOAD_WIDGET = 1,
        TRANSITION_WIDGET = 2
    };
private:
    static const QStringList headerLabels;
    int exitEventsButtonRow;
    int transitionsButtonRow;
    BehaviorGraphView *behaviorView;
    hkbStateMachineStateInfo *bsData;
    int stateIndex;
    QGroupBox *groupBox;
    EventUI *eventUI;
    TransitionsUI *transitionUI;
    QGridLayout *topLyt;
    TableWidget *table;
    QPushButton *returnPB;
    LineEdit *name;
    SpinBox *stateId;
    DoubleSpinBox *probability;
    CheckBox *enable;
    CheckBox *enableTransitions;
};
}
#endif // STATEUI_H
