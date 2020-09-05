#include "skeletonfile.h"
#include "src/xml/hkxxmlreader.h"
#include "src/xml/hkxxmlwriter.h"
#include "src/ui/mainwindow.h"
#include "src/hkxclasses/hksimplelocalframe.h"
#include "src/hkxclasses/animation/hkaskeleton.h"
#include "src/hkxclasses/animation/hkaanimationcontainer.h"
#include "src/hkxclasses/hkrootlevelcontainer.h"

using namespace UI;

SkeletonFile::SkeletonFile(MainWindow *window, const QString & name)
    : HkxFile(window, name),
      largestRef(0)
{
    getReader().setFile(this);
}

HkxSharedPtr * SkeletonFile::findSkeleton(long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < skeletons.size(); i++){
        if (skeletons.at(i).data() && skeletons.at(i).getShdPtrReference() == ref){
            return &skeletons[i];
        }
    }
    return nullptr;
}

HkxSharedPtr * SkeletonFile::findLocalFrame(long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < localFrames.size(); i++){
        if (localFrames.at(i).data() && localFrames.at(i).getShdPtrReference() == ref){
            return &localFrames[i];
        }
    }
    return nullptr;
}

QStringList SkeletonFile::getBonesFromSkeletonAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (index < skeletons.size() && index >= 0){
        if (skeletons.at(index).data()){
            return static_cast<hkaSkeleton *>(skeletons.at(index).data())->getBoneNames();
        }
    }
    return QStringList();
}

int SkeletonFile::getNumberOfBones(bool ragdoll) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (!skeletons.isEmpty()){
        if (!ragdoll){
            if (skeletons.first().data()){
                return static_cast<hkaSkeleton *>(skeletons.first().data())->bones.size();
            }
        }else{
            if (skeletons.size() > 1 && skeletons.at(1).data()){
                return static_cast<hkaSkeleton *>(skeletons.at(1).data())->bones.size();
            }
        }
    }
    return -1;
}

QStringList SkeletonFile::getLocalFrameNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList names;
    for (auto i = 0; i < localFrames.size(); i++){
        if (localFrames.at(i).data()){
            names.append(static_cast<hkSimpleLocalFrame *>(localFrames.at(i).data())->getName());
        }
    }
    return names;
}

hkaSkeleton *SkeletonFile::getSkeleton(bool isragdoll) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (!skeletons.isEmpty()){
        if (!isragdoll){
            if (skeletons.first().data()){
                return static_cast<hkaSkeleton *>(skeletons.first().data());
            }
        }else{
            if (skeletons.size() > 1 && skeletons.at(1).data()){
                return static_cast<hkaSkeleton *>(skeletons.at(1).data());
            }
        }
    }
    return nullptr;
}

bool SkeletonFile::addObjectToFile(HkxObject *obj, long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        (ref > largestRef) ? largestRef = ref : largestRef++;
        obj->setReference(largestRef);
        if (obj->getSignature() == HKA_SKELETON){
            skeletons.append(HkxSharedPtr(obj, ref));
        }else if (obj->getSignature() == HK_SIMPLE_LOCAL_FRAME){
            localFrames.append(HkxSharedPtr(obj, ref));
        }else if (obj->getSignature() == HK_ROOT_LEVEL_CONTAINER){
            setRootObject(HkxSharedPtr(obj, ref));
        }else{
            LogFile::writeToLog("SkeletonFile: addObjectToFile() failed!\nInvalid type enum for this object!\nObject signature is: "+QString::number(obj->getSignature(), 16));
            return false;
        }
        return true;
    }
    return false;
}

bool SkeletonFile::parseBinary() {
	return false;
}

bool SkeletonFile::parse(){
    //std::lock_guard <std::mutex> guard(mutex);
    long index = 2;
    auto ok = false;
    HkxSignature signature;
    QByteArray value;
    auto ref = 0;
    auto appendnread = [&](HkxObject *obj, const QString & nameoftype){
        (!appendAndReadData(index, obj)) ? LogFile::writeToLog("BehaviorFile: parse(): Failed to read a "+nameoftype+" object! Ref: "+QString::number(ref)) : NULL;
    };
    if (getReader().parse()){
        for (; index < getReader().getNumElements(); index++){
            value = getReader().getNthAttributeNameAt(index, 1);
            if (value == "class"){
                value = getReader().getNthAttributeValueAt(index, 2);
                if (value != ""){
                    ref = getReader().getNthAttributeValueAt(index, 0).remove(0, 1).toLong(&ok);
                    (!ok) ? LogFile::writeToLog("BehaviorFile: parse() failed! The object reference string contained invalid characters and failed to convert to an integer!") : NULL;
                    signature = (HkxSignature)value.toULongLong(&ok, 16);
                    (!ok) ? LogFile::writeToLog("BehaviorFile: parse() failed! The object signature string contained invalid characters and failed to convert to an integer!") : NULL;
                    switch (signature){
                    case HK_SIMPLE_LOCAL_FRAME:
                        appendnread(new hkSimpleLocalFrame(this, "", ref), "HK_SIMPLE_LOCAL_FRAME"); break;
                    case HKA_SKELETON:
                        appendnread(new hkaSkeleton(this, ref), "HKA_SKELETON"); break;
                    case HKA_ANIMATION_CONTAINER:
                        appendnread(new hkaAnimationContainer(this, ref), "HKA_ANIMATION_CONTAINER"); break;
                    case HK_ROOT_LEVEL_CONTAINER:
                        appendnread(new UI::hkRootLevelContainer(this, ref), "HK_ROOT_LEVEL_CONTAINER"); break;
                    default:
                        LogFile::writeToLog(fileName()+": Unknown signature detected! Unknown object class name is: "+getReader().getNthAttributeValueAt(index, 1)+" Unknown object signature is: "+QString::number(signature, 16));
                    }
                }
            }
        }
        closeFile();
        getReader().clear();
        (link()) ? ok = true : LogFile::writeToLog(fileName()+": failed to link!!!");
    }else{
        LogFile::writeToLog(fileName()+": failed to parse!!!");
    }
    return ok;
}

bool SkeletonFile::link(){
    //std::lock_guard <std::mutex> guard(mutex);
    if (!getRootObject().constData()){
        LogFile::writeToLog("SkeletonFile: link() failed!\nThe root object of this character file is nullptr!");
        return false;
    }else if (getRootObject()->getSignature() != HK_ROOT_LEVEL_CONTAINER){
        LogFile::writeToLog("SkeletonFile: link() failed!\nThe root object of this character file is NOT a hkRootLevelContainer!\nThe root object signature is: "+QString::number(getRootObject()->getSignature(), 16));
        return false;
    }
    if (!getRootObject()->link()){
        LogFile::writeToLog("SkeletonFile: link() failed!\nThe root object of this character file failed to link to it's children!");
        return false;
    }
    return true;
}


void SkeletonFile::write(){
    /*//std::lock_guard <std::mutex> guard(mutex);
    ulong ref = 100;
    getRootObject()->setReference(ref);
    ref++;
    getWriter().setFile(this);
    getWriter().writeToXMLFile();*/
}
