#include "BSTimerModifierUI.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/modifiers/BSTimerModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define NAME_ROW 0
#define ENABLE_ROW 1
#define ALARM_TIME_SECONDS_ROW 2
#define ALARM_EVENT_ID_ROW 3
#define ALARM_EVENT_PAYLOAD_ROW 4
#define RESET_ALARM_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSTimerModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSTimerModifierUI::BSTimerModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      alarmTimeSeconds(new DoubleSpinBox),
      alarmEventPayload(new QLineEdit),
      resetAlarm(new CheckBox)
{
    setTitle("BSTimerModifierUI");
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
    table->setItem(ALARM_TIME_SECONDS_ROW, NAME_COLUMN, new TableWidgetItem("alarmTimeSeconds"));
    table->setItem(ALARM_TIME_SECONDS_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ALARM_TIME_SECONDS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ALARM_TIME_SECONDS_ROW, VALUE_COLUMN, alarmTimeSeconds);
    table->setItem(ALARM_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("alarmEventId"));
    table->setItem(ALARM_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ALARM_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(ALARM_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(ALARM_EVENT_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("alarmEventPayload"));
    table->setItem(ALARM_EVENT_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(ALARM_EVENT_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(ALARM_EVENT_PAYLOAD_ROW, VALUE_COLUMN, alarmEventPayload);
    table->setItem(RESET_ALARM_ROW, NAME_COLUMN, new TableWidgetItem("resetAlarm"));
    table->setItem(RESET_ALARM_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(RESET_ALARM_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RESET_ALARM_ROW, VALUE_COLUMN, resetAlarm);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSTimerModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(alarmTimeSeconds, SIGNAL(editingFinished()), this, SLOT(setAlarmTimeSeconds()), Qt::UniqueConnection);
        connect(alarmEventPayload, SIGNAL(editingFinished()), this, SLOT(setAlarmEventPayload()), Qt::UniqueConnection);
        connect(resetAlarm, SIGNAL(released()), this, SLOT(setResetAlarm()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(alarmTimeSeconds, SIGNAL(editingFinished()), this, SLOT(setAlarmTimeSeconds()));
        disconnect(alarmEventPayload, SIGNAL(editingFinished()), this, SLOT(setAlarmEventPayload()));
        disconnect(resetAlarm, SIGNAL(released()), this, SLOT(setResetAlarm()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSTimerModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && properties && events){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(setAlarmEventId(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSTimerModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSTimerModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_TIMER_MODIFIER){
            bsData = static_cast<BSTimerModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            alarmTimeSeconds->setValue(bsData->getAlarmTimeSeconds());
            auto text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getAlarmEventID());
            (text != "") ? table->item(ALARM_EVENT_ID_ROW, VALUE_COLUMN)->setText(text) : table->item(ALARM_EVENT_ID_ROW, VALUE_COLUMN)->setText("None");
            auto payload = bsData->getAlarmEventPayload();
            (payload) ? alarmEventPayload->setText(payload->getData()) : alarmEventPayload->setText("");
            resetAlarm->setChecked(bsData->getResetAlarm());
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(ALARM_TIME_SECONDS_ROW, BINDING_COLUMN, varBind, "alarmTimeSeconds", table, bsData);
            UIHelper::loadBinding(RESET_ALARM_ROW, BINDING_COLUMN, varBind, "resetAlarm", table, bsData);
        }else{
            LogFile::writeToLog("BSTimerModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSTimerModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSTimerModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSTimerModifierUI::setName(): The data is nullptr!!");
    }
}

void BSTimerModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSTimerModifierUI::setEnable(): The data is nullptr!!");
}

void BSTimerModifierUI::setAlarmTimeSeconds(){
    (bsData) ? bsData->setAlarmTimeSeconds(alarmTimeSeconds->value()) : LogFile::writeToLog("BSTimerModifierUI::setAlarmTimeSeconds(): The data is nullptr!!");
}

void BSTimerModifierUI::setAlarmEventId(int index, const QString & name){
    if (bsData){
        bsData->setAlarmEventID(index - 1);
        table->item(ALARM_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("BSTimerModifierUI::setAlarmEventId(): The data is nullptr!!");
    }
}

void BSTimerModifierUI::setAlarmEventPayload(){
    if (bsData){
        auto payload = bsData->getAlarmEventPayload();
        if (alarmEventPayload->text() != ""){
            if (payload){
                payload->setData(alarmEventPayload->text());
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), alarmEventPayload->text());
                bsData->setAlarmEventPayload(payload);
            }
        }else{
            bsData->setAlarmEventPayload(nullptr);
        }
    }else{
        LogFile::writeToLog("BSTimerModifierUI::setalarmEventPayload(): The data is nullptr!!");
    }
}

void BSTimerModifierUI::setResetAlarm(){
    (bsData) ? bsData->setResetAlarm(resetAlarm->isChecked()) : LogFile::writeToLog("BSTimerModifierUI::setResetAlarm(): The data is nullptr!!");
}

void BSTimerModifierUI::viewSelected(int row, int column){
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
            case ALARM_TIME_SECONDS_ROW:
                checkisproperty(ALARM_TIME_SECONDS_ROW, "alarmTimeSeconds"); break;
            case RESET_ALARM_ROW:
                checkisproperty(RESET_ALARM_ROW, "resetAlarm"); break;
            }
        }else if (column == VALUE_COLUMN && row == ALARM_EVENT_ID_ROW){
            emit viewEvents(bsData->getAlarmEventID() + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("BSTimerModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSTimerModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSTimerModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSTimerModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        index--;
        (index == bsData->getAlarmEventID()) ? table->item(ALARM_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
    }else{
        LogFile::writeToLog("BSTimerModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void BSTimerModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("alarmTimeSeconds", ALARM_TIME_SECONDS_ROW);
            setname("resetAlarm", RESET_ALARM_ROW);
        }
    }else{
        LogFile::writeToLog("BSTimerModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSTimerModifierUI::setBindingVariable(int index, const QString &name){
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
        case ALARM_TIME_SECONDS_ROW:
            checkisproperty(ALARM_TIME_SECONDS_ROW, "alarmTimeSeconds", VARIABLE_TYPE_REAL); break;
        case RESET_ALARM_ROW:
            checkisproperty(RESET_ALARM_ROW, "resetAlarm", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("BSTimerModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
