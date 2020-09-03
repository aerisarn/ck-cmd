#include "transitionsui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbexpressioncondition.h"
#include "src/hkxclasses/behavior/hkbblendingtransitioneffect.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachine.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachinestateinfo.h"
#include "src/hkxclasses/behavior/hkbstatemachinetransitioninfoarray.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/hkbExpressionCondition.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/hkxclassesui/behaviorui/blendingtransitioneffectui.h"
#include "src/filetypes/behaviorfile.h"

#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 22

#define TRIGGER_INTERVAL_ENTER_EVENT_ID_ROW 0
#define TRIGGER_INTERVAL_EXIT_EVENT_ID_ROW 1
#define TRIGGER_INTERVAL_ENTER_TIME_ROW 2
#define TRIGGER_INTERVAL_EXIT_TIME_ROW 3
#define INITIATE_INTERVAL_ENTER_EVENT_ID_ROW 4
#define INITIATE_INTERVAL_EXIT_EVENT_ID_ROW 5
#define INITIATE_INTERVAL_ENTER_TIME_ROW 6
#define INITIATE_INTERVAL_EXIT_TIME_ROW 7
#define TRANSITION_ROW 8
#define CONDITION_ROW 9
#define EVENT_ID_ROW 10
#define TO_STATE_ID_ROW 11
#define FROM_NESTED_STATE_ID_ROW 12
#define TO_NESTED_STATE_ID_ROW 13
#define PRIORITY_ROW 14
#define FLAG_GLOBAL_WILDCARD_ROW 15
#define FLAG_LOCAL_WILDCARD_ROW 16
#define FLAG_USE_NESTED_STATE_ROW 17
#define FLAG_ALLOW_SELF_TRANSITION_ROW 18
#define FLAG_DISALLOW_RANDOM_TRANSITION_ROW 19
#define FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE_ROW 20
#define FLAG_ABUT_END_STATE_ROW 21

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define VALUE_COLUMN 2

const QStringList TransitionsUI::headerLabels = {
    "Name",
    "Type",
    "Value"
};

