#include "bspassbytargettriggermodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/modifiers/BSPassByTargetTriggerModifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 7

#define NAME_ROW 0
#define ENABLE_ROW 1
#define TARGET_POSITION_ROW 2
#define RADIUS_ROW 3
#define MOVEMENT_DIRECTION_ROW 4
#define TRIGGER_EVENT_ID_ROW 5
#define TRIGGER_EVENT_PAYLOAD_ROW 6

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList BSPassByTargetTriggerModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

BSPassByTargetTriggerModifierUI::BSPassByTargetTriggerModifierUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      targetPosition(new QuadVariableWidget),
      radius(new DoubleSpinBox),
      movementDirection(new QuadVariableWidget),
      triggerEventPayload(new QLineEdit)
{
    setTitle("BSPassByTargetTriggerModifierUI");
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
    table->setItem(TARGET_POSITION_ROW, NAME_COLUMN, new TableWidgetItem("targetPosition"));
    table->setItem(TARGET_POSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(TARGET_POSITION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(TARGET_POSITION_ROW, VALUE_COLUMN, targetPosition);
    table->setItem(RADIUS_ROW, NAME_COLUMN, new TableWidgetItem("radius"));
    table->setItem(RADIUS_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(RADIUS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(RADIUS_ROW, VALUE_COLUMN, radius);
    table->setItem(MOVEMENT_DIRECTION_ROW, NAME_COLUMN, new TableWidgetItem("movementDirection"));
    table->setItem(MOVEMENT_DIRECTION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(MOVEMENT_DIRECTION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MOVEMENT_DIRECTION_ROW, VALUE_COLUMN, movementDirection);
    table->setItem(TRIGGER_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("triggerEventId"));
    table->setItem(TRIGGER_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(TRIGGER_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(TRIGGER_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(TRIGGER_EVENT_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("triggerEventPayload"));
    table->setItem(TRIGGER_EVENT_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(TRIGGER_EVENT_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(TRIGGER_EVENT_PAYLOAD_ROW, VALUE_COLUMN, triggerEventPayload);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BSPassByTargetTriggerModifierUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
        connect(targetPosition, SIGNAL(editingFinished()), this, SLOT(setTargetPosition()), Qt::UniqueConnection);
        connect(radius, SIGNAL(editingFinished()), this, SLOT(setRadius()), Qt::UniqueConnection);
        connect(movementDirection, SIGNAL(editingFinished()), this, SLOT(setMovementDirection()), Qt::UniqueConnection);
        connect(triggerEventPayload, SIGNAL(editingFinished()), this, SLOT(setTriggerEventPayload()), Qt::UniqueConnection);
        connect(enable, SIGNAL(released()), this, SLOT(setResetAlarm()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(name, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
        disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
        disconnect(targetPosition, SIGNAL(editingFinished()), this, SLOT(setTargetPosition()));
        disconnect(radius, SIGNAL(editingFinished()), this, SLOT(setRadius()));
        disconnect(movementDirection, SIGNAL(editingFinished()), this, SLOT(setMovementDirection()));
        disconnect(triggerEventPayload, SIGNAL(editingFinished()), this, SLOT(setTriggerEventPayload()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
    }
}

void BSPassByTargetTriggerModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && properties && events){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(setTriggerEventId(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int,QString,QStringList)), variables, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int,QString,QStringList)), properties, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int,QString,QStringList)), events, SLOT(showTable(int,QString,QStringList)), Qt::UniqueConnection);
    }else{
        LogFile::writeToLog("BSPassByTargetTriggerModifierUI::connectToTables(): One or more arguments are nullptr!!");
    }
}

void BSPassByTargetTriggerModifierUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BS_PASS_BY_TARGET_TRIGGER_MODIFIER){
            bsData = static_cast<BSPassByTargetTriggerModifier *>(data);
            name->setText(bsData->getName());
            enable->setChecked(bsData->getEnable());
            targetPosition->setValue(bsData->getTargetPosition());
            radius->setValue(bsData->getRadius());
            movementDirection->setValue(bsData->getMovementDirection());
            auto text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->getTriggerEventID());
            (text != "") ? table->item(TRIGGER_EVENT_ID_ROW, VALUE_COLUMN)->setText(text) : table->item(TRIGGER_EVENT_ID_ROW, VALUE_COLUMN)->setText("None");
            auto payload = bsData->getTriggerEventPayload();
            (payload) ? triggerEventPayload->setText(payload->getData()) : triggerEventPayload->setText("");
            auto varBind = bsData->getVariableBindingSetData();
            UIHelper::loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable", table, bsData);
            UIHelper::loadBinding(TARGET_POSITION_ROW, BINDING_COLUMN, varBind, "targetPosition", table, bsData);
            UIHelper::loadBinding(RADIUS_ROW, BINDING_COLUMN, varBind, "radius", table, bsData);
            UIHelper::loadBinding(MOVEMENT_DIRECTION_ROW, BINDING_COLUMN, varBind, "movementDirection", table, bsData);
        }else{
            LogFile::writeToLog("BSPassByTargetTriggerModifierUI::loadData(): The data is an incorrect type!!");
        }
    }else{
        LogFile::writeToLog("BSPassByTargetTriggerModifierUI::loadData(): The data is nullptr!!");
    }
    toggleSignals(true);
}

void BSPassByTargetTriggerModifierUI::setName(const QString &newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
    }else{
        LogFile::writeToLog("BSPassByTargetTriggerModifierUI::setName(): The data is nullptr!!");
    }
}

void BSPassByTargetTriggerModifierUI::setEnable(){
    (bsData) ? bsData->setEnable(enable->isChecked()) : LogFile::writeToLog("BSPassByTargetTriggerModifierUI::setEnable(): The data is nullptr!!");
}

void BSPassByTargetTriggerModifierUI::setTargetPosition(){
    (bsData) ? bsData->setTargetPosition(targetPosition->value()) : LogFile::writeToLog("BSPassByTargetTriggerModifierUI::setTargetPosition(): The data is nullptr!!");
}

void BSPassByTargetTriggerModifierUI::setRadius(){
    (bsData) ? bsData->setRadius(radius->value()) : LogFile::writeToLog("BSPassByTargetTriggerModifierUI::setRadius(): The data is nullptr!!");
}

void BSPassByTargetTriggerModifierUI::setMovementDirection(){
    (bsData) ? bsData->setMovementDirection(movementDirection->value()) : LogFile::writeToLog("BSPassByTargetTriggerModifierUI::setMovementDirection(): The data is nullptr!!");
}

void BSPassByTargetTriggerModifierUI::setTriggerEventId(int index, const QString & name){
    if (bsData){
        bsData->setTriggerEventID(index - 1);
        table->item(TRIGGER_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
    }else{
        LogFile::writeToLog("BSDistTriggerModifierUI::setTriggerEventId(): The data is nullptr!!");
    }
}

void BSPassByTargetTriggerModifierUI::setTriggerEventPayload(){
    if (bsData){
        auto payload = bsData->getTriggerEventPayload();
        if (triggerEventPayload->text() != ""){
            if (payload){
                payload->setData(triggerEventPayload->text());
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), triggerEventPayload->text());
                bsData->setTriggerEventPayload(payload);
            }
        }else{
            bsData->setTriggerEventPayload(nullptr);
        }
    }else{
        LogFile::writeToLog("BSDistTriggerModifierUI::setTriggerEventPayload(): The data is nullptr!!");
    }
}

void BSPassByTargetTriggerModifierUI::viewSelected(int row, int column){
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
            case TARGET_POSITION_ROW:
                checkisproperty(TARGET_POSITION_ROW, "targetPosition"); break;
            case RADIUS_ROW:
                checkisproperty(RADIUS_ROW, "radius"); break;
            case MOVEMENT_DIRECTION_ROW:
                checkisproperty(MOVEMENT_DIRECTION_ROW, "movementDirection"); break;
            }
        }else if (column == VALUE_COLUMN && row == TRIGGER_EVENT_ID_ROW){
            emit viewEvents(bsData->getTriggerEventID() + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("BSPassByTargetTriggerModifierUI::viewSelected(): The 'bsData' pointer is nullptr!!");
    }
}

