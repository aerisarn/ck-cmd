#include "StateMachineUI.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/hkbstatemachinetransitioninfoarray.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachine.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachinestateinfo.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/hkxclassesui/behaviorui/transitionsui.h"
#include "src/ui/hkxclassesui/behaviorui/generators/stateui.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/hkxclassesui/behaviorui/transitionsui.h"
#include "src/ui/hkxclassesui/behaviorui/eventui.h"
#include "src/ui/mainwindow.h"

#include <QHeaderView>

#include "src/ui/genericdatawidgets.h"

using namespace UI;

#define BASE_NUMBER_OF_ROWS 14

#define NAME_ROW 0
#define EVENT_TO_SEND_WHEN_STATE_OR_TRANSITION_CHANGES_ROW 1
#define START_STATE_ID_ROW 2
#define RETURN_TO_PREVIOUS_STATE_EVENT_ID_ROW 3
#define RANDOM_TRANSITION_EVENT_ID_ROW 4
#define TRANSITION_TO_NEXT_HIGHER_STATE_EVENT_ID_ROW 5
#define TRANSITION_TO_NEXT_LOWER_STATE_EVENT_ID_ROW 6
#define SYNC_VARIABLE_INDEX_ROW 7
#define WRAP_AROUND_STATE_ID_ROW 8
#define MAX_SIMULTANEOUS_TRANSITIONS_ROW 9
#define START_STATE_MODE_ROW 10
#define SELF_TRANSITION_MODE_ROW 11
#define ADD_STATE_ROW 12
#define INITIAL_ADD_TRANSITION_ROW 13

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList StateMachineUI::types = {
    "hkbStateMachine",
    "hkbManualSelectorGenerator",
    "hkbBlenderGenerator",
    "BSiStateTaggingGenerator",
    "BSBoneSwitchGenerator",
    "BSCyclicBlendTransitionGenerator",
    "BSSynchronizedClipGenerator",
    "hkbModifierGenerator",
    "BSOffsetAnimationGenerator",
    "hkbPoseMatchingGenerator",
    "hkbClipGenerator",
    "hkbBehaviorReferenceGenerator",
    "BGSGamebryoSequenceGenerator"
};

const QStringList StateMachineUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