TransitionsUI::TransitionsUI()
    : file(nullptr),
      bsData(nullptr),
      transitionIndex(-1),
      transitionUI(new BlendingTransitionEffectUI()),
      returnPB(new QPushButton("Return")),
      topLyt(new QGridLayout),
      groupBox(new QGroupBox("hkbStateMachineTransitionInfoArray")),
      table(new TableWidget(QColor(Qt::white))),
      enterTimeTI(new DoubleSpinBox),
      exitTimeTI(new DoubleSpinBox),
      enterTimeII(new DoubleSpinBox),
      exitTimeII(new DoubleSpinBox),
      transition(new CheckButtonCombo("Edit")),
      condition(new ConditionLineEdit),
      toStateId(new ComboBox),
      fromNestedStateId(new ComboBox),
      toNestedStateId(new ComboBox),
      priority(new SpinBox),
      flagGlobalWildcard(new CheckBox),
      flagLocalWildcard(new CheckBox),
      flagUseNestedState(new CheckBox),
      flagAllowSelfTransition(new CheckBox),
      flagDisallowRandomTransition(new CheckBox),
      flagDisallowReturnToState(new CheckBox),
      flagAbutEndState(new CheckBox)
{
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(TRIGGER_INTERVAL_ENTER_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("Trigger Interval: Enter Event Id"));
    table->setItem(TRIGGER_INTERVAL_ENTER_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(TRIGGER_INTERVAL_ENTER_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(TRIGGER_INTERVAL_EXIT_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("Trigger Interval: Exit Event Id"));
    table->setItem(TRIGGER_INTERVAL_EXIT_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(TRIGGER_INTERVAL_EXIT_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(TRIGGER_INTERVAL_ENTER_TIME_ROW, NAME_COLUMN, new TableWidgetItem("Trigger Interval: Enter Time"));
    table->setItem(TRIGGER_INTERVAL_ENTER_TIME_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(TRIGGER_INTERVAL_ENTER_TIME_ROW, VALUE_COLUMN, enterTimeTI);
    table->setItem(TRIGGER_INTERVAL_EXIT_TIME_ROW, NAME_COLUMN, new TableWidgetItem("Trigger Interval: Exit Time"));
    table->setItem(TRIGGER_INTERVAL_EXIT_TIME_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(TRIGGER_INTERVAL_EXIT_TIME_ROW, VALUE_COLUMN, exitTimeTI);
    table->setItem(INITIATE_INTERVAL_ENTER_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("Initiate Interval: Enter Event Id"));
    table->setItem(INITIATE_INTERVAL_ENTER_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(INITIATE_INTERVAL_ENTER_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(INITIATE_INTERVAL_EXIT_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("Initiate Interval: Exit Event Id"));
    table->setItem(INITIATE_INTERVAL_EXIT_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(INITIATE_INTERVAL_EXIT_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(INITIATE_INTERVAL_ENTER_TIME_ROW, NAME_COLUMN, new TableWidgetItem("Initiate Interval: Enter Time"));
    table->setItem(INITIATE_INTERVAL_ENTER_TIME_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(INITIATE_INTERVAL_ENTER_TIME_ROW, VALUE_COLUMN, enterTimeII);
    table->setItem(INITIATE_INTERVAL_EXIT_TIME_ROW, NAME_COLUMN, new TableWidgetItem("Initiate Interval: Exit Time"));
    table->setItem(INITIATE_INTERVAL_EXIT_TIME_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(INITIATE_INTERVAL_EXIT_TIME_ROW, VALUE_COLUMN, exitTimeII);
    table->setItem(TRANSITION_ROW, NAME_COLUMN, new TableWidgetItem("transition"));
    table->setItem(TRANSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkbTransitionEffect", Qt::AlignCenter));
    table->setItem(TRANSITION_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(TRANSITION_ROW, VALUE_COLUMN, transition);
    table->setItem(CONDITION_ROW, NAME_COLUMN, new TableWidgetItem("condition"));
    table->setItem(CONDITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkbExpressionCondition", Qt::AlignCenter));
    table->setCellWidget(CONDITION_ROW, VALUE_COLUMN, condition);
    table->setItem(EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("eventId"));
    table->setItem(EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(TO_STATE_ID_ROW, NAME_COLUMN, new TableWidgetItem("toStateId"));
    table->setItem(TO_STATE_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setCellWidget(TO_STATE_ID_ROW, VALUE_COLUMN, toStateId);
    table->setItem(FROM_NESTED_STATE_ID_ROW, NAME_COLUMN, new TableWidgetItem("fromNestedStateId"));
    table->setItem(FROM_NESTED_STATE_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setCellWidget(FROM_NESTED_STATE_ID_ROW, VALUE_COLUMN, fromNestedStateId);
    table->setItem(TO_NESTED_STATE_ID_ROW, NAME_COLUMN, new TableWidgetItem("toNestedStateId"));
    table->setItem(TO_NESTED_STATE_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setCellWidget(TO_NESTED_STATE_ID_ROW, VALUE_COLUMN, toNestedStateId);
    table->setItem(PRIORITY_ROW, NAME_COLUMN, new TableWidgetItem("priority"));
    table->setItem(PRIORITY_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(PRIORITY_ROW, VALUE_COLUMN, priority);
    table->setItem(FLAG_GLOBAL_WILDCARD_ROW, NAME_COLUMN, new TableWidgetItem("flagGlobalWildcard"));
    table->setItem(FLAG_GLOBAL_WILDCARD_ROW, TYPE_COLUMN, new TableWidgetItem("TransitionFlag", Qt::AlignCenter));
    table->setCellWidget(FLAG_GLOBAL_WILDCARD_ROW, VALUE_COLUMN, flagGlobalWildcard);
    table->setItem(FLAG_LOCAL_WILDCARD_ROW, NAME_COLUMN, new TableWidgetItem("flagLocalWildcard"));
    table->setItem(FLAG_LOCAL_WILDCARD_ROW, TYPE_COLUMN, new TableWidgetItem("TransitionFlag", Qt::AlignCenter));
    table->setCellWidget(FLAG_LOCAL_WILDCARD_ROW, VALUE_COLUMN, flagLocalWildcard);
    table->setItem(FLAG_USE_NESTED_STATE_ROW, NAME_COLUMN, new TableWidgetItem("flagUseNestedState"));
    table->setItem(FLAG_USE_NESTED_STATE_ROW, TYPE_COLUMN, new TableWidgetItem("TransitionFlag", Qt::AlignCenter));
    table->setCellWidget(FLAG_USE_NESTED_STATE_ROW, VALUE_COLUMN, flagUseNestedState);
    table->setItem(FLAG_ALLOW_SELF_TRANSITION_ROW, NAME_COLUMN, new TableWidgetItem("flagAllowSelfTransition"));
    table->setItem(FLAG_ALLOW_SELF_TRANSITION_ROW, TYPE_COLUMN, new TableWidgetItem("TransitionFlag", Qt::AlignCenter));
    table->setCellWidget(FLAG_ALLOW_SELF_TRANSITION_ROW, VALUE_COLUMN, flagAllowSelfTransition);
    table->setItem(FLAG_DISALLOW_RANDOM_TRANSITION_ROW, NAME_COLUMN, new TableWidgetItem("flagDisallowRandomTransition"));
    table->setItem(FLAG_DISALLOW_RANDOM_TRANSITION_ROW, TYPE_COLUMN, new TableWidgetItem("TransitionFlag", Qt::AlignCenter));
    table->setCellWidget(FLAG_DISALLOW_RANDOM_TRANSITION_ROW, VALUE_COLUMN, flagDisallowRandomTransition);
    table->setItem(FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE_ROW, NAME_COLUMN, new TableWidgetItem("flagDisallowReturnToState"));
    table->setItem(FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE_ROW, TYPE_COLUMN, new TableWidgetItem("TransitionFlag", Qt::AlignCenter));
    table->setCellWidget(FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE_ROW, VALUE_COLUMN, flagDisallowReturnToState);
    table->setItem(FLAG_ABUT_END_STATE_ROW, NAME_COLUMN, new TableWidgetItem("flagAbutEndState"));
    table->setItem(FLAG_ABUT_END_STATE_ROW, TYPE_COLUMN, new TableWidgetItem("TransitionFlag", Qt::AlignCenter));
    table->setCellWidget(FLAG_ABUT_END_STATE_ROW, VALUE_COLUMN, flagAbutEndState);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(transitionUI);
    fromNestedStateId->setDisabled(true);
    toNestedStateId->setDisabled(true);
    toggleSignals(true);
}

void TransitionsUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(enterTimeTI, SIGNAL(editingFinished()), this, SLOT(setTriggerIntervalEnterTime()), Qt::UniqueConnection);
        connect(exitTimeTI, SIGNAL(editingFinished()), this, SLOT(setTriggerIntervalExitTime()), Qt::UniqueConnection);
        connect(enterTimeII, SIGNAL(editingFinished()), this, SLOT(setInitiateIntervalEnterTime()), Qt::UniqueConnection);
        connect(exitTimeII, SIGNAL(editingFinished()), this, SLOT(setInitiateIntervalExitTime()), Qt::UniqueConnection);
        connect(transition, SIGNAL(pressed()), this, SLOT(viewTransitionEffect()), Qt::UniqueConnection);
        connect(transition, SIGNAL(enabled(bool)), this, SLOT(toggleTransitionEffect(bool)), Qt::UniqueConnection);
        connect(condition, SIGNAL(editingFinished()), this, SLOT(setCondition()), Qt::UniqueConnection);
        connect(toStateId, SIGNAL(currentIndexChanged(QString)), this, SLOT(setToStateId(QString)), Qt::UniqueConnection);
        connect(fromNestedStateId, SIGNAL(currentIndexChanged(QString)), this, SLOT(setFromNestedStateId(QString)), Qt::UniqueConnection);
        connect(toNestedStateId, SIGNAL(currentIndexChanged(QString)), this, SLOT(setToNestedStateId(QString)), Qt::UniqueConnection);
        connect(priority, SIGNAL(editingFinished()), this, SLOT(setPriority()), Qt::UniqueConnection);
        connect(flagGlobalWildcard, SIGNAL(released()), this, SLOT(toggleGlobalWildcardFlag()), Qt::UniqueConnection);
        connect(flagLocalWildcard, SIGNAL(released()), this, SLOT(toggleLocalWildcardFlag()), Qt::UniqueConnection);
        connect(flagUseNestedState, SIGNAL(released()), this, SLOT(toggleUseNestedStateFlag()), Qt::UniqueConnection);
        connect(flagAllowSelfTransition, SIGNAL(released()), this, SLOT(toggleAllowSelfTransition()), Qt::UniqueConnection);
        connect(flagDisallowRandomTransition, SIGNAL(released()), this, SLOT(toggleDisallowRandomTransitionFlag()), Qt::UniqueConnection);
        connect(flagDisallowReturnToState, SIGNAL(released()), this, SLOT(toggleDisallowReturnToStateFlag()), Qt::UniqueConnection);
        connect(flagAbutEndState, SIGNAL(released()), this, SLOT(toggleAbutEndStateFlag()), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(transitionEffectRenamed(QString)), this, SLOT(transitionEffectRenamed(QString)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(enterTimeTI, SIGNAL(editingFinished()), this, SLOT(setTriggerIntervalEnterTime()));
        disconnect(exitTimeTI, SIGNAL(editingFinished()), this, SLOT(setTriggerIntervalExitTime()));
        disconnect(enterTimeII, SIGNAL(editingFinished()), this, SLOT(setInitiateIntervalEnterTime()));
        disconnect(exitTimeII, SIGNAL(editingFinished()), this, SLOT(setInitiateIntervalExitTime()));
        disconnect(transition, SIGNAL(pressed()), this, SLOT(viewTransitionEffect()));
        disconnect(transition, SIGNAL(enabled(bool)), this, SLOT(toggleTransitionEffect(bool)));
        disconnect(condition, SIGNAL(editingFinished()), this, SLOT(setCondition()));
        disconnect(toStateId, SIGNAL(currentIndexChanged(QString)), this, SLOT(setToStateId(QString)));
        disconnect(fromNestedStateId, SIGNAL(currentIndexChanged(QString)), this, SLOT(setFromNestedStateId(QString)));
        disconnect(toNestedStateId, SIGNAL(currentIndexChanged(QString)), this, SLOT(setToNestedStateId(QString)));
        disconnect(priority, SIGNAL(editingFinished()), this, SLOT(setPriority()));
        disconnect(flagGlobalWildcard, SIGNAL(released()), this, SLOT(toggleGlobalWildcardFlag()));
        disconnect(flagLocalWildcard, SIGNAL(released()), this, SLOT(toggleLocalWildcardFlag()));
        disconnect(flagUseNestedState, SIGNAL(released()), this, SLOT(toggleUseNestedStateFlag()));
        disconnect(flagAllowSelfTransition, SIGNAL(released()), this, SLOT(toggleAllowSelfTransition()));
        disconnect(flagDisallowRandomTransition, SIGNAL(released()), this, SLOT(toggleDisallowRandomTransitionFlag()));
        disconnect(flagDisallowReturnToState, SIGNAL(released()), this, SLOT(toggleDisallowReturnToStateFlag()));
        disconnect(flagAbutEndState, SIGNAL(released()), this, SLOT(toggleAbutEndStateFlag()));
        disconnect(transitionUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(transitionUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)));
        disconnect(transitionUI, SIGNAL(transitionEffectRenamed(QString)), this, SLOT(transitionEffectRenamed(QString)));
        disconnect(transitionUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void TransitionsUI::loadData(BehaviorFile *parentfile, hkbStateMachine *parent, hkbStateMachineTransitionInfoArray::HkTransition *data, int index){
    toggleSignals(false);
    if (data && parent && parentfile){
        bsData = (hkbStateMachineTransitionInfoArray::HkTransition *)data;
        parentObj = parent;
        file = parentfile;
        transitionIndex = index;
        auto name = file->getEventNameAt(bsData->triggerInterval.enterEventId);
        auto loadTableValue = [&](int row){
            auto item = table->item(row, VALUE_COLUMN);
            (name != "") ? item->setText(name) : item->setText("NONE");
        };
        loadTableValue(TRIGGER_INTERVAL_ENTER_EVENT_ID_ROW);
        name = file->getEventNameAt(bsData->triggerInterval.exitEventId);
        loadTableValue(TRIGGER_INTERVAL_EXIT_EVENT_ID_ROW);
        enterTimeTI->setValue(bsData->triggerInterval.enterTime);
        exitTimeTI->setValue(bsData->triggerInterval.exitTime);
        name = file->getEventNameAt(bsData->initiateInterval.enterEventId);
        loadTableValue(INITIATE_INTERVAL_ENTER_EVENT_ID_ROW);
        name = file->getEventNameAt(bsData->initiateInterval.exitEventId);
        loadTableValue(INITIATE_INTERVAL_EXIT_EVENT_ID_ROW);
        enterTimeII->setValue(bsData->initiateInterval.enterTime);
        exitTimeII->setValue(bsData->initiateInterval.exitTime);
        auto transeffect = static_cast<hkbBlendingTransitionEffect *>(bsData->transition.data());
        if (transeffect){
            transition->setChecked(true);
            transition->setText(transeffect->getName());
        }else{
            transition->setChecked(false);
            transition->setText("nullptr");
        }
        (bsData->condition.data()) ? condition->setText(static_cast<hkbExpressionCondition *>(bsData->condition.data())->getExpression()) : condition->setText("");
        name = file->getEventNameAt(bsData->eventId);
        loadTableValue(EVENT_ID_ROW);
        toStateId->clear();
        toStateId->addItems(parentObj->getStateNames());
        toStateId->setCurrentIndex(parentObj->getStateNames().indexOf(parentObj->getStateName(bsData->toStateId)));
        //fromNestedStateId->clear();
        //fromNestedStateId->addItems(parentObj->getStateNames());
        toNestedStateId->clear();
        toNestedStateId->addItems(parentObj->getNestedStateNames(bsData->toStateId));
        toNestedStateId->setCurrentIndex(parentObj->getNestedStateNames(bsData->toStateId).indexOf(parentObj->getNestedStateName(bsData->toStateId, bsData->toNestedStateId)));
        priority->setValue(bsData->priority);
        auto flags = bsData->flags.split("|");
        flagGlobalWildcard->setChecked(false);
        flagUseNestedState->setChecked(false);
        flagDisallowRandomTransition->setChecked(false);
        flagDisallowReturnToState->setChecked(false);
        flagAbutEndState->setChecked(false);
        toNestedStateId->setDisabled(true);
        if (flags.isEmpty()){
            if (bsData->flags == "FLAG_IS_GLOBAL_WILDCARD"){
                flagGlobalWildcard->setChecked(true);
            }else if (bsData->flags == "FLAG_IS_LOCAL_WILDCARD"){
                flagLocalWildcard->setChecked(true);
            }else if (bsData->flags == "FLAG_TO_NESTED_STATE_ID_IS_VALID"){
                flagUseNestedState->setChecked(true);
                toNestedStateId->setDisabled(false);
            }else if (bsData->flags == "FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE"){
                flagAllowSelfTransition->setChecked(true);
            }else if (bsData->flags == "FLAG_DISALLOW_RANDOM_TRANSITION"){
                flagDisallowRandomTransition->setChecked(true);
            }else if (bsData->flags == "FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE"){
                flagDisallowReturnToState->setChecked(true);
            }else if (bsData->flags == "FLAG_ABUT_AT_END_OF_FROM_GENERATOR"){
                flagAbutEndState->setChecked(true);
            }
        }else{
            for (auto i = 0; i < flags.size(); i++){
                if (flags.at(i) == "FLAG_IS_GLOBAL_WILDCARD"){
                    flagGlobalWildcard->setChecked(true);
                }else if (flags.at(i) == "FLAG_IS_LOCAL_WILDCARD"){
                    flagLocalWildcard->setChecked(true);
                }else if (flags.at(i) == "FLAG_TO_NESTED_STATE_ID_IS_VALID"){
                    flagUseNestedState->setChecked(true);
                    toNestedStateId->setDisabled(false);
                }else if (flags.at(i) == "FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE"){
                    flagAllowSelfTransition->setChecked(true);
                }else if (flags.at(i) == "FLAG_DISALLOW_RANDOM_TRANSITION"){
                    flagDisallowRandomTransition->setChecked(true);
                }else if (flags.at(i) == "FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE"){
                    flagDisallowReturnToState->setChecked(true);
                }else if (flags.at(i) == "FLAG_ABUT_AT_END_OF_FROM_GENERATOR"){
                    flagAbutEndState->setChecked(true);
                }
            }
        }
    }else{
        LogFile::writeToLog("TransitionsUI::loadData(): The data, parent file or parent state machine is nullptr!!");
    }
    toggleSignals(true);
}

void TransitionsUI::setId(int index, const QString &name, int & eventid, int row){
    if (bsData){
        eventid = --index;
        auto item = table->item(row, VALUE_COLUMN);
        if (item){
            item->setText(name);
            checkInterval();
            (parentObj) ? parentObj->setIsFileChanged(true) : LogFile::writeToLog("TransitionsUI::setId(): parentObj is nullptr!!");
        }else{
            LogFile::writeToLog("TransitionsUI::setId(): invalid row selected!!");
        }
    }else{
        LogFile::writeToLog("TransitionsUI::setId(): The data is nullptr!!");
    }
}

void TransitionsUI::setTriggerIntervalEnterEventId(int index, const QString &name){
    if (bsData){
        setId(index, name, bsData->triggerInterval.enterEventId, TRIGGER_INTERVAL_ENTER_EVENT_ID_ROW);
    }else{
        LogFile::writeToLog("TransitionsUI::setTriggerIntervalEnterEventId(): The data is nullptr!!");
    }
}

void TransitionsUI::setTriggerIntervalExitEventId(int index, const QString &name){
    if (bsData){
        setId(index, name, bsData->triggerInterval.exitEventId, TRIGGER_INTERVAL_EXIT_EVENT_ID_ROW);
    }else{
        LogFile::writeToLog("TransitionsUI::setTriggerIntervalExitEventId(): The data is nullptr!!");
    }
}

void TransitionsUI::checkInterval(){
    if (bsData){
        auto check = [&](bool value, const QString & flag){
            if (value){
                if (!bsData->flags.contains(flag)){
                    (bsData->flags != "0") ? bsData->flags.append("|"+flag) : bsData->flags = flag;
                }
            }else{
                bsData->flags.remove(flag);
                auto lastindex = bsData->flags.lastIndexOf("|");
                (lastindex > bsData->flags.size() - 1) ? bsData->flags.remove(lastindex, lastindex + 1) : NULL;
            }
        };
        check(bsData->usingTriggerInterval(), "FLAG_USE_TRIGGER_INTERVAL");
        check(bsData->usingInitiateInterval(), "FLAG_USE_INITIATE_INTERVAL");
    }else{
        LogFile::writeToLog("TransitionsUI::checkInterval(): The data is nullptr!!");
    }
}

void TransitionsUI::setTriggerIntervalEnterTime(){
    if (bsData){
        bsData->triggerInterval.enterTime = enterTimeTI->value();
        checkInterval();
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setTriggerIntervalEnterTime(): The data is nullptr!!");
    }
}

void TransitionsUI::setTriggerIntervalExitTime(){
    if (bsData){
        bsData->triggerInterval.exitTime = exitTimeTI->value();
        checkInterval();
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setTriggerIntervalExitTime(): The data is nullptr!!");
    }
}

void TransitionsUI::setInitiateIntervalEnterEventId(int index, const QString &name){
    if (bsData){
        bsData->initiateInterval.enterEventId = --index;
        table->item(INITIATE_INTERVAL_ENTER_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
        checkInterval();
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setInitiateIntervalEnterEventId(): The data is nullptr!!");
    }
}

void TransitionsUI::setInitiateIntervalExitEventId(int index, const QString &name){
    if (bsData){
        bsData->initiateInterval.exitEventId = --index;
        table->item(INITIATE_INTERVAL_EXIT_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
        checkInterval();
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setInitiateIntervalExitEventId(): The data is nullptr!!");
    }
}

void TransitionsUI::setInitiateIntervalEnterTime(){
    if (bsData){
        bsData->initiateInterval.enterTime = enterTimeII->value();
        checkInterval();
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setInitiateIntervalEnterTime(): The data is nullptr!!");
    }
}

void TransitionsUI::setInitiateIntervalExitTime(){
    if (bsData){
        bsData->initiateInterval.exitTime = exitTimeII->value();
        checkInterval();
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setInitiateIntervalExitTime(): The data is nullptr!!");
    }
}

void TransitionsUI::viewTransitionEffect(){
    if (bsData){
        transitionUI->loadData(bsData->transition.data());
        setCurrentIndex(TRANSITION_EFFECT_WIDGET);
    }else{
        LogFile::writeToLog("TransitionsUI::viewTransitionEffect(): The data is nullptr!!");
    }
}

void TransitionsUI::toggleTransitionEffect(bool enable){
    if (bsData){
        if (!enable){
            bsData->transition = HkxSharedPtr();
            static_cast<BehaviorFile *>(parentObj->getParentFile())->removeOtherData();
        }else if (!bsData->transition.data()){
            auto effect = new hkbBlendingTransitionEffect(parentObj->getParentFile());
            bsData->transition = HkxSharedPtr(effect);
            transition->setText(effect->getName());
        }
    }else{
        LogFile::writeToLog("TransitionsUI::toggleTransitionEffect(): The data is nullptr!!");
    }
}

void TransitionsUI::setCondition(){
    if (bsData && parentObj){
        if (condition->text() != ""){
            if (bsData->condition.data()){
                static_cast<hkbExpressionCondition *>(bsData->condition.data())->setExpression(condition->text());
            }else{
                bsData->condition = HkxSharedPtr(new hkbExpressionCondition(static_cast<BehaviorFile *>(parentObj->getParentFile()), condition->text()));
            }
        }else{
            bsData->condition = HkxSharedPtr();
            if (!bsData->flags.contains("FLAG_DISABLE_CONDITION")){
                (bsData->flags == "0") ? bsData->flags = "FLAG_DISABLE_CONDITION" : bsData->flags.append("|FLAG_DISABLE_CONDITION");
            }
        }
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setCondition(): The data or parent object is nullptr!!");
    }
}

void TransitionsUI::setEventId(int index, const QString &name){
    if (bsData){
        bsData->eventId = --index;
        (name != "") ? table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : LogFile::writeToLog("TransitionsUI::setEventId(): The event name is nullptr!!");
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setEventId(): The data is nullptr!!");
    }
}

void TransitionsUI::setToStateId(const QString &name){
    if (bsData && parentObj){
        if (name != ""){
            bsData->toStateId = parentObj->getStateId(name);
            if (flagUseNestedState->isChecked()){
                disconnect(toNestedStateId, SIGNAL(currentIndexChanged(QString)), this, SLOT(setToNestedStateId(QString)));
                toNestedStateId->clear();
                toNestedStateId->addItems(parentObj->getNestedStateNames(bsData->toStateId));
                toNestedStateId->setCurrentIndex(parentObj->getNestedStateNames(bsData->toStateId).indexOf(parentObj->getNestedStateName(bsData->toStateId, bsData->toNestedStateId)));
                connect(toNestedStateId, SIGNAL(currentIndexChanged(QString)), this, SLOT(setToNestedStateId(QString)), Qt::UniqueConnection);
                emit transitionNameChanged("toNestedState_"+toNestedStateId->currentText(), transitionIndex);
            }else{
                emit transitionNameChanged("to_"+name, transitionIndex);
            }
        }else{
            LogFile::writeToLog("TransitionsUI::setToStateId(): The event name is nullptr!!");
        }
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setToStateId(): The data or parent object is nullptr!!");
    }
}

void TransitionsUI::setFromNestedStateId(const QString &name){
    if (bsData && parentObj){
        (name != "") ? bsData->fromNestedStateId = parentObj->getNestedStateId(name, bsData->toStateId) : LogFile::writeToLog("TransitionsUI::setFromNestedStateId(): The event name is nullptr!!");
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setFromNestedStateId(): The data or parent object is nullptr!!");
    }
}

void TransitionsUI::setToNestedStateId(const QString &name){
    if (bsData && parentObj){
        if (name != ""){
            bsData->toNestedStateId = parentObj->getNestedStateId(name, bsData->toStateId);
            emit transitionNameChanged("toNestedState_"+toNestedStateId->currentText(), transitionIndex);
        }else{
            LogFile::writeToLog("TransitionsUI::setToNestedStateId(): The event name is nullptr!!");
        }
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setToNestedStateId(): The data or parent object is nullptr!!");
    }
}

void TransitionsUI::setPriority(){
    if (bsData){
        bsData->priority = priority->value();
        parentObj->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("TransitionsUI::setPriority(): The data is nullptr!!");
    }
}

void TransitionsUI::toggleFlag(CheckBox *checkbox, const QString & flag, ComboBox *combobox){
    if (bsData){
        if (checkbox->isChecked()){
            if (bsData->flags == "0"){
                bsData->flags = flag;
            }else if (!bsData->flags.contains(flag)){
                bsData->flags.append("|"+flag);
            }
            (combobox) ? combobox->setDisabled(false) : NULL;
        }else{
            (bsData->flags == flag) ? bsData->flags = "0" : bsData->flags.remove(flag), bsData->flags.replace("||", "|");
            (combobox) ? combobox->setDisabled(true) : NULL;
        }
        (parentObj) ? parentObj->setIsFileChanged(true) : LogFile::writeToLog("TransitionsUI::toggleFlag(): The parentObj is nullptr!!");
    }else{
        LogFile::writeToLog("TransitionsUI::toggleFlag(): The data is nullptr!!");
    }
}

void TransitionsUI::toggleGlobalWildcardFlag(){
    (bsData) ? toggleFlag(flagGlobalWildcard, "FLAG_IS_GLOBAL_WILDCARD") : LogFile::writeToLog("TransitionsUI::toggleGlobalWildcardFlag(): The data is nullptr!!");
}

void TransitionsUI::toggleLocalWildcardFlag(){
    (bsData) ? toggleFlag(flagLocalWildcard, "FLAG_IS_LOCAL_WILDCARD") : LogFile::writeToLog("TransitionsUI::toggleLocalWildcardFlag(): The data is nullptr!!");
}

void TransitionsUI::toggleUseNestedStateFlag(){
    (bsData) ? toggleFlag(flagUseNestedState, "FLAG_TO_NESTED_STATE_ID_IS_VALID", toNestedStateId) : LogFile::writeToLog("TransitionsUI::toggleUseNestedStateFlag(): The data is nullptr!!");
}

void TransitionsUI::toggleAllowSelfTransition(){
    (bsData) ? toggleFlag(flagUseNestedState, "FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE", toNestedStateId) : LogFile::writeToLog("TransitionsUI::toggleAllowSelfTransition(): The data is nullptr!!");
}

void TransitionsUI::toggleDisallowRandomTransitionFlag(){
    (bsData) ? toggleFlag(flagDisallowRandomTransition, "FLAG_DISALLOW_RANDOM_TRANSITION") : LogFile::writeToLog("TransitionsUI::toggleDisallowRandomTransitionFlag(): The data is nullptr!!");
}

void TransitionsUI::toggleDisallowReturnToStateFlag(){
    (bsData) ? toggleFlag(flagDisallowReturnToState, "FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE") : LogFile::writeToLog("TransitionsUI::toggleDisallowReturnToStateFlag(): The data is nullptr!!");
}

void TransitionsUI::toggleAbutEndStateFlag(){
    (bsData) ? toggleFlag(flagAbutEndState, "FLAG_ABUT_AT_END_OF_FROM_GENERATOR") : LogFile::writeToLog("TransitionsUI::toggleAbutEndStateFlag(): The data is nullptr!!");
}

void TransitionsUI::eventTableElementSelected(int index, const QString &name){
    if (bsData){
        switch (table->currentRow()){
        case TRIGGER_INTERVAL_ENTER_EVENT_ID_ROW:
            setTriggerIntervalEnterEventId(index, name); break;
        case TRIGGER_INTERVAL_EXIT_EVENT_ID_ROW:
            setTriggerIntervalExitEventId(index, name); break;
        case INITIATE_INTERVAL_ENTER_EVENT_ID_ROW:
            setInitiateIntervalEnterEventId(index, name); break;
        case INITIATE_INTERVAL_EXIT_EVENT_ID_ROW:
            setInitiateIntervalExitEventId(index, name); break;
        case EVENT_ID_ROW:
            setEventId(index, name); break;
        default:
            LogFile::writeToLog("TransitionsUI::eventTableElementSelected(): Event relayed to wrong table row!!");
        }
    }else{
        LogFile::writeToLog("TransitionsUI::eventTableElementSelected(): The data is nullptr!!");
    }
}

void TransitionsUI::variableTableElementSelected(int index, const QString &name){
    if (bsData){
        switch (currentIndex()){
        case TRANSITION_EFFECT_WIDGET:
            transitionUI->setBindingVariable(index, name); break;
        default:
            LogFile::writeToLog("TransitionsUI::variableTableElementSelected(): Event relayed to wrong widget!!");
        }
    }else{
        LogFile::writeToLog("TransitionsUI::variableTableElementSelected(): The data is nullptr!!");
    }
}

void TransitionsUI::returnToWidget(){
    setCurrentIndex(TRANSITION_WIDGET);
}

void TransitionsUI::viewSelectedChild(int row, int column){
    if (bsData){
        auto index = 0;
        if (row < BASE_NUMBER_OF_ROWS && row >= 0){
            if (column == VALUE_COLUMN){
                switch (row){
                case TRIGGER_INTERVAL_ENTER_EVENT_ID_ROW:
                    index = bsData->triggerInterval.enterEventId; break;
                case TRIGGER_INTERVAL_EXIT_EVENT_ID_ROW:
                    index = bsData->triggerInterval.exitEventId; break;
                case INITIATE_INTERVAL_ENTER_EVENT_ID_ROW:
                    index = bsData->initiateInterval.enterEventId; break;
                case INITIATE_INTERVAL_EXIT_EVENT_ID_ROW:
                    index = bsData->initiateInterval.exitEventId; break;
                case EVENT_ID_ROW:
                    index = bsData->eventId; break;
                default:
                    return;
                }
                emit viewEvents(++index, QString(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("BlendingTransitionEffectUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void TransitionsUI::transitionEffectRenamed(const QString &name){
    (name != "") ? transition->setText(name) : LogFile::writeToLog("TransitionsUI::transitionEffectRenamed(): The name the empty string!!");
}

void TransitionsUI::eventRenamed(const QString &name, int index){
    if (bsData){
        auto set = [&](int eventid, int row){
            (index == eventid) ? table->item(row, VALUE_COLUMN)->setText(name) : NULL;
        };
        set(bsData->triggerInterval.enterEventId, TRIGGER_INTERVAL_ENTER_EVENT_ID_ROW);
        set(bsData->triggerInterval.exitEventId, TRIGGER_INTERVAL_EXIT_EVENT_ID_ROW);
        set(bsData->initiateInterval.enterEventId, INITIATE_INTERVAL_ENTER_EVENT_ID_ROW);
        set(bsData->initiateInterval.exitEventId, INITIATE_INTERVAL_EXIT_EVENT_ID_ROW);
        set(bsData->eventId, EVENT_ID_ROW);
    }else{
        LogFile::writeToLog("TransitionsUI::eventRenamed(): The data is nullptr!!");
    }
}
