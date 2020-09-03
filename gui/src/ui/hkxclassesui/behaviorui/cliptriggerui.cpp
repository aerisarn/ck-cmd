#include "cliptriggerui.h"

#include "src/utility.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/generators/hkbclipgenerator.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace UI;

#define BASE_NUMBER_OF_ROWS 6

#define EVENT_ROW 0
#define PAYLOAD_ROW 1
#define LOCAL_TIME_ROW 2
#define RELATIVE_TO_END_ROW 3
#define ACYCLIC_ROW 4
#define ISANNOTATION_ROW 5

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define VALUE_COLUMN 2

const QStringList ClipTriggerUI::headerLabels = {
    "Name",
    "Type",
    "Value"
};

ClipTriggerUI::ClipTriggerUI()
    : file(nullptr),
      bsData(nullptr),
      indexOfTrigger(-1),
      topLyt(new QGridLayout),
      returnPB(new QPushButton("Return")),
      table(new TableWidget),
      payload(new QLineEdit),
      localTime(new DoubleSpinBox),
      relativeToEndOfClip(new CheckBox),
      acyclic(new CheckBox),
      isAnnotation(new CheckBox)
{
    setTitle("Clip Trigger Editor");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(EVENT_ROW, NAME_COLUMN, new TableWidgetItem("event"));
    table->setItem(EVENT_ROW, TYPE_COLUMN, new TableWidgetItem("hkEvent", Qt::AlignCenter));
    table->setItem(EVENT_ROW, VALUE_COLUMN, new TableWidgetItem("NONE", Qt::AlignCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("payload"));
    table->setItem(PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkbStringEventPayload", Qt::AlignCenter));
    table->setCellWidget(PAYLOAD_ROW, VALUE_COLUMN, payload);
    table->setItem(LOCAL_TIME_ROW, NAME_COLUMN, new TableWidgetItem("localTime"));
    table->setItem(LOCAL_TIME_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(LOCAL_TIME_ROW, VALUE_COLUMN, localTime);
    table->setItem(RELATIVE_TO_END_ROW, NAME_COLUMN, new TableWidgetItem("relativeToEndOfClip"));
    table->setItem(RELATIVE_TO_END_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(RELATIVE_TO_END_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(RELATIVE_TO_END_ROW, VALUE_COLUMN, relativeToEndOfClip);
    table->setItem(ACYCLIC_ROW, NAME_COLUMN, new TableWidgetItem("acyclic"));
    table->setItem(ACYCLIC_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ACYCLIC_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(ACYCLIC_ROW, VALUE_COLUMN, acyclic);
    table->setItem(ISANNOTATION_ROW, NAME_COLUMN, new TableWidgetItem("isAnnotation"));
    table->setItem(ISANNOTATION_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ISANNOTATION_ROW, VALUE_COLUMN, new TableWidgetItem("", Qt::AlignCenter, QColor(Qt::lightGray)));
    table->setCellWidget(ISANNOTATION_ROW, VALUE_COLUMN, isAnnotation);
    topLyt->addWidget(returnPB, 0, 1, 1, 1);
    topLyt->addWidget(table, 1, 0, 6, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void ClipTriggerUI::toggleSignals(bool toggleconnections){
    if (toggleconnections){
        connect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()), Qt::UniqueConnection);
        connect(localTime, SIGNAL(editingFinished()), this, SLOT(setLocalTime()), Qt::UniqueConnection);
        connect(relativeToEndOfClip, SIGNAL(released()), this, SLOT(setRelativeToEndOfClip()), Qt::UniqueConnection);
        connect(acyclic, SIGNAL(released()), this, SLOT(setAcyclic()), Qt::UniqueConnection);
        connect(isAnnotation, SIGNAL(released()), this, SLOT(setIsAnnotation()), Qt::UniqueConnection);
        connect(payload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()), Qt::UniqueConnection);
        connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)), Qt::UniqueConnection);
    }else{
        disconnect(returnPB, SIGNAL(released()), this, SIGNAL(returnToParent()));
        disconnect(localTime, SIGNAL(editingFinished()), this, SLOT(setLocalTime()));
        disconnect(relativeToEndOfClip, SIGNAL(released()), this, SLOT(setRelativeToEndOfClip()));
        disconnect(acyclic, SIGNAL(released()), this, SLOT(setAcyclic()));
        disconnect(isAnnotation, SIGNAL(released()), this, SLOT(setIsAnnotation()));
        disconnect(payload, SIGNAL(editingFinished()), this, SLOT(setEventPayload()));
        disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelectedChild(int,int)));
    }
}