StateMachineUI::StateMachineUI()
    : transitionsButtonRow(INITIAL_ADD_TRANSITION_ROW),
      behaviorView(nullptr),
      bsData(nullptr),
      groupBox(new QGroupBox("hkbStateMachine")),
      topLyt(new QGridLayout),
      eventToSendWhenStateOrTransitionChanges(new CheckButtonCombo("Edit")),
      eventUI(new EventUI),
      stateUI(new StateUI),
      transitionUI(new TransitionsUI),
      table(new TableWidget(QColor(Qt::white))),
      typeSelectorCB(new ComboBox),
      name(new LineEdit),
      startStateId(new ComboBox),
      wrapAroundStateId(new CheckBox),
      maxSimultaneousTransitions(new SpinBox),
      startStateMode(new ComboBox),
      selfTransitionMode(new ComboBox)
{
    table->setAcceptDrops(true);
    table->viewport()->setAcceptDrops(true);
    table->setDragDropOverwriteMode(true);
    table->setDropIndicatorShown(true);
    table->setDragDropMode(QAbstractItemView::InternalMove);
    table->setRowSwapRange(INITIAL_ADD_TRANSITION_ROW);
    typeSelectorCB->insertItems(0, types);
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(EVENT_TO_SEND_WHEN_STATE_OR_TRANSITION_CHANGES_ROW, NAME_COLUMN, new TableWidgetItem("eventToSendWhenStateOrTransitionChanges"));
    table->setItem(EVENT_TO_SEND_WHEN_STATE_OR_TRANSITION_CHANGES_ROW, TYPE_COLUMN, new TableWidgetItem("hkEvent", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_WHEN_STATE_OR_TRANSITION_CHANGES_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_WHEN_STATE_OR_TRANSITION_CHANGES_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::gray)));
    table->setCellWidget(EVENT_TO_SEND_WHEN_STATE_OR_TRANSITION_CHANGES_ROW, VALUE_COLUMN, eventToSendWhenStateOrTransitionChanges);
    table->setItem(START_STATE_ID_ROW, NAME_COLUMN, new TableWidgetItem("startStateId"));
    table->setItem(START_STATE_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(START_STATE_ID_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(START_STATE_ID_ROW, VALUE_COLUMN, startStateId);
    table->setItem(RETURN_TO_PREVIOUS_STATE_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("returnToPreviousStateEventId"));
    table->setItem(RETURN_TO_PREVIOUS_STATE_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(RETURN_TO_PREVIOUS_STATE_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(RETURN_TO_PREVIOUS_STATE_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(RANDOM_TRANSITION_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("randomTransitionEventId"));
    table->setItem(RANDOM_TRANSITION_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(RANDOM_TRANSITION_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(RANDOM_TRANSITION_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(TRANSITION_TO_NEXT_HIGHER_STATE_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("transitionToNextHigherStateEventId"));
    table->setItem(TRANSITION_TO_NEXT_HIGHER_STATE_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(TRANSITION_TO_NEXT_HIGHER_STATE_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(TRANSITION_TO_NEXT_HIGHER_STATE_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(TRANSITION_TO_NEXT_LOWER_STATE_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("transitionToNextLowerStateEventId"));
    table->setItem(TRANSITION_TO_NEXT_LOWER_STATE_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(TRANSITION_TO_NEXT_LOWER_STATE_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(TRANSITION_TO_NEXT_LOWER_STATE_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(SYNC_VARIABLE_INDEX_ROW, NAME_COLUMN, new TableWidgetItem("syncVariableIndex"));
    table->setItem(SYNC_VARIABLE_INDEX_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(SYNC_VARIABLE_INDEX_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(SYNC_VARIABLE_INDEX_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP));
    table->setItem(WRAP_AROUND_STATE_ID_ROW, NAME_COLUMN, new TableWidgetItem("wrapAroundStateId"));
    table->setItem(WRAP_AROUND_STATE_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(WRAP_AROUND_STATE_ID_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setItem(WRAP_AROUND_STATE_ID_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(WRAP_AROUND_STATE_ID_ROW, VALUE_COLUMN, wrapAroundStateId);
    table->setItem(MAX_SIMULTANEOUS_TRANSITIONS_ROW, NAME_COLUMN, new TableWidgetItem("maxSimultaneousTransitions"));
    table->setItem(MAX_SIMULTANEOUS_TRANSITIONS_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(MAX_SIMULTANEOUS_TRANSITIONS_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(MAX_SIMULTANEOUS_TRANSITIONS_ROW, VALUE_COLUMN, maxSimultaneousTransitions);
    maxSimultaneousTransitions->setMaximum(32);
    maxSimultaneousTransitions->setMinimum(0);
    table->setItem(START_STATE_MODE_ROW, NAME_COLUMN, new TableWidgetItem("startStateMode"));
    table->setItem(START_STATE_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("StartStateMode", Qt::AlignCenter));
    table->setItem(START_STATE_MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(START_STATE_MODE_ROW, VALUE_COLUMN, startStateMode);
    table->setItem(SELF_TRANSITION_MODE_ROW, NAME_COLUMN, new TableWidgetItem("selfTransitionMode"));
    table->setItem(SELF_TRANSITION_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("SelfTransitionMode", Qt::AlignCenter));
    table->setItem(SELF_TRANSITION_MODE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(SELF_TRANSITION_MODE_ROW, VALUE_COLUMN, selfTransitionMode);
    table->setItem(ADD_STATE_ROW, NAME_COLUMN, new TableWidgetItem("Add State With Generator", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new state with a generator of the type specified in the adjacent combo box"));
    table->setCellWidget(ADD_STATE_ROW, TYPE_COLUMN, typeSelectorCB);
    table->setItem(ADD_STATE_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected State", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected state"));
    table->setItem(ADD_STATE_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected State", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected state"));
    table->setItem(INITIAL_ADD_TRANSITION_ROW, NAME_COLUMN, new TableWidgetItem("Add Transition", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new transition"));
    table->setItem(INITIAL_ADD_TRANSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStateMachineTransitionInfoArray", Qt::AlignCenter, QColor(Qt::gray)));
    table->setItem(INITIAL_ADD_TRANSITION_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Transition", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to remove the selected transition"));
    table->setItem(INITIAL_ADD_TRANSITION_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Transition", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black), "Double click to edit the selected transition"));
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(stateUI);
    addWidget(transitionUI);
    addWidget(eventUI);
    toggleSignals(true);
}

void StateMachineUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(wrapAroundStateId, SIGNAL(clicked(bool)), this, SLOT(setWrapAroundStateId(bool)), Qt::UniqueConnection);
        connect(maxSimultaneousTransitions, SIGNAL(editingFinished()), this, SLOT(setMaxSimultaneousTransitions()), Qt::UniqueConnection);
        connect(startStateId, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartStateId(int)), Qt::UniqueConnection);
        connect(startStateMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartStateMode(int)), Qt::UniqueConnection);
        connect(selfTransitionMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setSelfTransitionMode(int)), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)), Qt::UniqueConnection);
        connect(eventToSendWhenStateOrTransitionChanges, SIGNAL(pressed()), this, SLOT(viewEventToSendWhenStateOrTransitionChanges()), Qt::UniqueConnection);
        connect(eventToSendWhenStateOrTransitionChanges, SIGNAL(enabled(bool)), this, SLOT(toggleEventToSendWhenStateOrTransitionChanges(bool)), Qt::UniqueConnection);
        connect(eventUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(eventUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
        connect(stateUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
        connect(stateUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(stateUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)), Qt::UniqueConnection);
        connect(stateUI, SIGNAL(returnToParent(bool)), this, SLOT(returnToWidget(bool)), Qt::UniqueConnection);
        connect(stateUI, SIGNAL(viewGenerators(int,QString,QStringList)), this, SIGNAL(viewGenerators(int,QString,QStringList)), Qt::UniqueConnection);
        connect(stateUI, SIGNAL(stateNameChanged(QString,int)), this, SLOT(stateRenamed(QString,int)), Qt::UniqueConnection);
        connect(stateUI, SIGNAL(stateIdChanged(int,int,QString)), this, SLOT(setStateIDForRow(int,int,QString)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(transitionNameChanged(QString,int)), this, SLOT(transitionRenamed(QString,int)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(wrapAroundStateId, SIGNAL(clicked(bool)), this, SLOT(setWrapAroundStateId(bool)));
        disconnect(maxSimultaneousTransitions, SIGNAL(editingFinished()), this, SLOT(setMaxSimultaneousTransitions()));
        disconnect(startStateId, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartStateId(int)));
        disconnect(startStateMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartStateMode(int)));
        disconnect(selfTransitionMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setSelfTransitionMode(int)));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(table, SIGNAL(itemDropped(int,int)), this, SLOT(swapGeneratorIndices(int,int)));
        disconnect(eventToSendWhenStateOrTransitionChanges, SIGNAL(pressed()), this, SLOT(viewEventToSendWhenStateOrTransitionChanges()));
        disconnect(eventToSendWhenStateOrTransitionChanges, SIGNAL(enabled(bool)), this, SLOT(toggleEventToSendWhenStateOrTransitionChanges(bool)));
        disconnect(eventUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(eventUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
        disconnect(stateUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
        disconnect(stateUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(stateUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)));
        disconnect(stateUI, SIGNAL(returnToParent(bool)), this, SLOT(returnToWidget(bool)));
        disconnect(stateUI, SIGNAL(viewGenerators(int,QString,QStringList)), this, SIGNAL(viewGenerators(int,QString,QStringList)));
        disconnect(stateUI, SIGNAL(stateNameChanged(QString,int)), this, SLOT(stateRenamed(QString,int)));
        disconnect(stateUI, SIGNAL(stateIdChanged(int,int,QString)), this, SLOT(setStateIDForRow(int,int,QString)));
        disconnect(transitionUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(transitionUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(transitionUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)));
        disconnect(transitionUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
        disconnect(transitionUI, SIGNAL(transitionNameChanged(QString,int)), this, SLOT(transitionRenamed(QString,int)));
    }
}

void StateMachineUI::loadData(HkxObject *data){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data && data->getSignature() == HKB_STATE_MACHINE){
        bsData = static_cast<hkbStateMachine *>(data);
        QStringList statenames("None");
        statenames = statenames + bsData->getStateNames();
        name->setText(bsData->getName());
        (bsData->getEventToSendWhenStateOrTransitionChangesID() > -1) ? eventToSendWhenStateOrTransitionChanges->setChecked(true) : eventToSendWhenStateOrTransitionChanges->setChecked(false);
        startStateId->clear();
        startStateId->insertItems(0, statenames);
        startStateId->setCurrentIndex(startStateId->findText(bsData->getStateName(bsData->getStartStateId()), Qt::MatchCaseSensitive));
        auto varBind = bsData->getVariableBindingSetData();
        UIHelper::loadBinding(START_STATE_ID_ROW, BINDING_COLUMN, varBind, "startStateId", table, bsData);
        UIHelper::loadBinding(WRAP_AROUND_STATE_ID_ROW, BINDING_COLUMN, varBind, "wrapAroundStateId", table, bsData);
        auto eventname = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->returnToPreviousStateEventId);
        auto labeleventnamme = [&](int row, const QString & name){
            (name != "") ? table->item(row, VALUE_COLUMN)->setText(name) : table->item(row, VALUE_COLUMN)->setText("None");
        };
        labeleventnamme(RETURN_TO_PREVIOUS_STATE_EVENT_ID_ROW, eventname);
        eventname = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->randomTransitionEventId);
        labeleventnamme(RANDOM_TRANSITION_EVENT_ID_ROW, eventname);
        eventname = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->transitionToNextHigherStateEventId);
        labeleventnamme(TRANSITION_TO_NEXT_HIGHER_STATE_EVENT_ID_ROW, eventname);
        eventname = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->transitionToNextLowerStateEventId);
        labeleventnamme(TRANSITION_TO_NEXT_LOWER_STATE_EVENT_ID_ROW, eventname);
        eventname = static_cast<BehaviorFile *>(bsData->getParentFile())->getVariableNameAt(bsData->syncVariableIndex);
        labeleventnamme(SYNC_VARIABLE_INDEX_ROW, eventname);
        wrapAroundStateId->setChecked(bsData->getWrapAroundStateId());
        maxSimultaneousTransitions->setValue(bsData->getMaxSimultaneousTransitions());
        (!startStateMode->count()) ? startStateMode->insertItems(0, bsData->StartStateMode) : NULL;
        startStateMode->setCurrentIndex(bsData->StartStateMode.indexOf(bsData->startStateMode));
        (!selfTransitionMode->count()) ? selfTransitionMode->insertItems(0, bsData->SelfTransitionMode) : NULL;
        selfTransitionMode->setCurrentIndex(bsData->SelfTransitionMode.indexOf(bsData->getSelfTransitionMode()));
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("StateMachineUI::loadData(): The data is nullptr or an incorrect type!!");
    }
    toggleSignals(true);
}

void StateMachineUI::loadDynamicTableRows(){
    if (bsData){
        auto temp = ADD_STATE_ROW + bsData->getNumberOfStates() + 1 - transitionsButtonRow;
        if (temp > 0){
            for (auto i = 0; i < temp; i++){
                table->insertRow(transitionsButtonRow);
                transitionsButtonRow++;
            }
        }else if (temp < 0){
            for (auto i = temp; i < 0; i++){
                table->removeRow(transitionsButtonRow - 1);
                transitionsButtonRow--;
            }
        }
        transitionsButtonRow = ADD_STATE_ROW + bsData->getNumberOfStates() + 1;
        for (auto i = INITIAL_ADD_TRANSITION_ROW, j = 0; i < transitionsButtonRow, j < bsData->getNumberOfStates(); i++, j++){
            auto state = static_cast<hkbStateMachineStateInfo *>(bsData->states.at(j).data());
            if (state){
                UIHelper::setRowItems(i, state->getName()+" ->ID: "+QString::number(state->getStateId()), state->getClassname(), "Remove", "Edit", "Double click to remove this state", "Double click to edit this state", table);
            }else{
                LogFile::writeToLog("StateMachineUI::loadData(): Null state found!!!");
            }
        }
        auto trans = static_cast<hkbStateMachineTransitionInfoArray *>(bsData->wildcardTransitions.data());
        if (trans){
            table->setRowCount(transitionsButtonRow + trans->getNumTransitions() + 1);
            for (auto i = transitionsButtonRow + 1, j = 0; i < table->rowCount(), j < trans->getNumTransitions(); i++, j++){
                UIHelper::setRowItems(i, trans->getTransitionNameAt(j), trans->getClassname(), "Remove", "Edit", "Double click to remove this transition", "Double click to edit this transition", table);
            }
        }else{
            table->setRowCount(transitionsButtonRow + 1);
        }
    }else{
        LogFile::writeToLog("StateMachineUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void StateMachineUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit generatorNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("StateMachineUI::setName(): The data is nullptr!!");
    }
}

void StateMachineUI::setReturnToPreviousStateEventId(int index, const QString & name){
    (bsData) ? bsData->setReturnToPreviousStateEventId(index), table->item(RETURN_TO_PREVIOUS_STATE_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : LogFile::writeToLog("StateMachineUI::setReturnToPreviousStateEventId(): The data is nullptr!!");
}

void StateMachineUI::setStartStateId(int index){
    (bsData) ? bsData->setStartStateId(index) : LogFile::writeToLog("StateMachineUI::setStartStateId(): The data is nullptr!!");
}

void StateMachineUI::setRandomTransitionEventId(int index, const QString & name){
    (bsData) ? bsData->setRandomTransitionEventId(index), table->item(RANDOM_TRANSITION_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : LogFile::writeToLog("StateMachineUI::setRandomTransitionEventId(): The data is nullptr!!");
}

void StateMachineUI::setTransitionToNextHigherStateEventId(int index, const QString &name){
    (bsData) ? bsData->setTransitionToNextHigherStateEventId(index), table->item(TRANSITION_TO_NEXT_HIGHER_STATE_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : LogFile::writeToLog("StateMachineUI::setTransitionToNextHigherStateEventId(): The data is nullptr!!");
}

void StateMachineUI::setTransitionToNextLowerStateEventId(int index, const QString &name){
    (bsData) ? bsData->setTransitionToNextLowerStateEventId(index), table->item(TRANSITION_TO_NEXT_LOWER_STATE_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : LogFile::writeToLog("StateMachineUI::setTransitionToNextLowerStateEventId(): The data is nullptr!!");
}

void StateMachineUI::setSyncVariableIndex(int index, const QString &name){
    (bsData) ? bsData->setSyncVariableIndex(index), table->item(SYNC_VARIABLE_INDEX_ROW, VALUE_COLUMN)->setText(name) : LogFile::writeToLog("StateMachineUI::setSyncVariableIndex(): The data is nullptr!!");
}

void StateMachineUI::setWrapAroundStateId(bool checked){
    (bsData) ? bsData->setWrapAroundStateId(checked) : LogFile::writeToLog("StateMachineUI::setWrapAroundStateId(): The data is nullptr!!");
}

void StateMachineUI::setMaxSimultaneousTransitions(){
    (bsData) ? bsData->setMaxSimultaneousTransitions(maxSimultaneousTransitions->value()) : LogFile::writeToLog("StateMachineUI::setMaxSimultaneousTransitions(): The data is nullptr!!");
}

void StateMachineUI::setStartStateMode(int index){
    (bsData) ? bsData->setStartStateMode(index) : LogFile::writeToLog("StateMachineUI::setStartStateMode(): The data is nullptr!!");
}

void StateMachineUI::setSelfTransitionMode(int index){
    (bsData) ? bsData->setSelfTransitionMode(index) : LogFile::writeToLog("StateMachineUI::setSelfTransitionMode(): The data is nullptr!!");
}

void StateMachineUI::swapGeneratorIndices(int index1, int index2){
    if (bsData){
        index1 = index1 - BASE_NUMBER_OF_ROWS;
        index2 = index2 - BASE_NUMBER_OF_ROWS;
        if (!bsData->swapChildren(index1, index2)){
            WARNING_MESSAGE("Cannot swap these rows!!");
        }else{  //TO DO: check if necessary...
            (behaviorView->getSelectedItem()) ? behaviorView->getSelectedItem()->reorderChildren() : LogFile::writeToLog("BlenderGeneratorUI::swapGeneratorIndices(): No item selected!!");
        }
    }else{
        LogFile::writeToLog("BlenderGeneratorUI::swapGeneratorIndices(): The data is nullptr!!");
    }
}

void StateMachineUI::setStateIDForRow(int index, int newID, const QString &statename){
    auto row = ADD_STATE_ROW + index;
    (table->item(row, NAME_COLUMN)) ? table->item(row, NAME_COLUMN)->setText(statename+" ->ID: "+QString::number(newID)) : LogFile::writeToLog("StateMachineUI::setStateIDForRow(): Unwanted state id change event heard!!");
}

void StateMachineUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case START_STATE_ID_ROW:
            checkisproperty(START_STATE_ID_ROW, "startStateId", VARIABLE_TYPE_INT32); break;
        case SYNC_VARIABLE_INDEX_ROW:
            checkisproperty(SYNC_VARIABLE_INDEX_ROW, "syncVariableIndex", VARIABLE_TYPE_INT32); break;
        case WRAP_AROUND_STATE_ID_ROW:
            checkisproperty(WRAP_AROUND_STATE_ID_ROW, "wrapAroundStateId", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("StateMachineUI::setBindingVariable(): The data is nullptr!!");
    }
}

void StateMachineUI::viewEventToSendWhenStateOrTransitionChanges(){
    if (bsData){
        eventUI->loadData(static_cast<BehaviorFile *>(bsData->getParentFile()), &bsData->eventToSendWhenStateOrTransitionChanges);
        setCurrentIndex(EVENT_PAYLOAD_WIDGET);
    }else{
        LogFile::writeToLog("StateMachineUI::viewEventToSendWhenStateOrTransitionChanges(): The data is nullptr!!");
    }
}

void StateMachineUI::toggleEventToSendWhenStateOrTransitionChanges(bool enable){
    if (bsData){
        if (!enable){
            bsData->eventToSendWhenStateOrTransitionChanges.id = -1;
            bsData->eventToSendWhenStateOrTransitionChanges.payload = HkxSharedPtr();
            static_cast<BehaviorFile *>(bsData->getParentFile())->removeOtherData();
        }
    }else{
        LogFile::writeToLog("StateMachineUI::toggleEventToSendWhenStateOrTransitionChanges(): The data is nullptr!!");
    }
}

void StateMachineUI::stateRenamed(const QString &name, int stateIndex){
    if (bsData){
        auto item = table->item(INITIAL_ADD_TRANSITION_ROW + stateIndex, NAME_COLUMN);
        (item) ? item->setText(name) : LogFile::writeToLog("StateMachineUI::stateRenamed(): Invalid row selected!!");
    }else{
        LogFile::writeToLog("StateMachineUI::stateRenamed(): The data is nullptr!!");
    }
}

void StateMachineUI::transitionRenamed(const QString &name, int index){
    if (bsData){
        auto item = table->item(transitionsButtonRow + index + 1, NAME_COLUMN);
        (item) ? item->setText(name) : LogFile::writeToLog("StateMachineUI::transitionRenamed(): Invalid row selected!!");
    }else{
        LogFile::writeToLog("StateMachineUI::transitionRenamed(): The data is nullptr!!");
    }
}

void StateMachineUI::selectTableToView(bool viewproperties, const QString & path){
    if (bsData){
        if (viewproperties){
            if (bsData->getVariableBindingSetData()){
                emit viewProperties(bsData->getVariableBindingSetData()->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewProperties(0, QString(), QStringList());
            }
        }else{
            if (bsData->getVariableBindingSetData()){
                emit viewVariables(bsData->getVariableBindingSetData()->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewVariables(0, QString(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("StateMachineUI::selectTableToView(): The data is nullptr!!");
    }
}

void StateMachineUI::viewSelectedChild(int row, int column){
    auto checkisproperty = [&](int row, const QString & fieldname){
        bool properties;
        (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
        selectTableToView(properties, fieldname);
    };
    if (bsData){
        if (row < ADD_STATE_ROW && row >= 0){
            if (column == BINDING_COLUMN){
                switch (row){
                case START_STATE_ID_ROW:
                    checkisproperty(START_STATE_ID_ROW, "startStateId"); break;
                case WRAP_AROUND_STATE_ID_ROW:
                    checkisproperty(WRAP_AROUND_STATE_ID_ROW, "wrapAroundStateId"); break;
                }
            }else if (column == VALUE_COLUMN){
                if (row == EVENT_TO_SEND_WHEN_STATE_OR_TRANSITION_CHANGES_ROW){
                    eventUI->loadData(static_cast<BehaviorFile *>(bsData->getParentFile()), &bsData->eventToSendWhenStateOrTransitionChanges);
                    setCurrentIndex(EVENT_PAYLOAD_WIDGET);
                }else{
                    auto index = -1;
                    switch (row){
                    case EVENT_TO_SEND_WHEN_STATE_OR_TRANSITION_CHANGES_ROW:
                        index = bsData->getEventToSendWhenStateOrTransitionChangesID();
                        break;
                    case RETURN_TO_PREVIOUS_STATE_EVENT_ID_ROW:
                        index = bsData->getReturnToPreviousStateEventId();
                        break;
                    case RANDOM_TRANSITION_EVENT_ID_ROW:
                        index = bsData->getRandomTransitionEventId();
                        break;
                    case TRANSITION_TO_NEXT_HIGHER_STATE_EVENT_ID_ROW:
                        index = bsData->getTransitionToNextHigherStateEventId();
                        break;
                    case TRANSITION_TO_NEXT_LOWER_STATE_EVENT_ID_ROW:
                        index = bsData->getTransitionToNextLowerStateEventId();
                        break;
                    case SYNC_VARIABLE_INDEX_ROW:
                        index = bsData->getSyncVariableIndex();
                        emit viewVariables(++index, QString(), QStringList());
                    default:
                        return;
                    }
                    emit viewEvents(++index, QString(), QStringList());
                }
            }
        }else if (row == ADD_STATE_ROW && column == NAME_COLUMN){
            addStateWithGenerator();
        }else if (row == transitionsButtonRow && column == NAME_COLUMN){
            addTransition();
        }else if (row > ADD_STATE_ROW && row < transitionsButtonRow){
            auto result = row - BASE_NUMBER_OF_ROWS + 1;
            if (bsData->getNumberOfStates() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    stateUI->loadData(static_cast<hkbStateMachineStateInfo *>(bsData->states.at(result).data()), result);
                    setCurrentIndex(STATE_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the state \""+static_cast<hkbStateMachineStateInfo *>(bsData->states.at(result).data())->getName()+"\"?") == QMessageBox::Yes){
                        removeState(result);
                    }
                }
            }else{
                LogFile::writeToLog("StateMachineUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }else if (row > transitionsButtonRow && row < table->rowCount()){
            auto result = row - BASE_NUMBER_OF_ROWS - bsData->getNumberOfStates();
            auto trans = static_cast<hkbStateMachineTransitionInfoArray *>(bsData->wildcardTransitions.data());
            if (trans && result < trans->getNumTransitions() && result >= 0){
                if (column == VALUE_COLUMN){
                    transitionUI->loadData(static_cast<BehaviorFile *>(bsData->getParentFile()), bsData, trans->getTransitionAt(result), result);
                    setCurrentIndex(TRANSITION_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the transition \""+trans->getTransitionNameAt(result)+"\"?") == QMessageBox::Yes){
                        removeTransition(result);
                    }
                }
            }else{
                LogFile::writeToLog("StateMachineUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }
    }else{
        LogFile::writeToLog("StateMachineUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void StateMachineUI::eventTableElementSelected(int index, const QString &name){
    index--;
    switch (currentIndex()){
    case MAIN_WIDGET:
        switch (table->currentRow()){
        case RETURN_TO_PREVIOUS_STATE_EVENT_ID_ROW:
            setReturnToPreviousStateEventId(index, name); break;
        case RANDOM_TRANSITION_EVENT_ID_ROW:
            setRandomTransitionEventId(index, name); break;
        case TRANSITION_TO_NEXT_HIGHER_STATE_EVENT_ID_ROW:
            setTransitionToNextHigherStateEventId(index, name); break;
        case TRANSITION_TO_NEXT_LOWER_STATE_EVENT_ID_ROW:
            setTransitionToNextLowerStateEventId(index, name); break;
        default:
            WARNING_MESSAGE("StateMachineUI::eventTableElementSelected(): An unwanted element selected event was recieved!!");
            return;
        }
        break;
    case EVENT_PAYLOAD_WIDGET:
        eventUI->setEvent(index, name); break;
    case STATE_WIDGET:
        stateUI->eventTableElementSelected(index, name); break;
    case TRANSITION_WIDGET:
        transitionUI->eventTableElementSelected(index, name); break;
    default:
        WARNING_MESSAGE("StateMachineUI::eventTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void StateMachineUI::variableTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        if (table->currentRow() == SYNC_VARIABLE_INDEX_ROW){
            (bsData) ? bsData->setSyncVariableIndex(index - 1) : LogFile::writeToLog("StateMachineUI::variableTableElementSelected(): The data is nullptr!!");
        }else{
            setBindingVariable(index, name);
        }
        break;
    case STATE_WIDGET:
        stateUI->variableTableElementSelected(index, name); break;
    case TRANSITION_WIDGET:
        transitionUI->variableTableElementSelected(index, name); break;
    default:
        WARNING_MESSAGE("StateMachineUI::variableTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void StateMachineUI::generatorTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case STATE_WIDGET:
        stateUI->setGenerator(index, name); break;
    default:
        WARNING_MESSAGE("StateMachineUI::generatorTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void StateMachineUI::generatorRenamed(const QString &name, int index){
    switch (currentIndex()){
    case STATE_WIDGET:
        stateUI->generatorRenamed(name, index); break;
    default:
        WARNING_MESSAGE("StateMachineUI::generatorTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void StateMachineUI::addStateWithGenerator(){
    if (bsData && behaviorView){
        auto typeEnum = static_cast<Generator_Type>(typeSelectorCB->currentIndex());
        behaviorView->appendState();
        switch (typeEnum){
        case STATE_MACHINE:
            behaviorView->appendStateMachine(); break;
        case MANUAL_SELECTOR_GENERATOR:
            behaviorView->appendManualSelectorGenerator(); break;
        case BLENDER_GENERATOR:
            behaviorView->appendBlenderGenerator(); break;
        case I_STATE_TAGGING_GENERATOR:
            behaviorView->appendIStateTaggingGenerator(); break;
        case BONE_SWITCH_GENERATOR:
            behaviorView->appendBoneSwitchGenerator(); break;
        case CYCLIC_BLEND_TRANSITION_GENERATOR:
            behaviorView->appendCyclicBlendTransitionGenerator(); break;
        case SYNCHRONIZED_CLIP_GENERATOR:
            behaviorView->appendSynchronizedClipGenerator(); break;
        case MODIFIER_GENERATOR:
            behaviorView->appendModifierGenerator(); break;
        case OFFSET_ANIMATION_GENERATOR:
            behaviorView->appendOffsetAnimationGenerator(); break;
        case POSE_MATCHING_GENERATOR:
            behaviorView->appendPoseMatchingGenerator(); break;
        case CLIP_GENERATOR:
            behaviorView->appendClipGenerator(); break;
        case BEHAVIOR_REFERENCE_GENERATOR:
            behaviorView->appendBehaviorReferenceGenerator(); break;
        case GAMEBYRO_SEQUENCE_GENERATOR:
            behaviorView->appendBGSGamebryoSequenceGenerator(); break;
        default:
            LogFile::writeToLog("StateMachineUI::addNewStateWithGenerator(): Invalid typeEnum!!");
            return;
        }
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("StateMachineUI::addNewStateWithGenerator(): The data is nullptr!!");
    }
}

void StateMachineUI::removeState(int index){
    auto currentindex = -1;
    if (bsData && behaviorView){
        if (index < bsData->states.size() && index >= 0){
            behaviorView->removeItemFromGraph(behaviorView->getSelectedIconsChildIcon(bsData->states.at(index).data()), index);
            behaviorView->removeObjects();
        }else{
            WARNING_MESSAGE("StateMachineUI::removeState(): Invalid index of child to remove!!");
        }
        disconnect(startStateId, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartStateId(int)));
        currentindex = startStateId->currentIndex();
        startStateId->removeItem(index);
        (currentindex == index) ? startStateId->setCurrentIndex(0) : NULL;
        connect(startStateId, SIGNAL(currentIndexChanged(int)), this, SLOT(setStartStateId(int)), Qt::UniqueConnection);
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("StateMachineUI::removeState(): The data is nullptr!!");
    }
}

void StateMachineUI::removeTransition(int index){
    if (bsData){
        auto trans = static_cast<hkbStateMachineTransitionInfoArray *>(bsData->wildcardTransitions.data());
        if (trans){
            if (index < trans->getNumTransitions() && index >= 0){
                trans->removeTransition(index);
                loadDynamicTableRows();
            }else{
                WARNING_MESSAGE("StateMachineUI::removeTransition(): Invalid row index selected!!");
            }
        }else{
            WARNING_MESSAGE("StateMachineUI::removeTransition(): Transition data is nullptr!!");
        }
    }else{
        LogFile::writeToLog("StateMachineUI::removeTransition(): The data is nullptr!!");
    }
}

void StateMachineUI::addTransition(){
    if (bsData){
        auto trans = static_cast<hkbStateMachineTransitionInfoArray *>(bsData->wildcardTransitions.data());
        if (!trans){
            trans = new hkbStateMachineTransitionInfoArray(bsData->getParentFile(), bsData, -1);
            bsData->wildcardTransitions = HkxSharedPtr(trans);
        }
        trans->addTransition();
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("StateMachineUI::addTransition(): The data is nullptr!!");
    }
}

void StateMachineUI::returnToWidget(bool reloadData){
    (reloadData) ? loadData(bsData) : NULL;
    setCurrentIndex(MAIN_WIDGET);
}

void StateMachineUI::returnToWidget(){
    setCurrentIndex(MAIN_WIDGET);
}

void StateMachineUI::connectToTables(GenericTableWidget *generators, GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (generators && variables && events && properties){
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(generators, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(eventTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(variableTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(generators, SIGNAL(elementSelected(int,QString)), this, SLOT(generatorTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewGenerators(int,QString,QStringList)), generators, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("StateMachineUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void StateMachineUI::variableRenamed(const QString & name, int index){
    if (bsData){
        if (name != ""){
            index--;
            auto bind = bsData->getVariableBindingSetData();
            if (bind){
                auto setname = [&](const QString & fieldname, int row){
                    auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                    (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
                };
                setname("startStateId", START_STATE_ID_ROW);
                setname("syncVariableIndex", SYNC_VARIABLE_INDEX_ROW);
                setname("wrapAroundStateId", WRAP_AROUND_STATE_ID_ROW);
            }
        }
    }else{
        LogFile::writeToLog("StateMachineUI::variableRenamed(): The data is nullptr!!");
    }
}

void StateMachineUI::eventRenamed(const QString & name, int index){
    if (bsData){
        if (currentIndex() == MAIN_WIDGET){
            auto checkindex = [&](int eventid, int row){
                (index == eventid) ? table->item(row, VALUE_COLUMN)->setText(name) : NULL;
            };
            checkindex(bsData->getReturnToPreviousStateEventId(), RETURN_TO_PREVIOUS_STATE_EVENT_ID_ROW);
            checkindex(bsData->getRandomTransitionEventId(), RANDOM_TRANSITION_EVENT_ID_ROW);
            checkindex(bsData->getTransitionToNextHigherStateEventId(), TRANSITION_TO_NEXT_HIGHER_STATE_EVENT_ID_ROW);
            checkindex(bsData->getTransitionToNextLowerStateEventId(), TRANSITION_TO_NEXT_LOWER_STATE_EVENT_ID_ROW);
        }else{
            eventUI->eventRenamed(name, index);
        }
    }else{
        LogFile::writeToLog("StateMachineUI::eventRenamed(): The data is nullptr!!");
    }
}

void StateMachineUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
    setCurrentIndex(MAIN_WIDGET);
    stateUI->setBehaviorView(view);
}

