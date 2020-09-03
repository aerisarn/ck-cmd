#include "rangesui.h"

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

#define BASE_NUMBER_OF_ROWS 5

#define EVENT_ID_ROW 0
#define PAYLOAD_ROW 1
#define MINIMUM_DISTANCE_ROW 2
#define MAXIMUM_DISTANCE_ROW 3
#define IGNORE_HANDLE_ROW 4

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList RangesUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

RangesUI::RangesUI()
    : file(nullptr),
      rangeIndex(-1),
      bsData(nullptr),
      parent(nullptr),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      payload(new QLineEdit),
      minDistance(new DoubleSpinBox),
      maxDistance(new DoubleSpinBox),
      ignoreHandle(new CheckBox)
{
    setTitle("hkRanges");
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
    table->setItem(MINIMUM_DISTANCE_ROW, NAME_COLUMN, new TableWidgetItem("minDistance"));
    table->setItem(MINIMUM_DISTANCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MINIMUM_DISTANCE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MINIMUM_DISTANCE_ROW, VALUE_COLUMN, minDistance);
    table->setItem(MAXIMUM_DISTANCE_ROW, NAME_COLUMN, new TableWidgetItem("maxDistance"));
    table->setItem(MAXIMUM_DISTANCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(MAXIMUM_DISTANCE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(MAXIMUM_DISTANCE_ROW, VALUE_COLUMN, maxDistance);
    table->setItem(IGNORE_HANDLE_ROW, NAME_COLUMN, new TableWidgetItem("ignoreHandle"));
    table->setItem(IGNORE_HANDLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(IGNORE_HANDLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(IGNORE_HANDLE_ROW, VALUE_COLUMN, ignoreHandle);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 6, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void RangesUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(minDistance, SIGNAL(editingFinished()), this, SLOT(setMinDistance()), Qt::UniqueConnection);
        connect(maxDistance, SIGNAL(editingFinished()), this, SLOT(setMaxDistance()), Qt::UniqueConnection);
        connect(ignoreHandle, SIGNAL(released()), this, SLOT(setIgnoreHandle()), Qt::UniqueConnection);
        connect(payload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(minDistance, SIGNAL(editingFinished()), this, SLOT(setMinDistance()));
        disconnect(maxDistance, SIGNAL(editingFinished()), this, SLOT(setMaxDistance()));
        disconnect(ignoreHandle, SIGNAL(released()), this, SLOT(setIgnoreHandle()));
        disconnect(payload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void RangesUI::loadData(BehaviorFile *parentFile, hkbSenseHandleModifier::hkRanges *ranges, hkbSenseHandleModifier *par, int index){
    toggleSignals(false);
    if (parentFile && ranges && par && index > -1){
        parent = par;
        rangeIndex = index;
        file = parentFile;
        bsData = ranges;
        auto eventName = file->getEventNameAt(bsData->event.id);
        auto item = table->item(EVENT_ID_ROW, VALUE_COLUMN);
        (eventName != "") ? item->setText(eventName) : item->setText("NONE");
        (ranges->event.payload.data()) ? payload->setText(static_cast<hkbStringEventPayload *>(ranges->event.payload.data())->getData()) : payload->setText("");
        minDistance->setValue(bsData->minDistance);
        maxDistance->setValue(bsData->maxDistance);
        ignoreHandle->setChecked(bsData->ignoreHandle);
        auto varBind = parent->getVariableBindingSetData();
        UIHelper::loadBinding(MINIMUM_DISTANCE_ROW, BINDING_COLUMN, varBind, "ranges:"+QString::number(rangeIndex)+"/minDistance", table, parent);
        UIHelper::loadBinding(MAXIMUM_DISTANCE_ROW, BINDING_COLUMN, varBind, "ranges:"+QString::number(rangeIndex)+"/maxDistance", table, parent);
        UIHelper::loadBinding(IGNORE_HANDLE_ROW, BINDING_COLUMN, varBind, "ranges:"+QString::number(rangeIndex)+"/ignoreHandle", table, parent);
    }else{
        LogFile::writeToLog("RangesUI::loadData(): Behavior file, bind or data is null!!!");
    }
    toggleSignals(true);
}

void RangesUI::setBindingVariable(int index, const QString & name){
    if (bsData){
        auto row = table->currentRow();
        auto checkisproperty = [&](int row, const QString & fieldname, hkVariableType type){
            bool isProperty;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? isProperty = true : isProperty = false;
            UIHelper::setBinding(index, row, BINDING_COLUMN, name, fieldname, type, isProperty, table, parent);
        };
        switch (row){
        case MINIMUM_DISTANCE_ROW:
            checkisproperty(MINIMUM_DISTANCE_ROW, "ranges:"+QString::number(rangeIndex)+"/minDistance", VARIABLE_TYPE_REAL); break;
        case MAXIMUM_DISTANCE_ROW:
            checkisproperty(MAXIMUM_DISTANCE_ROW, "ranges:"+QString::number(rangeIndex)+"/maxDistance", VARIABLE_TYPE_REAL); break;
        case IGNORE_HANDLE_ROW:
            checkisproperty(IGNORE_HANDLE_ROW, "ranges:"+QString::number(rangeIndex)+"/ignoreHandle", VARIABLE_TYPE_BOOL); break;
        }
    }else{
        LogFile::writeToLog("RangesUI::setBindingVariable(): The data is nullptr!!");
    }
}

void RangesUI::setEventId(int index, const QString & name){
    if (bsData && file){
        if (bsData->event.id != --index){
            bsData->event.id = index;
            table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
            file->setIsChanged(true);
        }
    }else{
        LogFile::writeToLog("RangesUI::setEvent(): Behavior file or data is null!!!");
    }
}

void RangesUI::setEventPayload(){
    if (bsData && file){
        auto payloadData = static_cast<hkbStringEventPayload *>(bsData->event.payload.data());
        if (payload->text() != ""){
            if (payloadData){
                if (payloadData->getData() != payload->text()){
                    payloadData->setData(payload->text());
                }else{
                    return;
                }
            }else{
                bsData->event.payload = HkxSharedPtr(new hkbStringEventPayload(file, payload->text()));
            }
        }else{
            bsData->event.payload = HkxSharedPtr();
        }
        file->setIsChanged(true);
    }else{
        LogFile::writeToLog("RangesUI::setEventPayload(): Behavior file or data is null!!!");
    }
}

void RangesUI::setMinDistance(){
    if (bsData && file){
        (bsData->minDistance != minDistance->value()) ? bsData->minDistance = minDistance->value(), file->setIsChanged(true) : LogFile::writeToLog("RangesUI::setminDistance(): minDistance not set!!");
    }else{
        LogFile::writeToLog("RangesUI::setMinDistance(): Behavior file or data is null!!!");
    }
}

void RangesUI::setMaxDistance(){
    if (bsData && file){
        (bsData->maxDistance != maxDistance->value()) ? bsData->maxDistance = maxDistance->value(), file->setIsChanged(true) : LogFile::writeToLog("RangesUI::setmaxDistance(): maxDistance not set!!");
    }else{
        LogFile::writeToLog("RangesUI::setMaxDistance(): Behavior file or data is null!!!");
    }
}

void RangesUI::setIgnoreHandle(){
    if (bsData && file){
        (bsData->ignoreHandle != ignoreHandle->isChecked()) ? bsData->ignoreHandle = ignoreHandle->isChecked(), file->setIsChanged(true) : LogFile::writeToLog("RangesUI::setignoreHandle(): ignoreHandle not set!!");
    }else{
        LogFile::writeToLog("RangesUI::setIsAnnotation(): Behavior file or data is null!!!");
    }
}

void RangesUI::viewSelectedChild(int row, int column){
    if (bsData){
        auto checkisproperty = [&](int row, const QString & fieldname){
            bool properties;
            (table->item(row, BINDING_COLUMN)->checkState() != Qt::Unchecked) ? properties = true : properties = false;
            selectTableToView(properties, fieldname);
        };
        if (column == BINDING_COLUMN){
            switch (row){
            case MINIMUM_DISTANCE_ROW:
                checkisproperty(MINIMUM_DISTANCE_ROW, "ranges:"+QString::number(rangeIndex)+"/minDistance"); break;
            case MAXIMUM_DISTANCE_ROW:
                checkisproperty(MAXIMUM_DISTANCE_ROW, "ranges:"+QString::number(rangeIndex)+"/maxDistance"); break;
            case IGNORE_HANDLE_ROW:
                checkisproperty(IGNORE_HANDLE_ROW, "ranges:"+QString::number(rangeIndex)+"/ignoreHandle"); break;
            }
        }else if (row == EVENT_ID_ROW && column == VALUE_COLUMN){
            emit viewEvents(bsData->event.id + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("RangesUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void RangesUI::selectTableToView(bool viewproperties, const QString & path){
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
        LogFile::writeToLog("RangesUI::selectTableToView(): The data is nullptr!!");
    }
}

void RangesUI::eventRenamed(const QString & name, int index){
    if (bsData){
        (index == bsData->event.id) ? table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
    }else{
        LogFile::writeToLog("RangesUI::eventRenamed(): The data is nullptr!!");
    }
}

void RangesUI::variableRenamed(const QString & name, int index){
    if (parent){
        index--;
        auto bind = parent->getVariableBindingSetData();
        if (bind){
            auto setname = [&](const QString & fieldname, int row){
                auto bindIndex = bind->getVariableIndexOfBinding(fieldname);
                (bindIndex == index) ? table->item(row, BINDING_COLUMN)->setText(name) : NULL;
            };
            setname("ranges:"+QString::number(rangeIndex)+"/minDistance", MINIMUM_DISTANCE_ROW);
            setname("ranges:"+QString::number(rangeIndex)+"/maxDistance", MAXIMUM_DISTANCE_ROW);
            setname("ranges:"+QString::number(rangeIndex)+"/ignoreHandle", IGNORE_HANDLE_ROW);
        }
    }else{
        LogFile::writeToLog("RangesUI::variableRenamed(): The data is nullptr!!");
    }
}
