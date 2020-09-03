#include "hkbattachmentmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"

using namespace UI;

uint hkbAttachmentModifier::refCount = 0;

const QString hkbAttachmentModifier::classname = "hkbAttachmentModifier";

hkbAttachmentModifier::hkbAttachmentModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      attacheeLayer(0)
{
    setType(HKB_ATTACHMENT_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "AttachmentModifier_"+QString::number(refCount);
}

const QString hkbAttachmentModifier::getClassname(){
    return classname;
}

QString hkbAttachmentModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbAttachmentModifier::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    bool ok;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    QByteArray text;
    while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "variableBindingSet"){
            if (!getVariableBindingSet().readShdPtrReference(index, reader)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variableBindingSet' reference!\nObject Reference: "+ref);
            }
        }else if (text == "userData"){
            userData = reader.getElementValueAt(index).toULong(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'userData' data field!\nObject Reference: "+ref);
            }
        }else if (text == "name"){
            name = reader.getElementValueAt(index);
            if (name == ""){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'name' data field!\nObject Reference: "+ref);
            }
        }else if (text == "enable"){
            enable = toBool(reader.getElementValueAt(index), &ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'enable' data field!\nObject Reference: "+ref);
            }
        }else if (text == "sendToAttacherOnAttach"){
            index++;
            while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    sendToAttacherOnAttach.id = reader.getElementValueAt(index).toInt(&ok);
                    if (!ok){
                        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'id' data field!\nObject Reference: "+ref);
                    }
                }else if (text == "payload"){
                    if (!sendToAttacherOnAttach.payload.readShdPtrReference(index, reader)){
                        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'payload' reference!\nObject Reference: "+ref);
                    }
                    break;
                }
                index++;
            }
        }else if (text == "sendToAttacheeOnAttach"){
            index++;
            while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    sendToAttacheeOnAttach.id = reader.getElementValueAt(index).toInt(&ok);
                    if (!ok){
                        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'id' data field!\nObject Reference: "+ref);
                    }
                }else if (text == "payload"){
                    if (!sendToAttacheeOnAttach.payload.readShdPtrReference(index, reader)){
                        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'payload' reference!\nObject Reference: "+ref);
                    }
                    break;
                }
                index++;
            }
        }else if (text == "sendToAttacherOnDetach"){
            index++;
            while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    sendToAttacherOnDetach.id = reader.getElementValueAt(index).toInt(&ok);
                    if (!ok){
                        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'id' data field!\nObject Reference: "+ref);
                    }
                }else if (text == "payload"){
                    if (!sendToAttacherOnDetach.payload.readShdPtrReference(index, reader)){
                        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'payload' reference!\nObject Reference: "+ref);
                    }
                    break;
                }
                index++;
            }
        }else if (text == "sendToAttacheeOnDetach"){
            index++;
            while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    sendToAttacheeOnDetach.id = reader.getElementValueAt(index).toInt(&ok);
                    if (!ok){
                        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'id' data field!\nObject Reference: "+ref);
                    }
                }else if (text == "payload"){
                    if (!sendToAttacheeOnDetach.payload.readShdPtrReference(index, reader)){
                        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'payload' reference!\nObject Reference: "+ref);
                    }
                    break;
                }
                index++;
            }
        }else if (text == "attachmentSetup"){
            if (!attachmentSetup.readShdPtrReference(index, reader)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'attachmentSetup' reference!\nObject Reference: "+ref);
            }
        }else if (text == "attacherHandle"){
            if (!attacherHandle.readShdPtrReference(index, reader)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'attacherHandle' reference!\nObject Reference: "+ref);
            }
        }else if (text == "attacheeHandle"){
            if (!attacheeHandle.readShdPtrReference(index, reader)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'attacheeHandle' reference!\nObject Reference: "+ref);
            }
        }else if (text == "attacheeLayer"){
            attacheeLayer = reader.getElementValueAt(index).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'attacheeLayer' data field!\nObject Reference: "+ref);
            }
        }
        index++;
    }
    index--;
    return true;
}

