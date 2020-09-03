#ifndef STATEMACHINEUI_H
#define STATEMACHINEUI_H

#include <QStackedWidget>

#include "src/utility.h"


class BehaviorGraphView;
class TableWidget;
class QGridLayout;
class SpinBox;
class LineEdit;
class ComboBox;
class CheckBox;
class QGroupBox;
class GenericTableWidget;
class CheckButtonCombo;

namespace UI {

class HkxObject;
class hkbStateMachine;
class hkbStateMachineStateInfo;
class hkbStateMachineTransitionInfoArray;
class hkbVariableBindingSet;
class StateUI;
class TransitionsUI;

class EventUI;

class StateMachineUI final: public QStackedWidget
{
    Q_OBJECT
public:
    StateMachineUI();
    StateMachineUI& operator=(const StateMachineUI&) = delete;
    StateMachineUI(const StateMachineUI &) = delete;
    ~StateMachineUI() = default;
public:
    void loadData(HkxObject *data);
    void variableRenamed(const QString & name, int index);
    void eventRenamed(const QString & name, int index);
    void generatorRenamed(const QString & name, int index);
    void setBehaviorView(BehaviorGraphView *view);
    void connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events);
signals:
    void generatorNameChanged(const QString & newName, int index);
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewEvents(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewGenerators(int index, const QString & typeallowed, const QStringList &typesdisallowed);
private slots:
    void setName(const QString &newname);
    void setStartStateId(int index);
    void setReturnToPreviousStateEventId(int index, const QString &name);
    void setRandomTransitionEventId(int index, const QString &name);
    void setTransitionToNextHigherStateEventId(int index, const QString &name);
    void setTransitionToNextLowerStateEventId(int index, const QString &name);
    void setSyncVariableIndex(int index, const QString &name);
    void setWrapAroundStateId(bool checked);
    void setMaxSimultaneousTransitions();
    void setStartStateMode(int index);
    void setSelfTransitionMode(int index);
    void setStateIDForRow(int index, int newID, const QString &statename);
    void viewSelectedChild(int row, int column);
    void eventTableElementSelected(int index, const QString &name);
    void variableTableElementSelected(int index, const QString &name);
    void generatorTableElementSelected(int index, const QString &name);
    void returnToWidget(bool reloadData);
    void returnToWidget();
    void viewEventToSendWhenStateOrTransitionChanges();
    void toggleEventToSendWhenStateOrTransitionChanges(bool enable);
    void stateRenamed(const QString & name, int stateIndex);
    void transitionRenamed(const QString &name, int index);
    void swapGeneratorIndices(int index1, int index2);
private:
    void toggleSignals(bool toggleconnections);
    void addStateWithGenerator();
    void addTransition();
    void loadDynamicTableRows();
    void setBindingVariable(int index, const QString & name);
    void removeState(int index);
    void removeTransition(int index);
    void selectTableToView(bool properties, const QString & path);
private:
    enum ACTIVE_WIDGET {
        MAIN_WIDGET,
        STATE_WIDGET = 1,
        TRANSITION_WIDGET = 2,
        EVENT_PAYLOAD_WIDGET = 3
    };
    enum Generator_Type {
        STATE_MACHINE = 0,
        MANUAL_SELECTOR_GENERATOR,
        BLENDER_GENERATOR,
        I_STATE_TAGGING_GENERATOR,
        BONE_SWITCH_GENERATOR,
        CYCLIC_BLEND_TRANSITION_GENERATOR,
        SYNCHRONIZED_CLIP_GENERATOR,
        MODIFIER_GENERATOR,
        OFFSET_ANIMATION_GENERATOR,
        POSE_MATCHING_GENERATOR,
        CLIP_GENERATOR,
        BEHAVIOR_REFERENCE_GENERATOR,
        GAMEBYRO_SEQUENCE_GENERATOR
    };
private:
    static const QStringList types;
    static const QStringList headerLabels;
    int transitionsButtonRow;
    BehaviorGraphView *behaviorView;
    hkbStateMachine *bsData;
    QGroupBox *groupBox;
    EventUI *eventUI;
    StateUI *stateUI;
    TransitionsUI *transitionUI;
    QGridLayout *topLyt;
    TableWidget *table;
    ComboBox *typeSelectorCB;
    LineEdit *name;
    CheckButtonCombo *eventToSendWhenStateOrTransitionChanges;
    ComboBox *startStateId;
    CheckBox *wrapAroundStateId;
    SpinBox *maxSimultaneousTransitions;
    ComboBox *startStateMode;
    ComboBox *selfTransitionMode;
};
}
#endif // STATEMACHINEUI_H
