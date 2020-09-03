#include "eventui.h"
#include "src/utility.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/hkxobject.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace UI;

EventUI::EventUI()
    : file(nullptr),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      nameL(new QLabel("Event Name:")),
      selectEvent(new QPushButton("NONE")),
      payloadL(new QLabel("Payload String:")),
      eventPayload(new QLineEdit),
      eventData(nullptr)
{
    setMinimumHeight(400);
    setMaximumHeight(400);
    setMinimumWidth(600);
    setMaximumWidth(600);
    //topLyt->setSizeConstraint(QLayout::SetMaximumSize);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setTitle("Event Payload Editor");
    topLyt->addWidget(returnPB, 0, 2, 1, 1);
    topLyt->addWidget(nameL, 2, 0, 1, 1);
    topLyt->addWidget(selectEvent, 2, 2, 1, 2);
    topLyt->addWidget(payloadL, 4, 0, 1, 1);
    topLyt->addWidget(eventPayload, 4, 2, 1, 2);
    setLayout(topLyt);
    toggleSignals(true);
}

void EventUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(selectEvent, SIGNAL(released()), this, SLOT(emitViewEvent()), Qt::UniqueConnection);
        connect(eventPayload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(selectEvent, SIGNAL(released()), this, SLOT(emitViewEvent()));
        disconnect(eventPayload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()));
    }
}

void EventUI::loadData(BehaviorFile *parentFile, hkEventPayload * event){
    toggleSignals(false);
    if (parentFile && event){
        file = parentFile;
        eventData = event;
        auto text = file->getEventNameAt(event->id);
        (text == "") ? text = "NONE" : NULL;
        selectEvent->setText(text);
        (event->payload.data()) ? eventPayload->setText(static_cast<hkbStringEventPayload *>(event->payload.data())->getData()) : eventPayload->setText("");
    }else{
        LogFile::writeToLog("EventUI::loadData(): Behavior file or data is null!!!");
    }
    toggleSignals(true);
}

QSize EventUI::sizeHint() const{
    return QSize(1600, 800);
}

QSize EventUI::minimumSizeHint() const{
    return QSize(1200, 600);
}

void EventUI::setEvent(int index, const QString & name){
    if (eventData && file){
        eventData->id = --index;
        selectEvent->setText(name);
        file->setIsChanged(true);
    }else{
        LogFile::writeToLog("EventUI::setEvent(): Behavior file or data is null!!!");
    }
}

void EventUI::setEventPayload(){
    if (eventData && file){
        auto payload = static_cast<hkbStringEventPayload *>(eventData->payload.data());
        if (eventPayload->text() != ""){
            (payload) ? payload->setData(eventPayload->text()) : eventData->payload = HkxSharedPtr(new hkbStringEventPayload(file, eventPayload->text()));
        }else{
            eventData->payload = HkxSharedPtr();
        }
        file->setIsChanged(true);
    }else{
        LogFile::writeToLog("EventUI::setEventPayload(): Behavior file or data is null!!!");
    }
}

void EventUI::emitViewEvent(){
    if (eventData){
        emit viewEvents(eventData->id + 1, QString(), QStringList());
    }else{
        LogFile::writeToLog("EventUI::emitViewEvent(): Event data is null!!!");
    }
}

void EventUI::eventRenamed(const QString & name, int index){
    if (eventData){
        (index == eventData->id) ? selectEvent->setText(name) : NULL;
    }else{
        LogFile::writeToLog("EventUI::eventRenamed(): The data is nullptr!!");
    }
}