bool hkbAttachmentModifier::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    auto writeref = [&](const HkxSharedPtr & shdptr, const QString & name){
        QString refString = "null";
        (shdptr.data()) ? refString = shdptr->getReferenceString() : NULL;
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), refString);
    };
    auto writechild = [&](const HkxSharedPtr & shdptr, const QString & datafield){
        if (shdptr.data() && !shdptr->write(writer))
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write '"+datafield+"'!!!");
    };
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writeref(getVariableBindingSet(), "variableBindingSet");
        writedatafield("userData", QString::number(userData));
        writedatafield("name", name);
        writedatafield("enable", getBoolAsString(enable));
        writedatafield("sendToAttacherOnAttach", "");
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(sendToAttacherOnAttach.id));
        writeref(sendToAttacherOnAttach.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("sendToAttacheeOnAttach", "");
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(sendToAttacheeOnAttach.id));
        writeref(sendToAttacheeOnAttach.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("sendToAttacherOnDetach", "");
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(sendToAttacherOnDetach.id));
        writeref(sendToAttacherOnDetach.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("sendToAttacheeOnDetach", "");
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(sendToAttacheeOnDetach.id));
        writeref(sendToAttacheeOnDetach.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writeref(attachmentSetup, "attachmentSetup");
        writeref(attacherHandle, "attacherHandle");
        writeref(attacheeHandle, "attacheeHandle");
        writedatafield("attacheeLayer", QString::number(attacheeLayer));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(sendToAttacherOnAttach.payload, "sendToAttacherOnAttach.payload");
        writechild(sendToAttacheeOnAttach.payload, "sendToAttacherOnAttach.payload");
        writechild(sendToAttacherOnDetach.payload, "sendToAttacherOnAttach.payload");
        writechild(sendToAttacheeOnDetach.payload, "sendToAttacherOnAttach.payload");
        /*writechild(attachmentSetup, "attachmentSetup");
        writechild(attacherHandle, "attacherHandle");
        writechild(attacheeHandle, "attacheeHandle");*/
    }
    return true;
}

bool hkbAttachmentModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (sendToAttacherOnAttach.id == eventindex || sendToAttacheeOnAttach.id == eventindex || sendToAttacherOnDetach.id == eventindex || sendToAttacheeOnDetach.id == eventindex ){
        return true;
    }
    return false;
}

void hkbAttachmentModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateind = [&](int & index){
        (index > eventindex) ? index-- : NULL;
    };
    updateind(sendToAttacherOnAttach.id);
    updateind(sendToAttacheeOnAttach.id);
    updateind(sendToAttacherOnDetach.id);
    updateind(sendToAttacheeOnDetach.id);
}

void hkbAttachmentModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto mergeind = [&](int & index){
        (index == oldindex) ? index = newindex : NULL;
    };
    mergeind(sendToAttacherOnAttach.id);
    mergeind(sendToAttacheeOnAttach.id);
    mergeind(sendToAttacherOnDetach.id);
    mergeind(sendToAttacheeOnDetach.id);
}

void hkbAttachmentModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
    std::lock_guard <std::mutex> guard(mutex);
    hkbBehaviorGraphData *recdata;
    hkbBehaviorGraphData *domdata;
    QString thiseventname;
    int eventindex;
    if (!getIsMerged() && dominantfile){
        //TO DO: Support character properties...
        recdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(getParentFile())->getBehaviorGraphData());
        domdata = static_cast<hkbBehaviorGraphData *>(dominantfile->getBehaviorGraphData());
        if (recdata && domdata){
            auto fixIndex = [&](int & id){ if (id < 0){return;}
                thiseventname = recdata->getEventNameAt(id);
                eventindex = domdata->getIndexOfEvent(thiseventname);
                if (eventindex == -1 && thiseventname != ""){
                    domdata->addEvent(thiseventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                }
                id = eventindex;
            };
            fixIndex(sendToAttacherOnAttach.id);
            fixIndex(sendToAttacheeOnAttach.id);
            fixIndex(sendToAttacherOnDetach.id);
            fixIndex(sendToAttacheeOnDetach.id);
            setIsMerged(true);
        }
    }
}

void hkbAttachmentModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateref = [&](HkxSharedPtr & shdptr){
        (shdptr.data()) ? shdptr->updateReferences(++ref) : NULL;
    };
    setReference(ref);
    setBindingReference(++ref);
    updateref(sendToAttacherOnAttach.payload);
    updateref(sendToAttacheeOnAttach.payload);
    updateref(sendToAttacherOnDetach.payload);
    updateref(sendToAttacheeOnDetach.payload);
}

QVector<HkxObject *> hkbAttachmentModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    auto getchildren = [&](const HkxSharedPtr & shdptr){
        (shdptr.data()) ? list.append(shdptr.data()) : NULL;
    };
    getchildren(sendToAttacherOnAttach.payload);
    getchildren(sendToAttacheeOnAttach.payload);
    getchildren(sendToAttacherOnDetach.payload);
    getchildren(sendToAttacheeOnDetach.payload);
    return list;
}

bool hkbAttachmentModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto linkpayloads = [&](HkxSharedPtr & data, const QString & fieldname){
        if (ptr){
            if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object '"+fieldname+"' is not a HKB_STRING_EVENT_PAYLOAD!");
                setDataValidity(false);
            }
            data = *ptr;
        }
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(sendToAttacherOnAttach.payload.getShdPtrReference());
    linkpayloads(sendToAttacherOnAttach.payload, "sendToAttacherOnAttach.payload");
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(sendToAttacheeOnAttach.payload.getShdPtrReference());
    linkpayloads(sendToAttacheeOnAttach.payload, "sendToAttacherOnAttach.payload");
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(sendToAttacherOnDetach.payload.getShdPtrReference());
    linkpayloads(sendToAttacherOnDetach.payload, "sendToAttacherOnAttach.payload");
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(sendToAttacheeOnDetach.payload.getShdPtrReference());
    linkpayloads(sendToAttacheeOnDetach.payload, "sendToAttacherOnAttach.payload");
    return true;
}

void hkbAttachmentModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    sendToAttacherOnAttach.payload = HkxSharedPtr();
    sendToAttacheeOnAttach.payload = HkxSharedPtr();
    sendToAttacherOnDetach.payload = HkxSharedPtr();
    sendToAttacheeOnDetach.payload = HkxSharedPtr();
    attachmentSetup = HkxSharedPtr();
    attacherHandle = HkxSharedPtr();
    attacheeHandle = HkxSharedPtr();
}

QString hkbAttachmentModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto checkevents = [&](int & id, HkxSharedPtr & payload, const QString & fieldname){
        if (id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": "+fieldname+" event id out of range! Setting to max index in range!");
            id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
        }
        if (payload.data() && payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid "+fieldname+" type! Signature: "+QString::number(payload->getSignature(), 16)+" Setting null value!");
            payload = HkxSharedPtr();
        }
    };
    auto temp = HkDynamicObject::evaluateDataValidity();
    (temp != "") ? errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!\n"): NULL;
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    checkevents(sendToAttacherOnAttach.id, sendToAttacherOnAttach.payload, "sendToAttacherOnAttach");
    checkevents(sendToAttacheeOnAttach.id, sendToAttacheeOnAttach.payload, "sendToAttacheeOnAttach");
    checkevents(sendToAttacherOnDetach.id, sendToAttacherOnDetach.payload, "sendToAttacherOnDetach");
    checkevents(sendToAttacheeOnDetach.id, sendToAttacheeOnDetach.payload, "sendToAttacheeOnDetach");
    setDataValidity(isvalid);
    return errors;
}

hkbAttachmentModifier::~hkbAttachmentModifier(){
    refCount--;
}
