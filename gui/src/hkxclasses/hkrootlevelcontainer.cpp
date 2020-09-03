#include "hkrootlevelcontainer.h"
#include "src/xml/hkxxmlreader.h"
#include "src/xml/hkxxmlwriter.h"
#include "src/filetypes/behaviorfile.h"
#include "src/filetypes/projectfile.h"
#include "src/filetypes/characterfile.h"
#include "src/filetypes/skeletonfile.h"

using namespace UI;

uint hkRootLevelContainer::refCount = 0;

const QString hkRootLevelContainer::classname = "hkRootLevelContainer";

hkRootLevelContainer::hkRootLevelContainer(HkxFile *parent, long ref)
    : HkxObject(parent, ref)
{
    setType(HK_ROOT_LEVEL_CONTAINER, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkRootLevelContainer::getClassname(){
    return classname;
}

bool hkRootLevelContainer::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    QString text;
    auto ok = true;
    auto numVariants = 0;
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        if (reader.getNthAttributeValueAt(index, 0) == "namedVariants"){
            numVariants = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (ok){
                for (auto j = 0; j < numVariants; j++, index++){
                    namedVariants.append(hkRootLevelContainerNamedVariant());
                    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                        text = reader.getNthAttributeValueAt(index, 0);
                        if (text == "name"){
                            namedVariants.last().name = reader.getElementValueAt(index);
                        }else if (text == "className"){
                            namedVariants.last().className = reader.getElementValueAt(index);
                        }else if (text == "variant"){
                            if (namedVariants.last().variant.readShdPtrReference(index, reader)){
                                break;
                            }else{
                                return false;
                            }
                        }
                    }
                }
                (numVariants > 0) ? index-- : NULL;
            }else{
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nAttempt to read the number of variants failed!");
                return false;
            }
        }
    }
    index--;
    return true;
}

bool hkRootLevelContainer::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto result = true;
    if (writer && !getIsWritten()){
        QString refString;
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"namedVariants", QString::number(namedVariants.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < namedVariants.size(); i++){
            writer->writeLine(writer->object, true);
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("name"), namedVariants.at(i).name);
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("className"), namedVariants.at(i).className);
            if (namedVariants.at(i).variant.data()){
                refString = namedVariants.at(i).variant->getReferenceString();
            }else{
                refString = "null";
            }
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("variant"), refString);
            writer->writeLine(writer->object, false);
        }
        if (namedVariants.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        for (auto i = 0; i < namedVariants.size(); i++){
            if (namedVariants.at(i).variant.data() && !namedVariants.at(i).variant->write(writer)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write variant at "+QString::number(i));
                result = false;
            }
        }
    }
    return result;
}

void hkRootLevelContainer::addVariant(const QString &name, HkxObject *ptr){
    std::lock_guard <std::mutex> guard(mutex);
    namedVariants.append(hkRootLevelContainerNamedVariant(name, name, ptr));
}

void hkRootLevelContainer::setVariantAt(int index, HkxObject * ptr){
    std::lock_guard <std::mutex> guard(mutex);
    if (index > -1 && index < namedVariants.size()){
        namedVariants[index].variant = HkxSharedPtr(ptr);
    }
}

bool hkRootLevelContainer::link(){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < namedVariants.size(); i++){//This is awful, I know. I'll sort it out later...
        HkxSharedPtr *ptr = nullptr;
        HkxFile *file = dynamic_cast<BehaviorFile *>(getParentFile());
        if (file){
            ptr = static_cast<BehaviorFile *>(getParentFile())->findBehaviorGraph(namedVariants.at(i).variant.getShdPtrReference());
        }else{
            file = dynamic_cast<ProjectFile *>(getParentFile());
            if (file){
                ptr = static_cast<ProjectFile *>(getParentFile())->findProjectData(namedVariants.at(i).variant.getShdPtrReference());
            }else{
                file = dynamic_cast<CharacterFile *>(getParentFile());
                if (file){
                    ptr = static_cast<CharacterFile *>(getParentFile())->findCharacterData(namedVariants.at(i).variant.getShdPtrReference());
                }else{
                    file = dynamic_cast<SkeletonFile *>(getParentFile());
                    if (file){
                        ptr = static_cast<SkeletonFile *>(getParentFile())->findSkeleton(namedVariants.at(i).variant.getShdPtrReference());
                    }else{
                        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nParent file type is invalid!!!");
                    }
                }
            }
        }
        if (!ptr || !ptr->data()){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nUnable to link variant reference "+QString::number(namedVariants.at(i).variant.getShdPtrReference())+"!");
            setDataValidity(false);
        }else{
            namedVariants[i].variant = *ptr;
        }
    }
    return true;
}

void hkRootLevelContainer::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < namedVariants.size(); i++){
        namedVariants[i].variant = HkxSharedPtr();
    }
}

QString hkRootLevelContainer::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < namedVariants.size(); i++){
        if (!namedVariants.at(i).variant.data()){
            setDataValidity(false);
            return QString();
        }
    }
    setDataValidity(true);
    return QString();
}

hkRootLevelContainer::~hkRootLevelContainer(){
    refCount--;
}

bool hkRootLevelContainer::hkRootLevelContainerNamedVariant::operator==(const hkRootLevelContainerNamedVariant & other){
    if (name == other.name && className == other.className){
        return true;
    }
    return false;
}

hkRootLevelContainer::hkRootLevelContainerNamedVariant::hkRootLevelContainerNamedVariant(const QString & varname, const QString & classname, HkxObject *ptr)
    : name(varname),
      className(classname),
      variant(ptr)
{
    //
}
