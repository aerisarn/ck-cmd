#include "legui.h"

#include "src/utility.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define EVENT_ID_ROW 0
#define PAYLOAD_ROW 1
#define GROUND_POSITION_ROW 2
#define VERTICAL_ERROR_ROW 3
#define HIT_SOMETHING_ROW 4
#define IS_PLANTED_MS_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList LegUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

LegUI::LegUI()
    : file(nullptr),
      legIndex(-1),
      bsData(nullptr),
      parent(nullptr),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      groundPosition(new QuadVariableWidget),
      payload(new QLineEdit),
      verticalError(new DoubleSpinBox),
      hitSomething(new CheckBox),
      isPlantedMS(new CheckBox)
{
    setTitle("hkLeg");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("eventId"));
    table->setItem(EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("payload"));
    table->setItem(PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setItem(PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(PAYLOAD_ROW, VALUE_COLUMN, payload);
    table->setItem(GROUND_POSITION_ROW, NAME_COLUMN, new TableWidgetItem("groundPosition"));
    table->setItem(GROUND_POSITION_ROW, TYPE_COLUMN, new TableWidgetItem("hkVector4", Qt::AlignCenter));
    table->setItem(GROUND_POSITION_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(GROUND_POSITION_ROW, VALUE_COLUMN, groundPosition);
    table->setItem(VERTICAL_ERROR_ROW, NAME_COLUMN, new TableWidgetItem("verticalError"));
    table->setItem(VERTICAL_ERROR_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(VERTICAL_ERROR_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(VERTICAL_ERROR_ROW, VALUE_COLUMN, verticalError);
    table->setItem(HIT_SOMETHING_ROW, NAME_COLUMN, new TableWidgetItem("hitSomething"));
    table->setItem(HIT_SOMETHING_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(HIT_SOMETHING_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(HIT_SOMETHING_ROW, VALUE_COLUMN, hitSomething);
    table->setItem(IS_PLANTED_MS_ROW, NAME_COLUMN, new TableWidgetItem("isPlantedMS"));
    table->setItem(IS_PLANTED_MS_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IS_PLANTED_MS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IS_PLANTED_MS_ROW, VALUE_COLUMN, isPlantedMS);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 6, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void LegUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(groundPosition, SIGNAL(editingFinished()), this, SLOT(setGroundPosition()), Qt::UniqueConnection);
        connect(verticalError, SIGNAL(editingFinished()), this, SLOT(setVerticalError()), Qt::UniqueConnection);
        connect(hitSomething, SIGNAL(released()), this, SLOT(setHitSomething()), Qt::UniqueConnection);
        connect(isPlantedMS, SIGNAL(released()), this, SLOT(setIsPlantedMS()), Qt::UniqueConnection);
        connect(payload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(groundPosition, SIGNAL(editingFinished()), this, SLOT(setGroundPosition()));
        disconnect(verticalError, SIGNAL(editingFinished()), this, SLOT(setVerticalError()));
        disconnect(hitSomething, SIGNAL(released()), this, SLOT(setHitSomething()));
        disconnect(isPlantedMS, SIGNAL(released()), this, SLOT(setIsPlantedMS()));
        disconnect(payload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void LegUI::loadData(BehaviorFile *parentFile, hkbFootIkControlsModifier::hkLeg *leg, hkbFootIkControlsModifier *par, int index){
    toggleSignals(false);
    if (parentFile && leg && par && index > -1){
        parent = par;
        legIndex = index;
        file = parentFile;
        bsData = leg;
        auto eventName = file->getEventNameAt(bsData->id);
        auto item = table->item(EVENT_ID_ROW, VALUE_COLUMN);
        (eventName != "") ? item->setText(eventName) : item->setText("NONE");
        (leg->payload.data()) ? payload->setText(static_cast<hkbStringEventPayload *>(leg->payload.data())->getData()) : payload->setText("");
        groundPosition->setValue(bsData->groundPosition);
        verticalError->setValue(bsData->verticalError);
        hitSomething->setChecked(bsData->hitSomething);
        isPlantedMS->setChecked(bsData->isPlantedMS);
        auto varBind = parent->getVariableBindingSetData();
        UIHelper::loadBinding(GROUND_POSITION_ROW, BINDING_COLUMN, varBind, "legs:"+QString::number(legIndex)+"/groundPosition", table, parent);
        UIHelper::loadBinding(VERTICAL_ERROR_ROW, BINDING_COLUMN, varBind, "legs:"+QString::number(legIndex)+"/verticalError", table, parent);
        UIHelper::loadBinding(HIT_SOMETHING_ROW, BINDING_COLUMN, varBind, "legs:"+QString::number(legIndex)+"/hitSomething", table, parent);
        UIHelper::loadBinding(IS_PLANTED_MS_ROW, BINDING_COLUMN, varBind, "legs:"+QString::number(legIndex)+"/isPlantedMS", table, parent);
    }else{
        LogFile::writeToLog("LegUI::loadData(): Behavior file, bind or data is null!!!");
    }
    toggleSignals(true);
}

void LegUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, parent);
        };
        switch (row){
        case GROUND_POSITION_ROW:
            checkisproperty(GROUND_POSITION_ROW, "legs:"+QString::number(legIndex)+"/groundPosition", VARIABLE_TYPE_VECTOR4); break;
        case VERTICAL_ERROR_ROW:
            checkisproperty(VERTICAL_ERROR_ROW, "legs:"+QString::number(legIndex)+"/verticalError", VARIABLE_TYPE_REAL); break;
        case HIT_SOMETHING_ROW:
            checkisproperty(HIT_SOMETHING_ROW, "legs:"+QString::number(legIndex)+"/hitSomething", VARIABLE_TYPE_BOOL); break;
        case IS_PLANTED_MS_ROW:
            checkisproperty(IS_PLANTED_MS_ROW, "legs:"+QString::number(legIndex)+"/isPlantedMS", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("LegUI::setBindingVariable(): The data is nullptr!!");
    }
}

void LegUI::setEventId(int index, const QString & name){
    if (bsData && file){
        if (bsData->id != --index){
            bsData->id = index;
            table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
            file->setIsChanged(true);
        }
    }else{
        LogFile::writeToLog("LegUI::setEvent(): Behavior file or data is null!!!");
    }
}

void LegUI::setEventPayload(){
    if (bsData && file){
        auto payloadData = static_cast<hkbStringEventPayload *>(bsData->payload.data());
        if (payload->text() != ""){
            if (payloadData){
                if (payloadData->getData() != payload->text()){
                    payloadData->setData(payload->text());
                }else{
                    return;
                }
            }else{
                bsData->payload = HkxSharedPtr(new hkbStringEventPayload(file, payload->text()));
            }
        }else{
            bsData->payload = HkxSharedPtr();
        }
        file->setIsChanged(true);
    }else{
        LogFile::writeToLog("LegUI::setEventPayload(): Behavior file or data is null!!!");
    }
}

void LegUI::setGroundPosition(){
    if (bsData && file){
        (bsData->groundPosition != groundPosition->value()) ? bsData->groundPosition = groundPosition->value(), file->setIsChanged(true) : LogFile::writeToLog("LegUI::setgroundPosition(): groundPosition not set!!");
    }else{
        LogFile::writeToLog("LegUI::setGroundPosition(): Behavior file or data is null!!!");
    }
}

void LegUI::setVerticalError(){
    if (bsData && file){
        (bsData->verticalError != verticalError->value()) ? bsData->verticalError = verticalError->value(), file->setIsChanged(true) : LogFile::writeToLog("LegUI::setverticalError(): verticalError not set!!");
    }else{
        LogFile::writeToLog("LegUI::setverticalError(): Behavior file or data is null!!!");
    }
}

void LegUI::setHitSomething(){
    if (bsData && file){
        (bsData->hitSomething != hitSomething->isChecked()) ? bsData->hitSomething = hitSomething->isChecked(), file->setIsChanged(true) : LogFile::writeToLog("LegUI::sethitSomething(): hitSomething not set!!");
    }else{
        LogFile::writeToLog("LegUI::sethitSomething(): Behavior file or data is null!!!");
    }
}

void LegUI::setIsPlantedMS(){
    if (bsData && file){
        (bsData->isPlantedMS != isPlantedMS->isChecked()) ? bsData->isPlantedMS = isPlantedMS->isChecked(), file->setIsChanged(true) : LogFile::writeToLog("LegUI::setisPlantedMS(): isPlantedMS not set!!");
    }else{
        LogFile::writeToLog("LegUI::setIsPlantedMS(): Behavior file or data is null!!!");
    }
}

void LegUI::viewSelectedChild(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case GROUND_POSITION_ROW:
                checkisproperty(GROUND_POSITION_ROW, "legs:"+QString::number(legIndex)+"/groundPosition"); break;
            case VERTICAL_ERROR_ROW:
                checkisproperty(VERTICAL_ERROR_ROW, "legs:"+QString::number(legIndex)+"/verticalError"); break;
            case HIT_SOMETHING_ROW:
                checkisproperty(HIT_SOMETHING_ROW, "legs:"+QString::number(legIndex)+"/hitSomething"); break;
            case IS_PLANTED_MS_ROW:
                checkisproperty(IS_PLANTED_MS_ROW, "legs:"+QString::number(legIndex)+"/isPlantedMS"); break;
            }
        }else if (row == EVENT_ID_ROW && column == VALUE_COLUMN){
                emit viewEvents(bsData->id + 1, QString(), QStringList());
            }
    }else{
        LogFile::writeToLog("LegUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void LegUI::selectTableToView(bool viewproperties, const QString & path){
    if (bsData){
        if (viewproperties){
            if (parent->getVariableBindingSetData()){
                emit viewProperties(static_cast<hkbVariableBindingSet *>(parent->getVariableBindingSetData())->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewProperties(0, QString(), QStringList());
            }
        }else{
            if (parent->getVariableBindingSetData()){
                emit viewVariables(static_cast<hkbVariableBindingSet *>(parent->getVariableBindingSetData())->getVariableIndexOfBinding(path) + 1, QString(), QStringList());
            }else{
                emit viewVariables(0, QString(), QStringList());
            }
        }
    }else{
        LogFile::writeToLog("LegUI::selectTableToView(): The data is nullptr!!");
    }
}

void LegUI::eventRenamed(const QString & name, int index){
    if (bsData){
        (index == bsData->id) ? table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
    }else{
        LogFile::writeToLog("LegUI::eventRenamed(): The data is nullptr!!");
    }
}

void LegUI::variableRenamed(const QString & name, int index){
    if (parent){
        index--;
        auto bind = parent->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("legs:"+QString::number(legIndex)+"/groundPosition", GROUND_POSITION_ROW);
            setname("legs:"+QString::number(legIndex)+"/verticalError", VERTICAL_ERROR_ROW);
            setname("legs:"+QString::number(legIndex)+"/hitSomething", HIT_SOMETHING_ROW);
            setname("legs:"+QString::number(legIndex)+"/isPlantedMS", IS_PLANTED_MS_ROW);
        }
    }else{
        LogFile::writeToLog("LegUI::variableRenamed(): The data is nullptr!!");
    }
}
