#include "bseventonfalsetotruemodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/modifiers/BSEventOnFalseToTrueModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 14

#define NAME_ROW 0
#define ENABLE_ROW 1
#define ENABLE_EVENT_1_ROW 2
#define VARIABLE_TO_TEST_1_ROW 3
#define EVENT_TO_SEND_1_ID_ROW 4
#define EVENT_TO_SEND_1_PAYLOAD_ROW 5
#define ENABLE_EVENT_2_ROW 6
#define VARIABLE_TO_TEST_2_ROW 7
#define EVENT_TO_SEND_2_ID_ROW 8
#define EVENT_TO_SEND_2_PAYLOAD_ROW 9
#define ENABLE_EVENT_3_ROW 10
#define VARIABLE_TO_TEST_3_ROW 11
#define EVENT_TO_SEND_3_ID_ROW 12
#define EVENT_TO_SEND_3_PAYLOAD_ROW 13

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSEventOnFalseToTrueModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSEventOnFalseToTrueModifierUI::BSEventOnFalseToTrueModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      bEnableEvent1(new CheckBox),
      bVariableToTest1(new CheckBox),
      eventToSend1Payload(new QLineEdit),
      bEnableEvent2(new CheckBox),
      bVariableToTest2(new CheckBox),
      eventToSend2Payload(new QLineEdit),
      bEnableEvent3(new CheckBox),
      bVariableToTest3(new CheckBox),
      eventToSend3Payload(new QLineEdit)
{
    setTitle("BSEventOnFalseToTrueModifier");
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
    table->setItem(ENABLE_EVENT_1_ROW, NAME_COLUMN, new TableWidgetItem("bEnableEvent1"));
    table->setItem(ENABLE_EVENT_1_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_EVENT_1_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_EVENT_1_ROW, VALUE_COLUMN, bEnableEvent1);
    table->setItem(VARIABLE_TO_TEST_1_ROW, NAME_COLUMN, new TableWidgetItem("bVariableToTest1"));
    table->setItem(VARIABLE_TO_TEST_1_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(VARIABLE_TO_TEST_1_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VARIABLE_TO_TEST_1_ROW, VALUE_COLUMN, bVariableToTest1);
    table->setItem(EVENT_TO_SEND_1_ID_ROW, NAME_COLUMN, new TableWidgetItem("eventToSend1Id"));
    table->setItem(EVENT_TO_SEND_1_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_1_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_1_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(EVENT_TO_SEND_1_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("eventToSend1Payload"));
    table->setItem(EVENT_TO_SEND_1_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_1_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(EVENT_TO_SEND_1_PAYLOAD_ROW, VALUE_COLUMN, eventToSend1Payload);
    table->setItem(ENABLE_EVENT_2_ROW, NAME_COLUMN, new TableWidgetItem("bEnableEvent2"));
    table->setItem(ENABLE_EVENT_2_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_EVENT_2_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_EVENT_2_ROW, VALUE_COLUMN, bEnableEvent2);
    table->setItem(VARIABLE_TO_TEST_2_ROW, NAME_COLUMN, new TableWidgetItem("bVariableToTest2"));
    table->setItem(VARIABLE_TO_TEST_2_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(VARIABLE_TO_TEST_2_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VARIABLE_TO_TEST_2_ROW, VALUE_COLUMN, bVariableToTest2);
    table->setItem(EVENT_TO_SEND_2_ID_ROW, NAME_COLUMN, new TableWidgetItem("eventToSend2Id"));
    table->setItem(EVENT_TO_SEND_2_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_2_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_2_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(EVENT_TO_SEND_2_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("eventToSend2Payload"));
    table->setItem(EVENT_TO_SEND_2_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_2_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(EVENT_TO_SEND_2_PAYLOAD_ROW, VALUE_COLUMN, eventToSend2Payload);
    table->setItem(ENABLE_EVENT_3_ROW, NAME_COLUMN, new TableWidgetItem("bEnableEvent3"));
    table->setItem(ENABLE_EVENT_3_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_EVENT_3_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_EVENT_3_ROW, VALUE_COLUMN, bEnableEvent3);
    table->setItem(VARIABLE_TO_TEST_3_ROW, NAME_COLUMN, new TableWidgetItem("bVariableToTest3"));
    table->setItem(VARIABLE_TO_TEST_3_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(VARIABLE_TO_TEST_3_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VARIABLE_TO_TEST_3_ROW, VALUE_COLUMN, bVariableToTest3);
    table->setItem(EVENT_TO_SEND_3_ID_ROW, NAME_COLUMN, new TableWidgetItem("eventToSend3Id"));
    table->setItem(EVENT_TO_SEND_3_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_3_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_3_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(EVENT_TO_SEND_3_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("eventToSend3Payload"));
    table->setItem(EVENT_TO_SEND_3_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(EVENT_TO_SEND_3_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(EVENT_TO_SEND_3_PAYLOAD_ROW, VALUE_COLUMN, eventToSend3Payload);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSEventOnFalseToTrueModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(bEnableEvent1, SIGNAL(released()), this, SLOT(setEnableEvent1()), Qt::UniqueConnection);
        connect(bVariableToTest1, SIGNAL(released()), this, SLOT(setVariableToTest1()), Qt::UniqueConnection);
        connect(eventToSend1Payload, SIGNAL(editingFinished()), this, SLOT(setEventToSend1Payload()), Qt::UniqueConnection);
        connect(bEnableEvent2, SIGNAL(released()), this, SLOT(setEnableEvent2()), Qt::UniqueConnection);
        connect(bVariableToTest2, SIGNAL(released()), this, SLOT(setVariableToTest2()), Qt::UniqueConnection);
        connect(eventToSend2Payload, SIGNAL(editingFinished()), this, SLOT(setEventToSend2Payload()), Qt::UniqueConnection);
        connect(bEnableEvent3, SIGNAL(released()), this, SLOT(setEnableEvent3()), Qt::UniqueConnection);
        connect(bVariableToTest3, SIGNAL(released()), this, SLOT(setVariableToTest3()), Qt::UniqueConnection);
        connect(eventToSend3Payload, SIGNAL(editingFinished()), this, SLOT(setEventToSend3Payload()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(bEnableEvent1, SIGNAL(released()), this, SLOT(setEnableEvent1()));
        disconnect(bVariableToTest1, SIGNAL(released()), this, SLOT(setVariableToTest1()));
        disconnect(eventToSend1Payload, SIGNAL(editingFinished()), this, SLOT(setEventToSend1Payload()));
        disconnect(bEnableEvent2, SIGNAL(released()), this, SLOT(setEnableEvent2()));
        disconnect(bVariableToTest2, SIGNAL(released()), this, SLOT(setVariableToTest2()));
        disconnect(eventToSend2Payload, SIGNAL(editingFinished()), this, SLOT(setEventToSend2Payload()));
        disconnect(bEnableEvent3, SIGNAL(released()), this, SLOT(setEnableEvent3()));
        disconnect(bVariableToTest3, SIGNAL(released()), this, SLOT(setVariableToTest3()));
        disconnect(eventToSend3Payload, SIGNAL(editingFinished()), this, SLOT(setEventToSend3Payload()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSEventOnFalseToTrueModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
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
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_EVENT_ON_FALSE_TO_TRUE_MODIFIER){
            bsData = static_cast<BSEventOnFalseToTrueModifier *>(data);
            auto payload1 = bsData->getEventToSend1Payload();
            auto payload2 = bsData->getEventToSend2Payload();
            auto payload3 = bsData->getEventToSend3Payload();
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            bEnableEvent1->setChecked(bsData->getBEnableEvent1());
            bVariableToTest1->setChecked(bsData->getBVariableToTest1());
            auto seteventtext = [&](const QString & eventname, int row){
                (eventname != "") ? table->item(row, VALUE_COLUMN)->setText(eventname) : table->item(row, VALUE_COLUMN)->setText("None");
            };
            auto setpayloadtext = [&](hkbStringEventPayload *payload, QLineEdit *lineedit){
                (payload) ? lineedit->setText(payload->getData()) : lineedit->setText("");
            };
            auto text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getEventToSend1ID());
            seteventtext(text, EVENT_TO_SEND_1_ID_ROW);
            setpayloadtext(payload1, eventToSend1Payload);
            bEnableEvent2->setChecked(bsData->getBEnableEvent2());
            bVariableToTest2->setChecked(bsData->getBVariableToTest2());
            text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getEventToSend2ID());
            seteventtext(text, EVENT_TO_SEND_2_ID_ROW);
            setpayloadtext(payload2, eventToSend2Payload);
            bEnableEvent3->setChecked(bsData->getBEnableEvent3());
            bVariableToTest3->setChecked(bsData->getBVariableToTest3());
            text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getEventToSend3ID());
            seteventtext(text, EVENT_TO_SEND_3_ID_ROW);
            setpayloadtext(payload3, eventToSend3Payload);
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(ENABLE_EVENT_1_ROW, BINDING_COLUMN, varBind, "bEnableEvent1", table, bsData);
            UIHelper::loadBinding(VARIABLE_TO_TEST_1_ROW, BINDING_COLUMN, varBind, "bVariableToTest1", table, bsData);
            UIHelper::loadBinding(ENABLE_EVENT_2_ROW, BINDING_COLUMN, varBind, "bEnableEvent2", table, bsData);
            UIHelper::loadBinding(VARIABLE_TO_TEST_2_ROW, BINDING_COLUMN, varBind, "bVariableToTest2", table, bsData);
            UIHelper::loadBinding(ENABLE_EVENT_3_ROW, BINDING_COLUMN, varBind, "bEnableEvent3", table, bsData);
            UIHelper::loadBinding(VARIABLE_TO_TEST_3_ROW, BINDING_COLUMN, varBind, "bVariableToTest3", table, bsData);
        }else{
            LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSEventOnFalseToTrueModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::setName(): The data is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::setEnable(): The data is nullptr!!");
}

void BSEventOnFalseToTrueModifierUI::setEnableEvent1(){
    (bsData) ? bsData->setBEnableEvent1(bEnableEvent1->isChecked()) : LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::setEnableEvent1(): The data is nullptr!!");
}

void BSEventOnFalseToTrueModifierUI::setVariableToTest1(){
    (bsData) ? bsData->setBVariableToTest1(bVariableToTest1->isChecked()) : LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::setBVariableToTest1(): The data is nullptr!!");
}

void BSEventOnFalseToTrueModifierUI::setEventToSend1Id(int index, const QString & name){
    if (bsData){
        bsData->setEventToSend1ID(index - 1);
        table->item(EVENT_TO_SEND_1_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::seteventToSend1Id(): The data is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::setEventToSend1Payload(){
    if (bsData){
        auto payload = bsData->getEventToSend1Payload();
        if (eventToSend1Payload->text() != ""){
            if (payload){
                payload->setData(eventToSend1Payload->text());
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), eventToSend1Payload->text());
                bsData->setEventToSend1Payload(payload);
            }
        }else{
            bsData->setEventToSend1Payload(nullptr);
        }
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::seteventToSend1Payload(): The data is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::setEnableEvent2(){
    (bsData) ? bsData->setBEnableEvent2(bEnableEvent2->isChecked()) : LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::setEnableEvent2(): The data is nullptr!!");
}

void BSEventOnFalseToTrueModifierUI::setVariableToTest2(){
    (bsData) ? bsData->setBVariableToTest2(bVariableToTest2->isChecked()) : LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::setBVariableToTest2(): The data is nullptr!!");
}

void BSEventOnFalseToTrueModifierUI::setEventToSend2Id(int index, const QString & name){
    if (bsData){
        bsData->setEventToSend2ID(index - 1);
        table->item(EVENT_TO_SEND_2_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::seteventToSend2Id(): The data is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::setEventToSend2Payload(){
    if (bsData){
        auto payload = bsData->getEventToSend2Payload();
        if (eventToSend2Payload->text() != ""){
            if (payload){
                payload->setData(eventToSend2Payload->text());
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), eventToSend2Payload->text());
                bsData->setEventToSend2Payload(payload);
            }
        }else{
            bsData->setEventToSend2Payload(nullptr);
        }
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::seteventToSend2Payload(): The data is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::setEnableEvent3(){
    (bsData) ? bsData->setBEnableEvent3(bEnableEvent3->isChecked()) : LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::setEnableEvent3(): The data is nullptr!!");
}

void BSEventOnFalseToTrueModifierUI::setVariableToTest3(){
    (bsData) ? bsData->setBVariableToTest3(bVariableToTest3->isChecked()) : LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::setBVariableToTest3(): The data is nullptr!!");
}

void BSEventOnFalseToTrueModifierUI::setEventToSend3Id(int index, const QString & name){
    if (bsData){
        bsData->setEventToSend3ID(index - 1);
        table->item(EVENT_TO_SEND_3_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::seteventToSend3Id(): The data is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::setEventToSend3Payload(){
    if (bsData){
        auto payload = bsData->getEventToSend3Payload();
        if (eventToSend3Payload->text() != ""){
            if (payload){
                payload->setData(eventToSend3Payload->text());
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), eventToSend3Payload->text());
                bsData->setEventToSend3Payload(payload);
            }
        }else{
            bsData->setEventToSend3Payload(nullptr);
        }
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::seteventToSend3Payload(): The data is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::eventTableElementSelected(int index, const QString &name){
    index--;
    switch (table->currentRow()){
    case EVENT_TO_SEND_1_ID_ROW:
        setEventToSend1Id(index, name); break;
    case EVENT_TO_SEND_2_ID_ROW:
        setEventToSend2Id(index, name); break;
    case EVENT_TO_SEND_3_ID_ROW:
        setEventToSend3Id(index, name); break;
    default:
        WARNING_MESSAGE("BSEventOnFalseToTrueModifierUI::eventTableElementSelected(): An unwanted element selected event was recieved!!");
    }
}

void BSEventOnFalseToTrueModifierUI::viewSelected(int row, int column){
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
            case ENABLE_EVENT_1_ROW:
                checkisproperty(ENABLE_EVENT_1_ROW, "bEnableEvent1"); break;
            case VARIABLE_TO_TEST_1_ROW:
                checkisproperty(VARIABLE_TO_TEST_1_ROW, "bVariableToTest1"); break;
            case ENABLE_EVENT_2_ROW:
                checkisproperty(ENABLE_EVENT_2_ROW, "bEnableEvent2"); break;
            case VARIABLE_TO_TEST_2_ROW:
                checkisproperty(VARIABLE_TO_TEST_2_ROW, "bVariableToTest2"); break;
            case ENABLE_EVENT_3_ROW:
                checkisproperty(ENABLE_EVENT_3_ROW, "bEnableEvent3"); break;
            case VARIABLE_TO_TEST_3_ROW:
                checkisproperty(VARIABLE_TO_TEST_3_ROW, "bVariableToTest3"); break;
            }
        }else if (column == VALUE_COLUMN && row == EVENT_TO_SEND_1_ID_ROW){
            emit viewEvents(bsData->getEventToSend1ID() + 1, QString(), QStringList());
        }else if (column == VALUE_COLUMN && row == EVENT_TO_SEND_2_ID_ROW){
            emit viewEvents(bsData->getEventToSend2ID() + 1, QString(), QStringList());
        }else if (column == VALUE_COLUMN && row == EVENT_TO_SEND_3_ID_ROW){
            emit viewEvents(bsData->getEventToSend3ID() + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto rename = [&](int row, int id){
            (index == id) ? table->item(row, VALUE_COLUMN)->setText(name) : NULL;
        };
        rename(bsData->getEventToSend1ID(), EVENT_TO_SEND_1_ID_ROW);
        rename(bsData->getEventToSend2ID(), EVENT_TO_SEND_2_ID_ROW);
        rename(bsData->getEventToSend3ID(), EVENT_TO_SEND_3_ID_ROW);
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("bEnableEvent1", ENABLE_EVENT_1_ROW);
            setname("bVariableToTest1", VARIABLE_TO_TEST_1_ROW);
            setname("bEnableEvent2", ENABLE_EVENT_2_ROW);
            setname("bVariableToTest2", VARIABLE_TO_TEST_2_ROW);
            setname("bEnableEvent3", ENABLE_EVENT_3_ROW);
            setname("bVariableToTest3", VARIABLE_TO_TEST_3_ROW);
        }
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSEventOnFalseToTrueModifierUI::setBindingVariable(int index, const QString &name){
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
        case ENABLE_EVENT_1_ROW:
            checkisproperty(ENABLE_EVENT_1_ROW, "bEnableEvent1", VARIABLE_TYPE_BOOL); break;
        case VARIABLE_TO_TEST_1_ROW:
            checkisproperty(VARIABLE_TO_TEST_1_ROW, "bVariableToTest1", VARIABLE_TYPE_BOOL); break;
        case ENABLE_EVENT_2_ROW:
            checkisproperty(ENABLE_EVENT_2_ROW, "bEnableEvent2", VARIABLE_TYPE_BOOL); break;
        case VARIABLE_TO_TEST_2_ROW:
            checkisproperty(VARIABLE_TO_TEST_2_ROW, "bVariableToTest2", VARIABLE_TYPE_BOOL); break;
        case ENABLE_EVENT_3_ROW:
            checkisproperty(ENABLE_EVENT_3_ROW, "bEnableEvent3", VARIABLE_TYPE_BOOL); break;
        case VARIABLE_TO_TEST_3_ROW:
            checkisproperty(VARIABLE_TO_TEST_3_ROW, "bVariableToTest3", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("BSEventOnFalseToTrueModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