void BSPassByTargetTriggerModifierUI::selectTableToView(bool viewisProperty, const QString & path){
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
        LogFile::writeToLog("BSPassByTargetTriggerModifierUI::selectTableToView(): The data is nullptr!!");
    }
}

void BSPassByTargetTriggerModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        index--;
        (index == bsData->getTriggerEventID()) ? table->item(TRIGGER_EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
    }else{
        LogFile::writeToLog("BSPassByTargetTriggerModifierUI::eventRenamed(): The data is nullptr!!");
    }
}

void BSPassByTargetTriggerModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        auto bind = bsData->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("enable", ENABLE_ROW);
            setname("targetPosition", TARGET_POSITION_ROW);
            setname("radius", RADIUS_ROW);
            setname("movementDirection", MOVEMENT_DIRECTION_ROW);
        }
    }else{
        LogFile::writeToLog("BSPassByTargetTriggerModifierUI::variableRenamed(): The 'bsData' pointer is nullptr!!");
    }
}

void BSPassByTargetTriggerModifierUI::setBindingVariable(int index, const QString &name){
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
        case TARGET_POSITION_ROW:
            checkisproperty(TARGET_POSITION_ROW, "targetPosition", VARIABLE_TYPE_VECTOR4); break;
        case RADIUS_ROW:
            checkisproperty(RADIUS_ROW, "radius", VARIABLE_TYPE_REAL); break;
        case MOVEMENT_DIRECTION_ROW:
            checkisproperty(MOVEMENT_DIRECTION_ROW, "movementDirection", VARIABLE_TYPE_VECTOR4); break;
        }
    }else{
        LogFile::writeToLog("BSPassByTargetTriggerModifierUI::setBindingVariable(): The data is nullptr!!");
    }
}