void ClipTriggerUI::loadData(BehaviorFile *parentFile, hkbClipGenerator *parent, int index, hkbClipTriggerArray::HkTrigger *trigger){
    toggleSignals(false);
    if (parentFile && parent && trigger){
        file = parentFile;
        parentClipGen = parent;
        indexOfTrigger = index;
        bsData = trigger;
        auto item = table->item(EVENT_ROW, VALUE_COLUMN);
        auto eventName = file->getEventNameAt(bsData->event.id);
        (eventName != "") ? item->setText(eventName) : item->setText("NONE");
        (trigger->event.payload.data()) ? payload->setText(static_cast<hkbStringEventPayload *>(trigger->event.payload.data())->getData()) : payload->setText("");
        localTime->setValue(bsData->localTime);
        relativeToEndOfClip->setChecked(bsData->relativeToEndOfClip);
        acyclic->setChecked(bsData->acyclic);
        isAnnotation->setChecked(bsData->isAnnotation);
    }else{
        LogFile::writeToLog("ClipTriggerUI::loadData(): Behavior file, parent clip generator or data is null!!!");
    }
    toggleSignals(true);
}

void ClipTriggerUI::setEventId(int index, const QString & name){
    if (bsData && file){
        if (bsData->event.id != --index){
            bsData->event.id = index;
            //file->setEventNameForClipGenAnimData(parentClipGen->getName(), indexOfTrigger, index);
            table->item(EVENT_ROW, VALUE_COLUMN)->setText(name);
            file->setIsChanged(true);
        }
    }else{
        LogFile::writeToLog("ClipTriggerUI::setEventId(): Behavior file or data is null!!!");
    }
}

void ClipTriggerUI::setEventPayload(){
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
        LogFile::writeToLog("ClipTriggerUI::setEventPayload(): Behavior file or data is null!!!");
    }
}

void ClipTriggerUI::setLocalTime(){
    if (bsData && file){
        //qreal trigtime;
        if (bsData->localTime != localTime->value()){
            bsData->localTime = localTime->value();
            file->setIsChanged(true);
            /*if (bsData->relativeToEndOfClip){
                trigtime = localTime->value() + file->getAnimationDurationFromAnimData(parentClipGen->getAnimationName());
            }else{
                trigtime = localTime->value();
            }
            file->setLocalTimeForClipGenAnimData(parentClipGen->getName(), indexOfTrigger, trigtime);*/
        }
    }else{
        LogFile::writeToLog("ClipTriggerUI::setLocalTime(): Behavior file or data is null!!!");
    }
}

void ClipTriggerUI::setRelativeToEndOfClip(){
    if (bsData && file){
        //qreal trigtime;
        if (bsData->relativeToEndOfClip != relativeToEndOfClip->isChecked()){
            bsData->relativeToEndOfClip = relativeToEndOfClip->isChecked();
            file->setIsChanged(true);
            /*if (bsData->relativeToEndOfClip){
                trigtime = localTime->value() + file->getAnimationDurationFromAnimData(parentClipGen->getAnimationName());
            }else{
                trigtime = localTime->value();
            }
            file->setLocalTimeForClipGenAnimData(parentClipGen->getName(), indexOfTrigger, trigtime);*/
        }
    }else{
        LogFile::writeToLog("ClipTriggerUI::setRelativeToEndOfClip(): Behavior file or data is null!!!");
    }
}

void ClipTriggerUI::setAcyclic(){
    if (bsData && file){
        (bsData->acyclic != acyclic->isChecked()) ? bsData->acyclic = acyclic->isChecked(), file->setIsChanged(true) : NULL;
    }else{
        LogFile::writeToLog("ClipTriggerUI::setAcyclic(): Behavior file or data is null!!!");
    }
}

void ClipTriggerUI::setIsAnnotation(){
    if (bsData && file){
        (bsData->isAnnotation != isAnnotation->isChecked()) ? bsData->isAnnotation = isAnnotation->isChecked(), file->setIsChanged(true) : NULL;
    }else{
        LogFile::writeToLog("ClipTriggerUI::setIsAnnotation(): Behavior file or data is null!!!");
    }
}

void ClipTriggerUI::viewSelectedChild(int row, int column){
    if (bsData){
        if (row == EVENT_ROW && column == VALUE_COLUMN){
            emit viewEvents(bsData->event.id + 1, QString(), QStringList());
        }
    }else{
        LogFile::writeToLog("ClipTriggerUI::viewSelectedChild(): The data is nullptr!!");
    }
}

void ClipTriggerUI::eventRenamed(const QString & name, int index){
    if (bsData){
        if (index == bsData->event.id){
            table->item(EVENT_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        LogFile::writeToLog("ClipTriggerUI::eventRenamed(): The data is nullptr!!");
    }
}
