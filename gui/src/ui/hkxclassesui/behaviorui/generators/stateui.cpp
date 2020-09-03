#include "stateui.h"
#include "StateUI.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachine.h"
#include "src/hkxclasses/behavior/generators/hkbblendergeneratorchild.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachinestateinfo.h"
#include "src/hkxclasses/behavior/generators/bsboneswitchgeneratorbonedata.h"
#include "src/hkxclasses/behavior/hkbstatemachineeventpropertyarray.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/ui/genericdatawidgets.h"
#include "src/ui/hkxclassesui/behaviorui/transitionsui.h"
#include "src/ui/hkxclassesui/behaviorui/eventui.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/behaviorgraphview.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/mainwindow.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"

#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 8

#define NAME_ROW 0
#define STATE_ID_ROW 1
#define PROBABILITY_ROW 2
#define ENABLE_ROW 3
#define GENERATOR_ROW 4
#define ADD_ENTER_EVENT_ROW 5
#define INITIAL_EXIT_EVENT_ROW 6
#define INITIAL_ADD_TRANSITION_ROW 7

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList StateUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

StateUI::StateUI()
    : behaviorView(nullptr),
      bsData(nullptr),
      stateIndex(-1),
      exitEventsButtonRow(INITIAL_EXIT_EVENT_ROW),
      transitionsButtonRow(INITIAL_ADD_TRANSITION_ROW),
      groupBox(new QGroupBox("hkbStateMachineStateInfo")),
      topLyt(new QGridLayout),
      transitionUI(new TransitionsUI()),
      eventUI(new EventUI()),
      table(new TableWidget(QColor(Qt::white))),
      returnPB(new QPushButton("Return")),
      name(new LineEdit),
      stateId(new SpinBox),
      probability(new DoubleSpinBox),
      enable(new CheckBox),
      enableTransitions(new CheckBox("Enable Transitions"))
{
    //stateId->setReadOnly(true);
    transitionUI->flagGlobalWildcard->setDisabled(true);
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(STATE_ID_ROW, NAME_COLUMN, new TableWidgetItem("id"));
    table->setItem(STATE_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(STATE_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(STATE_ID_ROW, VALUE_COLUMN, stateId);
    table->setItem(PROBABILITY_ROW, NAME_COLUMN, new TableWidgetItem("probability"));
    table->setItem(PROBABILITY_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(PROBABILITY_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(PROBABILITY_ROW, VALUE_COLUMN, probability);
    table->setItem(ENABLE_ROW, NAME_COLUMN, new TableWidgetItem("enable"));
    table->setItem(ENABLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(ENABLE_ROW, VALUE_COLUMN, enable);
    table->setItem(GENERATOR_ROW, NAME_COLUMN, new TableWidgetItem("generator"));
    table->setItem(GENERATOR_ROW, TYPE_COLUMN, new TableWidgetItem("hkbGenerator", Qt::AlignCenter));
    table->setItem(GENERATOR_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(GENERATOR_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_GENERATORS_TABLE_TIP));
    table->setItem(ADD_ENTER_EVENT_ROW, NAME_COLUMN, new TableWidgetItem("Add Enter Event", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a event to be raised when this state is entered"));
    table->setItem(ADD_ENTER_EVENT_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStateMachineEventPropertyArray", Qt::AlignCenter, QColor(Qt::gray)));
    table->setItem(ADD_ENTER_EVENT_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Enter Event", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black)));
    table->setItem(ADD_ENTER_EVENT_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Enter Event", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black)));
    table->setItem(INITIAL_EXIT_EVENT_ROW, NAME_COLUMN, new TableWidgetItem("Add Exit Event", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a event to be raised when this state is exited"));
    table->setItem(INITIAL_EXIT_EVENT_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStateMachineEventPropertyArray", Qt::AlignCenter, QColor(Qt::gray)));
    table->setItem(INITIAL_EXIT_EVENT_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Exit Event", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black)));
    table->setItem(INITIAL_EXIT_EVENT_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Exit Event", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black)));
    table->setItem(INITIAL_ADD_TRANSITION_ROW, NAME_COLUMN, new TableWidgetItem("Add Transition", Qt::AlignCenter, QColor(Qt::green), QBrush(Qt::black), "Double click to add a new transition"));
    table->setItem(INITIAL_ADD_TRANSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStateMachineTransitionInfoArray", Qt::AlignCenter, QColor(Qt::gray)));
    table->setItem(INITIAL_ADD_TRANSITION_ROW, BINDING_COLUMN, new TableWidgetItem("Remove Selected Transition", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black)));
    table->setItem(INITIAL_ADD_TRANSITION_ROW, VALUE_COLUMN, new TableWidgetItem("Edit Selected Transition", Qt::AlignCenter, QColor(Qt::gray), QBrush(Qt::black)));
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 8, 3);
    groupBox->setLayout(topLyt);
    //Order here must correspond with the ACTIVE_WIDGET Enumerated type!!!
    addWidget(groupBox);
    addWidget(eventUI);
    addWidget(transitionUI);
    toggleSignals(true);
}

void StateUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(clicked(bool)), this, SIGNAL(returnToParent(bool)), Qt::UniqueConnection);
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(stateId, SIGNAL(valueChanged(int)), this, SLOT(setStateId(int)), Qt::UniqueConnection);
        connect(probability, SIGNAL(editingFinished()), this, SLOT(setProbability()), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
        connect(eventUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(eventUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()), Qt::UniqueConnection);
        connect(transitionUI, SIGNAL(transitionNameChanged(QString,int)), this, SLOT(transitionRenamed(QString,int)), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(clicked(bool)), this, SIGNAL(returnToParent(bool)));
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(stateId, SIGNAL(valueChanged(int)), this, SLOT(setStateId(int)));
        disconnect(probability, SIGNAL(editingFinished()), this, SLOT(setProbability()));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
        disconnect(eventUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(eventUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
        disconnect(transitionUI, SIGNAL(viewEvents(int,QString,QStringList)), this, SIGNAL(viewEvents(int,QString,QStringList)));
        disconnect(transitionUI, SIGNAL(viewVariables(int,QString,QStringList)), this, SIGNAL(viewVariables(int,QString,QStringList)));
        disconnect(transitionUI, SIGNAL(viewProperties(int,QString,QStringList)), this, SIGNAL(viewProperties(int,QString,QStringList)));
        disconnect(transitionUI, SIGNAL(returnToParent()), this, SLOT(returnToWidget()));
        disconnect(transitionUI, SIGNAL(transitionNameChanged(QString,int)), this, SLOT(transitionRenamed(QString,int)));
    }
}

void StateUI::variableTableElementSelected(int index, const QString &name){
    if (bsData){
        switch (currentIndex()){
        case TRANSITION_WIDGET:
            transitionUI->variableTableElementSelected(index, name); break;
        default:
            LogFile::writeToLog("StateUI::variableTableElementSelected(): Event relayed to wrong widget!!");
        }
    }else{
        LogFile::writeToLog("StateUI::variableTableElementSelected(): The data is nullptr!!");
    }
}

void StateUI::loadData(HkxObject *data, int stateindex){
    toggleSignals(false);
    setCurrentIndex(MAIN_WIDGET);
    if (data && data->getSignature() == HKB_STATE_MACHINE_STATE_INFO){
        bsData = static_cast<hkbStateMachineStateInfo *>(data);
        stateIndex = stateindex;
        name->setText(bsData->getName());
        stateId->setValue(bsData->getStateId());
        probability->setValue(bsData->getProbability());
        enable->setChecked(bsData->getEnable());
        table->item(GENERATOR_ROW, VALUE_COLUMN)->setText(bsData->getGeneratorName());
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("StateUI::loadData(): The data is nullptr or an incorrect type!!");
    }
    toggleSignals(true);
}

void StateUI::loadDynamicTableRows(){
    if (bsData){
        auto events = static_cast<hkbStateMachineEventPropertyArray *>(bsData->enterNotifyEvents.data());
        auto temp = -1;
        (events) ? temp = ADD_ENTER_EVENT_ROW + events->events.size() + 1 - exitEventsButtonRow : temp = INITIAL_EXIT_EVENT_ROW - exitEventsButtonRow;
        if (temp > 0){
            for (auto i = 0; i < temp; i++){
                table->insertRow(exitEventsButtonRow);
                exitEventsButtonRow++;
                transitionsButtonRow++;
            }
        }else if (temp < 0){
            for (auto i = temp; i < 0; i++){
                table->removeRow(exitEventsButtonRow - 1);
                exitEventsButtonRow--;
                transitionsButtonRow--;
            }
        }
        events = static_cast<hkbStateMachineEventPropertyArray *>(bsData->exitNotifyEvents.data());
        (events) ? temp = exitEventsButtonRow + events->events.size() + 1 - transitionsButtonRow : temp = exitEventsButtonRow + 1 - transitionsButtonRow;
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
        auto setupeventrows = [&](int start, int end){
            if (events){
                for (auto i = start, j = 0; i < end, j < events->events.size(); i++, j++){
                    auto eventName = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(events->events.at(j).id);
                    if (eventName != ""){
                        UIHelper::setRowItems(i, eventName, events->getClassname(), "Remove", "Edit", "Double click to remove this event", "Double click to edit this event", table);
                    }else{
                        LogFile::writeToLog("StateUI::loadDynamicTableRows(): Invalid event name!!!");
                    }
                }
            }
        };
        events = static_cast<hkbStateMachineEventPropertyArray *>(bsData->enterNotifyEvents.data());
        setupeventrows(INITIAL_EXIT_EVENT_ROW, exitEventsButtonRow);
        events = static_cast<hkbStateMachineEventPropertyArray *>(bsData->exitNotifyEvents.data());
        setupeventrows(exitEventsButtonRow + 1, transitionsButtonRow);
        auto trans = static_cast<hkbStateMachineTransitionInfoArray *>(bsData->transitions.data());
        if (trans && trans->getSignature() == HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY){
            table->setRowCount(transitionsButtonRow + trans->getNumTransitions() + 1);
            for (auto i = transitionsButtonRow + 1, j = 0; i < table->rowCount(), j < trans->getNumTransitions(); i++, j++){
                UIHelper::setRowItems(i, trans->getTransitionNameAt(j), trans->getClassname(), "Remove", "Edit", "Double click to remove this transition", "Double click to edit this transition", table);
            }
        }else{
            table->setRowCount(transitionsButtonRow + 1);
        }
    }else{
        LogFile::writeToLog("StateUI::loadDynamicTableRows(): The data is nullptr!!");
    }
}

void StateUI::eventTableElementSelected(int index, const QString &name){
    index--;
    switch (currentIndex()){
    case EVENT_PAYLOAD_WIDGET:
        eventUI->setEvent(index, name); break;
    case TRANSITION_WIDGET:
        transitionUI->eventTableElementSelected(index, name); break;
    default:
        WARNING_MESSAGE("StateUI::eventTableElementSelected(): An unwanted element selected event was recieved!!");
    }
    loadDynamicTableRows();
}

void StateUI::generatorTableElementSelected(int index, const QString &name){
    switch (currentIndex()){
    case MAIN_WIDGET:
        setGenerator(index, name); break;
    default:
        WARNING_MESSAGE("StateUI::generatorTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void StateUI::connectToTables(GenericTableWidget *generators, GenericTableWidget *events){
    if (generators && events){
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(generators, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(eventTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(generators, SIGNAL(elementSelected(int,QString)), this, SLOT(generatorTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewGenerators(int,QString,QStringList)), generators, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("StateUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void StateUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit stateNameChanged(name->text(), stateIndex);
        emit generatorNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("StateUI::setName(): The data is nullptr!!");
    }
}

void StateUI::setStateId(int id){
    if (bsData){
        if (bsData->setStateId(id)){
            emit stateIdChanged(stateIndex, bsData->stateId, bsData->getName());
        }else{
            WARNING_MESSAGE("StateUI::setStateId(): Another state has the selected state ID!!! The state ID for this state was not changed!!!");
            disconnect(stateId, SIGNAL(valueChanged(int)), this, SLOT(setStateId(int)));
            stateId->setValue(bsData->stateId);
            connect(stateId, SIGNAL(valueChanged(int)), this, SLOT(setStateId(int)), Qt::UniqueConnection);
        }
    }else{
        LogFile::writeToLog("StateUI::setStateId(): The data is nullptr!!");
    }
}

void StateUI::setProbability(){
    (bsData) ? bsData->setProbability(probability->value()) : LogFile::writeToLog("StateUI::setProbability(): The data is nullptr!!");
}

void StateUI::setEnable(){
    (bsData) ? bsData->setProbability(enable->isChecked()) : LogFile::writeToLog("StateUI::setEnable(): The data is nullptr!!");
}

void StateUI::addEvent(HkxSharedPtr & eventarray){  //Make sure bsData is not nullptr before using this function!!
    auto events = static_cast<hkbStateMachineEventPropertyArray *>(eventarray.data());
    if (!events){
        events = new hkbStateMachineEventPropertyArray(bsData->getParentFile(), -1);
        eventarray = HkxSharedPtr(events);
    }
    events->addEvent();
    loadDynamicTableRows();
}

void StateUI::removeEvent(HkxSharedPtr & eventarray, int index){  //Make sure bsData is not nullptr before using this function!!
    auto events = static_cast<hkbStateMachineEventPropertyArray *>(eventarray.data());
    if (events){
        if (index < events->getNumOfEvents() && index >= 0){
            events->removeEvent(index);
            (!events->getNumOfEvents()) ? eventarray = HkxSharedPtr() : NULL;
            loadDynamicTableRows();
        }else{
            WARNING_MESSAGE("StateUI::removeEvent(): Invalid row index selected!!");
        }
    }else{
        LogFile::writeToLog("StateUI::removeEvent(): Event data is nullptr!!");
    }
}

void StateUI::addEnterEvent(){
    (bsData) ? addEvent(bsData->enterNotifyEvents) : LogFile::writeToLog("StateUI::addEnterEvent(): The data is nullptr!!");
}

void StateUI::removeEnterEvent(int index){
    (bsData) ? removeEvent(bsData->enterNotifyEvents, index) : LogFile::writeToLog("StateUI::removeEnterEvent(): The data is nullptr!!");
}

void StateUI::addExitEvent(){
    (bsData) ? addEvent(bsData->exitNotifyEvents) : LogFile::writeToLog("StateUI::addExitEvent(): The data is nullptr!!");
}

void StateUI::removeExitEvent(int index){
    (bsData) ? removeEvent(bsData->exitNotifyEvents, index) : LogFile::writeToLog("StateUI::removeExitEvent(): The data is nullptr!!");
}

void StateUI::addTransition(){
    if (bsData){
        auto trans = static_cast<hkbStateMachineTransitionInfoArray *>(bsData->transitions.data());
        if (!trans){
            trans = new hkbStateMachineTransitionInfoArray(bsData->getParentFile(), bsData->getParentStateMachine(), -1);
            bsData->transitions = HkxSharedPtr(trans);
        }
        trans->addTransition();
        loadDynamicTableRows();
    }else{
        LogFile::writeToLog("StateUI::addTransition(): The data is nullptr!!");
    }
}

void StateUI::removeTransition(int index){
    if (bsData){
        auto trans = static_cast<hkbStateMachineTransitionInfoArray *>(bsData->transitions.data());
        if (trans){
            if (index < trans->getNumTransitions() && index >= 0){
                trans->removeTransition(index);
                (!trans->getNumTransitions()) ? bsData->transitions = HkxSharedPtr() : NULL;
                loadDynamicTableRows();
            }else{
                WARNING_MESSAGE("StateUI::removeTransition(): Invalid row index selected!!");
            }
        }else{
            WARNING_MESSAGE("StateUI::removeTransition(): Transition data is nullptr!!");
        }
    }else{
        LogFile::writeToLog("StateUI::removeTransition(): The data is nullptr!!");
    }
}

void StateUI::viewSelectedChild(int row, int column){
    auto result = 0;
    auto count = 0;
    if (bsData){
        auto viewevent = [&](hkbStateMachineEventPropertyArray *eventarray, int eventrow){
            result = row - eventrow - 1;
            if (eventarray->events.size() > result && result >= 0){
                if (column == VALUE_COLUMN){
                    eventUI->loadData(static_cast<BehaviorFile *>(bsData->getParentFile()), &eventarray->events[result]);
                    setCurrentIndex(EVENT_PAYLOAD_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the event \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeEnterEvent(result);
                    }
                }
            }else{
                LogFile::writeToLog("StateUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        };
        if (row == GENERATOR_ROW && column == VALUE_COLUMN){
            QStringList list = {hkbStateMachineStateInfo::getClassname(), hkbBlenderGeneratorChild::getClassname(), BSBoneSwitchGeneratorBoneData::getClassname()};
            emit viewGenerators(bsData->getIndexOfGenerator(bsData->generator) + 1, QString(), list);
        }else if (row == ADD_ENTER_EVENT_ROW && column == NAME_COLUMN){
            addEnterEvent();
        }else if (row == exitEventsButtonRow && column == NAME_COLUMN){
            addExitEvent();
        }else if (row == transitionsButtonRow && column == NAME_COLUMN){
            addTransition();
        }else if (row > ADD_ENTER_EVENT_ROW && row < exitEventsButtonRow){
            auto enterEvents = static_cast<hkbStateMachineEventPropertyArray *>(bsData->enterNotifyEvents.data());
            viewevent(enterEvents, ADD_ENTER_EVENT_ROW);
        }else if (row > exitEventsButtonRow && row < transitionsButtonRow){
            auto exitEvents = static_cast<hkbStateMachineEventPropertyArray *>(bsData->exitNotifyEvents.data());
            viewevent(exitEvents, exitEventsButtonRow);
        }else if (row > exitEventsButtonRow && row < table->rowCount()){
            auto enterEvents = static_cast<hkbStateMachineEventPropertyArray *>(bsData->enterNotifyEvents.data());
            (enterEvents) ? count = enterEvents->events.size() : NULL;
            auto exitEvents = static_cast<hkbStateMachineEventPropertyArray *>(bsData->exitNotifyEvents.data());
            (exitEvents) ? count = count + exitEvents->events.size() : NULL;
            result = row - BASE_NUMBER_OF_ROWS - count;
            auto trans = static_cast<hkbStateMachineTransitionInfoArray *>(bsData->transitions.data());
            if (trans && result < trans->getNumTransitions() && result >= 0){
                if (column == VALUE_COLUMN){
                    transitionUI->loadData(static_cast<BehaviorFile *>(bsData->getParentFile()), bsData->getParentStateMachine(), trans->getTransitionAt(result), result);
                    setCurrentIndex(TRANSITION_WIDGET);
                }else if (column == BINDING_COLUMN){
                    if (MainWindow::yesNoDialogue("Are you sure you want to remove the transition \""+table->item(row, NAME_COLUMN)->text()+"\"?") == QMessageBox::Yes){
                        removeTransition(result);
                    }
                }
            }else{
                LogFile::writeToLog("StateUI::viewSelectedChild(): Invalid index of child to view!!");
            }
        }
    }else{
        LogFile::writeToLog("StateUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void StateUI::setGenerator(int index, const QString & name){
    UIHelper::setGenerator(index, name, bsData, static_cast<hkbGenerator *>(bsData->generator.data()), NULL_SIGNATURE, HkxObject::TYPE_GENERATOR, table, behaviorView, GENERATOR_ROW, VALUE_COLUMN);
}

void StateUI::setBehaviorView(BehaviorGraphView *view){
    behaviorView = view;
}

void StateUI::transitionRenamed(const QString &name, int index){
    if (bsData){
        auto item = table->item(transitionsButtonRow + index + 1, NAME_COLUMN);
        (item) ? item->setText(name) : LogFile::writeToLog("StateUI::transitionRenamed(): Invalid row selected!!");
    }else{
        LogFile::writeToLog("StateUI::transitionRenamed(): The data is nullptr!!");
    }
}

void StateUI::eventRenamed(const QString & name, int index){
    if (bsData){
        if (currentIndex() == EVENT_PAYLOAD_WIDGET){
            eventUI->eventRenamed(name, index);
        }else if (currentIndex() == TRANSITION_WIDGET){
            transitionUI->eventRenamed(name, index);
        }
        loadDynamicTableRows(); //Inefficient...
    }else{
        LogFile::writeToLog("StateUI::eventRenamed(): The data is nullptr!!");
    }
}

void StateUI::generatorRenamed(const QString & name, int index){
    if (bsData){
        index--;
        if (index == static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData->generator)){
            table->item(GENERATOR_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("StateUI::generatorRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void StateUI::setReturnPushButonVisability(bool visible){
    returnPB->setVisible(visible);
}

void StateUI::returnToWidget(){
    setCurrentIndex(MAIN_WIDGET);
}
