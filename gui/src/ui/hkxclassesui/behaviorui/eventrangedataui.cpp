#include "eventrangedataui.h"

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

#define BASE_NUMBER_OF_ROWS 4

#define EVENT_ID_ROW 0
#define PAYLOAD_ROW 1
#define UPPER_BOUND_ROW 2
#define EVENT_MODE_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

const QStringList EventRangeDataUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

EventRangeDataUI::EventRangeDataUI()
    : file(nullptr),
      rangeIndex(-1),
      bsData(nullptr),
      parent(nullptr),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      payload(new QLineEdit),
      upperBound(new DoubleSpinBox),
      eventMode(new ComboBox)
{
    setTitle("hkbEventRangeData");
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
    table->setItem(UPPER_BOUND_ROW, NAME_COLUMN, new TableWidgetItem("upperBound"));
    table->setItem(UPPER_BOUND_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(UPPER_BOUND_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(UPPER_BOUND_ROW, VALUE_COLUMN, upperBound);
    table->setItem(EVENT_MODE_ROW, NAME_COLUMN, new TableWidgetItem("eventMode"));
    table->setItem(EVENT_MODE_ROW, TYPE_COLUMN, new TableWidgetItem("EventMode", Qt::AlignCenter));
    table->setItem(EVENT_MODE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(EVENT_MODE_ROW, VALUE_COLUMN, eventMode);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 6, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void EventRangeDataUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(upperBound, SIGNAL(editingFinished()), this, SLOT(setUpperBound()), Qt::UniqueConnection);
        connect(eventMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setEventMode(int)), Qt::UniqueConnection);
        connect(payload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(upperBound, SIGNAL(editingFinished()), this, SLOT(setUpperBound()));
        disconnect(eventMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setEventMode(int)));
        disconnect(payload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void EventRangeDataUI::loadData(BehaviorFile *parentFile, hkbEventRangeDataArray::hkbEventRangeData *ranges, hkbEventRangeDataArray *par, int index){
    toggleSignals(false);
    if (parentFile && ranges && par && index > -1){
        parent = par;
        rangeIndex = index;
        file = parentFile;
        bsData = ranges;
        auto item = table->item(EVENT_ID_ROW, VALUE_COLUMN);
        auto eventName = file->getEventNameAt(bsData->event.id);
        (eventName != "") ? item->setText(eventName) : item->setText("NONE");
        (ranges->event.payload.data()) ? payload->setText(static_cast<hkbStringEventPayload *>(ranges->event.payload.data())->getData()) : payload->setText("");
        upperBound->setValue(bsData->upperBound);
        (!eventMode->count()) ? eventMode->insertItems(0, bsData->EventRangeMode) : NULL;
        eventMode->setCurrentIndex(bsData->EventRangeMode.indexOf(bsData->eventMode));
    }else{
        LogFile::writeToLog("EventRangeDataUI::loadData(): Behavior file, bind or data is null!!!");
    }
    toggleSignals(true);
}

void EventRangeDataUI::setEventId(int index, const QString & name){
    if (bsData && file){
        if (bsData->event.id != --index){
            bsData->event.id = index;
            table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
            file->setIsChanged(true);
        }
    }else{
        LogFile::writeToLog("EventRangeDataUI::setEvent(): Behavior file or data is null!!!");
    }
}

void EventRangeDataUI::setEventPayload(){
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
        LogFile::writeToLog("EventRangeDataUI::setEventPayload(): Behavior file or data is null!!!");
    }
}

void EventRangeDataUI::setUpperBound(){
    if (bsData && file){
        (bsData->upperBound != upperBound->value()) ? bsData->upperBound = upperBound->value(), file->setIsChanged(true) : NULL;
    }else{
        LogFile::writeToLog("EventRangeDataUI::setupperBound(): Behavior file or data is null!!!");
    }
}

void EventRangeDataUI::setEventMode(int index){
    (bsData) ? bsData->eventMode = bsData->EventRangeMode.at(index), file->setIsChanged(true) : LogFile::writeToLog("EventRangeDataUI::setEventMode(): The data is nullptr!!");
}

void EventRangeDataUI::viewSelectedChild(int row, int column){
    if (bsData){
        if (row == EVENT_ID_ROW && column == VALUE_COLUMN){
            emit viewEvents(bsData->event.id + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("EventRangeDataUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void EventRangeDataUI::eventRenamed(const QString & name, int index){
    if (bsData){
        (index == bsData->event.id) ? table->item(EVENT_ID_ROW, VALUE_COLUMN)->setText(name) : NULL;
    }else{
        LogFile::writeToLog("EventRangeDataUI::eventRenamed(): The data is nullptr!!");
    }
}

