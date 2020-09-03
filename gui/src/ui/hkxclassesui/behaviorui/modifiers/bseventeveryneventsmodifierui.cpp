#include "bseventeveryneventsmodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/modifiers/BSEventEveryNEventsModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 9

#define NAME_ROW 0
#define ENABLE_ROW 1
#define EVENT_TO_CHECK_FOR_ID_ROW 2
#define EVENT_TO_CHECK_FOR_PAYLOAD_ROW 3
#define EVENT_TO_SEND_ID_ROW 4
#define EVENT_TO_SEND_PAYLOAD_ROW 5
#define NUMBER_OF_EVENTS_BEFORE_SEND_ROW 6
#define MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW 7
#define RANDOMIZE_NUMBER_OF_EVENTS_ROW 8

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSEventEveryNEventsModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSEventEveryNEventsModifierUI::BSEventEveryNEventsModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      eventToCheckForPayload(new QLineEdit),
      eventToSendPayload(new QLineEdit),
      numberOfEventsBeforeSend(new SpinBox),
      minimumNumberOfEventsBeforeSend(new SpinBox),
      randomizeNumberOfEvents(new CheckBox)
{
    setTitle("BSEventEveryNEventsModifier");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(ENABLE_ROW, NAME_COLUMN, new TableWidgetItem("enable"));
    table->setItem(ENABLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_ROW, VALUE_COLUMN, enable);
    table->setItem(EVENT_TO_CHECK_FOR_ID_ROW, NAME_COLUMN, new TableWidgetItem("eventToCheckForId"));
    table->setItem(EVENT_TO_CHECK_FOR_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(EVENT_TO_CHECK_FOR_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(EVENT_TO_CHECK_FOR_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(EVENT_TO_CHECK_FOR_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("eventToCheckForPayload"));
    table->setItem(EVENT_TO_CHECK_FOR_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(EVENT_TO_CHECK_FOR_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(EVENT_TO_CHECK_FOR_PAYLOAD_ROW, VALUE_COLUMN, eventToCheckForPayload);
    table->setItem(EVENT_TO_SEND_ID_ROW, NAME_COLUMN, new TableWidgetItem("eventToSendId"));
    table->setItem(EVENT_TO_SEND_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(EVENT_TO_SEND_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("eventToSendPayload"));
    table->setItem(EVENT_TO_SEND_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(EVENT_TO_SEND_PAYLOAD_ROW, VALUE_COLUMN, eventToSendPayload);
    table->setItem(NUMBER_OF_EVENTS_BEFORE_SEND_ROW, NAME_COLUMN, new TableWidgetItem("numberOfEventsBeforeSend"));
    table->setItem(NUMBER_OF_EVENTS_BEFORE_SEND_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(NUMBER_OF_EVENTS_BEFORE_SEND_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(NUMBER_OF_EVENTS_BEFORE_SEND_ROW, VALUE_COLUMN, numberOfEventsBeforeSend);
    table->setItem(MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW, NAME_COLUMN, new TableWidgetItem("minimumNumberOfEventsBeforeSend"));
    table->setItem(MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW, VALUE_COLUMN, minimumNumberOfEventsBeforeSend);
    table->setItem(RANDOMIZE_NUMBER_OF_EVENTS_ROW, NAME_COLUMN, new TableWidgetItem("randomizeNumberOfEvents"));
    table->setItem(RANDOMIZE_NUMBER_OF_EVENTS_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(RANDOMIZE_NUMBER_OF_EVENTS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RANDOMIZE_NUMBER_OF_EVENTS_ROW, VALUE_COLUMN, randomizeNumberOfEvents);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSEventEveryNEventsModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(eventToCheckForPayload, SIGNAL(editingFinished()), this, SLOT(setEventToCheckForPayload()), Qt::UniqueConnection);
        connect(eventToSendPayload, SIGNAL(editingFinished()), this, SLOT(setEventToSendPayload()), Qt::UniqueConnection);
        connect(numberOfEventsBeforeSend, SIGNAL(editingFinished()), this, SLOT(setNumberOfEventsBeforeSend()), Qt::UniqueConnection);
        connect(minimumNumberOfEventsBeforeSend, SIGNAL(editingFinished()), this, SLOT(setMinimumNumberOfEventsBeforeSend()), Qt::UniqueConnection);
        connect(randomizeNumberOfEvents, SIGNAL(released()), this, SLOT(setRandomizeNumberOfEvents()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(eventToCheckForPayload, SIGNAL(editingFinished()), this, SLOT(setEventToCheckForPayload()));
        disconnect(eventToSendPayload, SIGNAL(editingFinished()), this, SLOT(setEventToSendPayload()));
        disconnect(numberOfEventsBeforeSend, SIGNAL(editingFinished()), this, SLOT(setNumberOfEventsBeforeSend()));
        disconnect(minimumNumberOfEventsBeforeSend, SIGNAL(editingFinished()), this, SLOT(setMinimumNumberOfEventsBeforeSend()));
        disconnect(randomizeNumberOfEvents, SIGNAL(released()), this, SLOT(setRandomizeNumberOfEvents()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSEventEveryNEventsModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && properties && events){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(eventTableElementSelected(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSEventEveryNEventsModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_EVENT_EVERY_N_EVENTS_MODIFIER){
            bsData = static_cast<BSEventEveryNEventsModifier *>(data);
            auto payload = static_cast<hkbStringEventPayload *>(bsData->getEventToCheckForPayload());
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            auto text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getEventToCheckForID());
            (text != "") ? table->item(EVENT_TO_CHECK_FOR_ID_ROW, VALUE_COLUMN)->setText(text) : table->item(EVENT_TO_CHECK_FOR_ID_ROW, VALUE_COLUMN)->setText("None");
            (payload) ? eventToCheckForPayload->setText(payload->getData()) : eventToCheckForPayload->setText("");
            payload = static_cast<hkbStringEventPayload *>(bsData->getEventToSendPayload());
            text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getEventToSendID());
            (text != "") ? table->item(EVENT_TO_SEND_ID_ROW, VALUE_COLUMN)->setText(text) : table->item(EVENT_TO_SEND_ID_ROW, VALUE_COLUMN)->setText("None");
            (payload) ? eventToSendPayload->setText(payload->getData()) : eventToSendPayload->setText("");
            numberOfEventsBeforeSend->setValue(bsData->getNumberOfEventsBeforeSend());
            minimumNumberOfEventsBeforeSend->setValue(bsData->getMinimumNumberOfEventsBeforeSend());
            randomizeNumberOfEvents->setChecked(bsData->getRandomizeNumberOfEvents());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(NUMBER_OF_EVENTS_BEFORE_SEND_ROW, BINDING_COLUMN, varBind, "numberOfEventsBeforeSend", table, bsData);
            UIHelper::loadBinding(MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW, BINDING_COLUMN, varBind, "minimumNumberOfEventsBeforeSend", table, bsData);
            UIHelper::loadBinding(RANDOMIZE_NUMBER_OF_EVENTS_ROW, BINDING_COLUMN, varBind, "randomizeNumberOfEvents", table, bsData);
        }else{
            LogFile::writeToLog("BSEventEveryNEventsModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSEventEveryNEventsModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::setName(): The data is nullptr!!");
    }
}

void BSEventEveryNEventsModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSEventEveryNEventsModifierUI::setEnable(): The data is nullptr!!");
}

void BSEventEveryNEventsModifierUI::setEventToCheckForId(int index, const QString & name){
    if (bsData){
        bsData->setEventToCheckForID(index - 1);
        table->item(EVENT_TO_CHECK_FOR_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::seteventToCheckForId(): The data is nullptr!!");
    }
}

void BSEventEveryNEventsModifierUI::setEventToCheckForPayload(){
    if (bsData){
        auto payload = bsData->getEventToCheckForPayload();
        if (eventToCheckForPayload->text() != ""){
            if (payload){
                payload->setData(eventToCheckForPayload->text());
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), eventToCheckForPayload->text());
                bsData->setEventToCheckForPayload(payload);
            }
        }else{
            bsData->setEventToCheckForPayload(nullptr);
        }
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::setEventToCheckForPayload(): The data is nullptr!!");
    }
}

void BSEventEveryNEventsModifierUI::setEventToSendId(int index, const QString &name){
    if (bsData){
        bsData->setEventToSendID(index - 1);
        table->item(EVENT_TO_SEND_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::setEventToSendId(): The data is nullptr!!");
    }
}

void BSEventEveryNEventsModifierUI::setEventToSendPayload(){
    if (bsData){
        auto payload = bsData->getEventToSendPayload();
        if (eventToSendPayload->text() != ""){
            if (payload){
                payload->setData(eventToSendPayload->text());
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), eventToSendPayload->text());
                bsData->setEventToSendPayload(payload);
            }
        }else{
            bsData->setEventToSendPayload(nullptr);
        }
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::setEventToSendPayload(): The data is nullptr!!");
    }
}

void BSEventEveryNEventsModifierUI::setNumberOfEventsBeforeSend(){
    (bsData) ? bsData->setNumberOfEventsBeforeSend(numberOfEventsBeforeSend->value()) : LogFile::writeToLog("BSEventEveryNEventsModifierUI::setNumberOfEventsBeforeSend(): The data is nullptr!!");
}

void BSEventEveryNEventsModifierUI::setMinimumNumberOfEventsBeforeSend(){
    (bsData) ? bsData->setMinimumNumberOfEventsBeforeSend(minimumNumberOfEventsBeforeSend->value()) : LogFile::writeToLog("BSEventEveryNEventsModifierUI::setMinimumNumberOfEventsBeforeSend(): The data is nullptr!!");
}

void BSEventEveryNEventsModifierUI::setRandomizeNumberOfEvents(){
    (bsData) ? bsData->setRandomizeNumberOfEvents(randomizeNumberOfEvents->isChecked()) : LogFile::writeToLog("BSEventEveryNEventsModifierUI::setRandomizeNumberOfEvents(): The data is nullptr!!");
}

void BSEventEveryNEventsModifierUI::eventTableElementSelected(int index, const QString &name){
    index--;
    switch (table->currentRow()){
    case EVENT_TO_CHECK_FOR_ID_ROW:
        setEventToCheckForId(index, name); break;
    case EVENT_TO_SEND_ID_ROW:
        setEventToSendId(index, name); break;
    default:
        WARNING_MESSAGE("BSEventEveryNEventsModifierUI::eventTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void BSEventEveryNEventsModifierUI::viewSelected(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case ENABLE_ROW:
                checkisproperty(ENABLE_ROW, "enable"); break;
            case NUMBER_OF_EVENTS_BEFORE_SEND_ROW:
                checkisproperty(NUMBER_OF_EVENTS_BEFORE_SEND_ROW, "numberOfEventsBeforeSend"); break;
            case MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW:
                checkisproperty(MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW, "minimumNumberOfEventsBeforeSend"); break;
            case RANDOMIZE_NUMBER_OF_EVENTS_ROW:
                checkisproperty(RANDOMIZE_NUMBER_OF_EVENTS_ROW, "randomizeNumberOfEvents"); break;
            }
        }else if (column == VALUE_COLUMN && row == EVENT_TO_CHECK_FOR_ID_ROW){
            emit viewEvents(bsData->getEventToCheckForID() + 1, QString(), QStringList());
        }else if (column == VALUE_COLUMN && row == EVENT_TO_SEND_ID_ROW){
            emit viewEvents(bsData->getEventToSendID() + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSEventEveryNEventsModifierUI::selectTableToView(bool viewisProperty, const QString & path){
    if (bsData){
        if (viewisProperty){
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
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSEventEveryNEventsModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        index--;
        (index == bsData->getEventToCheckForID()) ? table->item(EVENT_TO_CHECK_FOR_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
        (index == bsData->getEventToSendID()) ? table->item(EVENT_TO_SEND_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void BSEventEveryNEventsModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("numberOfEventsBeforeSend", NUMBER_OF_EVENTS_BEFORE_SEND_ROW);
            setname("minimumNumberOfEventsBeforeSend", MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW);
            setname("randomizeNumberOfEvents", RANDOMIZE_NUMBER_OF_EVENTS_ROW);
        }
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSEventEveryNEventsModifierUI::setBindingVariable(int index, const QString &name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, bsData);
        };
        switch (row){
        case ENABLE_ROW:
            checkisproperty(ENABLE_ROW, "enable", VARIABLE_TYPE_BOOL); break;
        case NUMBER_OF_EVENTS_BEFORE_SEND_ROW:
            checkisproperty(NUMBER_OF_EVENTS_BEFORE_SEND_ROW, "numberOfEventsBeforeSend", VARIABLE_TYPE_INT32); break;
        case MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW:
            checkisproperty(MINIMUM_NUMBER_OF_EVENTS_BEFORE_SEND_ROW, "minimumNumberOfEventsBeforeSend", VARIABLE_TYPE_INT32); break;
        case RANDOMIZE_NUMBER_OF_EVENTS_ROW:
            checkisproperty(RANDOMIZE_NUMBER_OF_EVENTS_ROW, "randomizeNumberOfEvents", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("BSEventEveryNEventsModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
