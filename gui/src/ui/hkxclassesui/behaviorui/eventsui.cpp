#include "eventsui.h"

#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphstringdata.h"
#include "src/ui/genericdatawidgets.h"

#include <QPushButton>
#include <QMessageBox>
#include <QStackedLayout>
#include <QStringList>
#include <QSignalMapper>
#include <QTableWidget>
#include <QHeaderView>
#include <QCoreApplication>

using namespace UI;

const QStringList EventsUI::headerLabels = {
    "Name",
    "Type",
    "Value"
};

EventsUI::EventsUI(const QString &title)
    : dataUI(nullptr),
      verLyt(new QVBoxLayout),
      loadedData(nullptr),
      table(new TableWidget),
      addObjectPB(new QPushButton("Add Event")),
      removeObjectPB(new QPushButton("Remove Selected Event")),
      buttonLyt(new QHBoxLayout),
      eventName(new LineEdit),
      flag(new CheckBox("Flag As Sync Point")),
      eventWidget(new TableWidget),
      stackLyt(new QStackedLayout),
      returnPB(new QPushButton("Return To Parent"))
{
    setTitle(title);
    stackLyt->addWidget(table);
    stackLyt->addWidget(eventWidget);
    stackLyt->setCurrentIndex(TABLE_WIDGET);
    eventWidget->setRowCount(1);
    eventWidget->setColumnCount(3);
    eventWidget->setCellWidget(0, 0, eventName);
    eventWidget->setCellWidget(0, 1, flag);
    eventWidget->setCellWidget(0, 2, returnPB);
    buttonLyt->addWidget(addObjectPB, 1);
    buttonLyt->addSpacing(2);
    buttonLyt->addWidget(removeObjectPB, 1);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(headerLabels);
    verLyt->addLayout(buttonLyt, 1);
    verLyt->addLayout(stackLyt, 10);
    setLayout(verLyt);
    connect(flag, SIGNAL(released()), this, SLOT(setBoolVariableValue()), Qt::UniqueConnection);
    connect(removeObjectPB, SIGNAL(pressed()), this, SLOT(removeEvent()), Qt::UniqueConnection);
    connect(addObjectPB, SIGNAL(pressed()), this, SLOT(addEvent()), Qt::UniqueConnection);
    connect(eventName, SIGNAL(editingFinished()), this, SLOT(renameSelectedEvent()), Qt::UniqueConnection);
    connect(table, SIGNAL(cellClicked(int,int)), this, SLOT(viewEvent(int,int)), Qt::UniqueConnection);
    connect(returnPB, SIGNAL(released()), this, SLOT(returnToTable()), Qt::UniqueConnection);
}

void EventsUI::viewEvent(int row, int column){
    if (column == 2 && loadedData && loadedData->eventInfos.size() > row){
        auto events = static_cast<hkbBehaviorGraphStringData *>(loadedData->stringData.data());
        disconnect(eventName, 0, this, 0);
        disconnect(flag, 0, this, 0);
        eventName->setText(events->getEventNameAt(row));
        (loadedData->getEventInfoAt(row) == "FLAG_SYNC_POINT") ? flag->setChecked(true) : flag->setChecked(false);
        connect(flag, SIGNAL(released()), this, SLOT(setBoolVariableValue()), Qt::UniqueConnection);
        connect(eventName, SIGNAL(editingFinished()), this, SLOT(renameSelectedEvent()), Qt::UniqueConnection);
        stackLyt->setCurrentIndex(EVENT_WIDGET);
    }
}

void EventsUI::returnToTable(){
    stackLyt->setCurrentIndex(TABLE_WIDGET);
}

void EventsUI::setBoolVariableValue(){
    if (loadedData){
        loadedData->setEventFlagAt(table->currentRow(), flag->isChecked());
        loadedData->setIsFileChanged(true);
    }else{
        LogFile::writeToLog("EventsUI: loadedData is nullptr!!");
    }
}

void EventsUI::renameSelectedEvent(){
    if (loadedData){
        auto newName = eventName->text();
        table->item(table->currentRow(), 0)->setText(newName);
        loadedData->setEventNameAt(table->currentRow(), newName);
        loadedData->setIsFileChanged(true);
        emit eventNameChanged(newName, table->currentRow());
    }else{
        LogFile::writeToLog("EventsUI: loadedData is nullptr!!");
    }
}

void EventsUI::loadData(HkxObject *data){
    if (data){
        if (data->getSignature() == HKB_BEHAVIOR_GRAPH_DATA){
            loadedData = static_cast<hkbBehaviorGraphData *>(data);
            auto eventNames = loadedData->getEventNames();
            for (auto i = 0; i < eventNames.size(); i++){
                auto row = table->rowCount();
                if (table->rowCount() > i){
                    table->setRowHidden(i, false);
                    (table->item(row, 0)) ? table->item(row, 0)->setText(eventNames.at(i)) : table->setItem(row, 0, new QTableWidgetItem(eventNames.at(i)));
                }else{
                    table->setRowCount(row + 1);
                    table->setItem(row, 0, new QTableWidgetItem(eventNames.at(i)));
                    table->setItem(row, 1, new QTableWidgetItem("hkEvent"));
                    table->setItem(row, 2, new QTableWidgetItem("Edit"));
                }
            }
            for (auto j = eventNames.size(); j < table->rowCount(); j++){
                table->setRowHidden(j, true);
            }
        }
    }else{
        LogFile::writeToLog("EventsUI: loadedData is nullptr!!");
    }
}

void EventsUI::clear(){
    returnToTable();
    for (auto i = table->rowCount() - 1; i >= 0; i--){
        table->removeRow(i);
    }
}

void EventsUI::addEvent(){
    if (loadedData){
        loadedData->addEvent();
        auto row = table->rowCount();
        auto event = loadedData->getEventNames().last();
        table->setRowCount(row + 1);
        table->setItem(row, 0, new QTableWidgetItem(event));
        table->setItem(row, 1, new QTableWidgetItem("hkEvent"));
        table->setItem(row, 2, new QTableWidgetItem("Edit"));
        (stackLyt->currentIndex() == EVENT_WIDGET) ? stackLyt->setCurrentIndex(TABLE_WIDGET) : NULL;
        table->setCurrentCell(row, 0);
        emit eventAdded(event);
    }else{
        LogFile::writeToLog("EventsUI: loadedData is nullptr!!");
    }
}

void EventsUI::removeEvent(){
    if (loadedData){
        disconnect(removeObjectPB, SIGNAL(pressed()), this, SLOT(removeEvent()));
        auto index = table->currentRow();
        auto message = static_cast<BehaviorFile *>(loadedData->getParentFile())->isEventReferenced(index);
        if (message == ""){
            loadedData->removeEvent(index);
            (index < table->rowCount()) ? table->removeRow(index) : NULL;
            (stackLyt->currentIndex() == EVENT_WIDGET) ? stackLyt->setCurrentIndex(TABLE_WIDGET) : NULL;
            loadedData->setIsFileChanged(true);
            static_cast<BehaviorFile *>(loadedData->getParentFile())->updateEventIndices(index);
            emit eventRemoved(index);
            table->setFocus();
        }else{
            WARNING_MESSAGE(message);
        }
        connect(removeObjectPB, SIGNAL(pressed()), this, SLOT(removeEvent()), Qt::UniqueConnection);
    }
}

void EventsUI::setHkDataUI(HkDataUI *ui){
    dataUI = ui;
}
