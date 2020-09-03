#include "dataiconmanager.h"
#include "treegraphicsitem.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/generators/hkbgenerator.h"
#include "src/hkxclasses/behavior/generators/hkbclipgenerator.h"
#include "src/hkxclasses/behavior/modifiers/hkbmodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "behaviorgraphview.h"

#include <QGraphicsScene>

using namespace UI;

QString DataIconManager::getName() const{
    return "";
}

bool DataIconManager::hasChildren() const{
    return false;
}

bool DataIconManager::hasIcons() const{
    if (!icons.isEmpty()){
        if (icons.size() > 1){
            return true;
        }
    }else{
        LogFile::writeToLog("DataIconManager::hasIcons(): 'icons' is empty!!!");
    }
    return false;
}

int DataIconManager::getNumberOfIcons() const{
    return icons.size();
}

TreeGraphicsItem *DataIconManager::getFirstIcon() const{
    if (!icons.isEmpty()){
        return icons.first();
    }
    return nullptr;
}

void DataIconManager::updateIconNames(){
    for (auto i = 0; i < icons.size(); i++){
        icons.at(i)->update();
    }
}

bool DataIconManager::hasSameSignatureAndName(const DataIconManager *other) const{
    if (!other || getSignature() != other->getSignature() || getName() != other->getName()){
        return false;
    }
    return true;
}

QVector<DataIconManager *> DataIconManager::getChildren() const{
    return QVector<DataIconManager *> ();
}

int DataIconManager::getIndexOfObj(DataIconManager *) const{
    return -1;
}

bool DataIconManager::wrapObjectAt(int index, DataIconManager *obj, DataIconManager *parentObj){
    if (obj && parentObj){
        if (parentObj->insertObjectAt(parentObj->getIndexOfObj(this), obj) && obj->insertObjectAt(index, this)){
            return true;
        }
    }
    return false;
}

bool DataIconManager::insertObjectAt(int, DataIconManager *){
    return false;
}

bool DataIconManager::removeObjectAt(int ){
    return false;
}

DataIconManager::DataIconManager(HkxFile *parent, long ref)
    : HkDynamicObject(parent, ref)
{
    //
}

bool DataIconManager::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    if (getType() == TYPE_GENERATOR){
        injectWhileMerging(((hkbGenerator *)recessiveObject));
    }else if (getType() == TYPE_MODIFIER){
        injectWhileMerging(((hkbModifier *)recessiveObject));
    }
    return true;
}

void DataIconManager::setIconValidity(bool valid){
    auto setoutlinecolor = [this](Qt::GlobalColor color){
        for (auto i = 0; i < icons.size(); i++){
            icons.at(i)->setPenColor(color);
            icons.at(i)->update(/*QRectF(icons.at(i)->pos(), QSizeF(icons.at(i)->boundingRect().size()))*/);
        }
    };
    (valid) ? setoutlinecolor(Qt::black) : setoutlinecolor(Qt::red);
}

void DataIconManager::setFocusOnTopIcon(){
    if (!icons.isEmpty()){
        auto icon = icons.first();
        if (icon && icon->scene() && !icon->scene()->views().isEmpty()){
            icon = icons.first();
            static_cast<BehaviorGraphView *>(icon->scene()->views().first())->centerOn(icon);
            static_cast<BehaviorGraphView *>(icon->scene()->views().first())->setSelectedItem(icon);
        }else{
            LogFile::writeToLog(getName()+": icons is missing scene or view!!");
        }
    }else{
        LogFile::writeToLog(getName()+": icons is empty!!");
    }
}

bool DataIconManager::isCircularLoop() const{
    if (!icons.isEmpty()){
        for (auto i = 0; i < icons.size(); i++){
            if (icons.at(i)->isCircular(icons.at(i))){
                return true;
            }
        }
    }else{
        LogFile::writeToLog(getName()+": icons is empty!!");
    }
    return false;
}

void DataIconManager::injectWhileMerging(HkxObject *recessiveobj){
    if (!getIsMerged() && recessiveobj){
        auto recobj = static_cast<DataIconManager *>(recessiveobj);
        auto domchildren = getChildren();
        auto recchildren = recobj->getChildren();
        DataIconManager *domchild;
        DataIconManager *recchild;
        HkxSignature domsig;
        HkxSignature recsig;
        bool found;
        QVector <DataIconManager *> children;
        auto domvarbind = getVariableBindingSetData();
        auto recvarbind = recobj->getVariableBindingSetData();
        if (domvarbind){
            domvarbind->merge(recvarbind);
        }else if (recvarbind){
            getVariableBindingSet() = HkxSharedPtr(recvarbind);
            auto parfile = static_cast<BehaviorFile *>(getParentFile());
            recobj->fixMergedIndices(parfile);
            parfile->addObjectToFile(recvarbind, -1);
        }
        for (auto i = 0; i < domchildren.size(); i++){
            found = false;
            domsig = domchildren.at(i)->getSignature();
            domchild = domchildren.at(i);
            for (auto j = 0; j < recchildren.size(); j++){
                recsig = recchildren.at(j)->getSignature();
                recchild = recchildren.at(j);
                if (domsig == recsig && domchild->getName() == recchild->getName()){
                    found = true;
                    break;
                }
            }
            if (!found){
                for (auto j = recchildren.size() - 1; j >= 0; j--){
                    recchild = recchildren.at(j);
                    auto tempchildren = recchild->getChildren();
                    for (auto k = 0; k < tempchildren.size(); k++){
                        auto tempsig = tempchildren.at(k)->getSignature();
                        auto tempchild = tempchildren.at(k);
                        if ((domsig == tempsig) && ((domchild->getName() == tempchild->getName())/* || //For FNIS problem in mt_behavior NPC_TurnLeft90
                                                  (domsig == HKB_CLIP_GENERATOR && !QString::compare(static_cast<hkbClipGenerator *>(domchild)->getAnimationName().section("\\", -1, -1),
                                                                                                     static_cast<hkbClipGenerator *>(tempchild)->getAnimationName().section("\\", -1, -1), Qt::CaseInsensitive))*/))
                        {
                            insertObjectAt(i, recchild);
                            if (!static_cast<BehaviorFile *>(getParentFile())->existsInBehavior(recchild)){
                                recchild->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
                                recchild->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
                                getParentFile()->addObjectToFile(recchild, -1);
                                getParentFile()->addObjectToFile(recchild->getVariableBindingSetData(), -1);
                            }
                            //tempchildren.removeAt(k);
                            for (auto m = 0; m < tempchildren.size(); m++){
                                tempchild = tempchildren.at(m);
                                if (!static_cast<BehaviorFile *>(getParentFile())->existsInBehavior(tempchild)){
                                    tempchild->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
                                    tempchild->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
                                    getParentFile()->addObjectToFile(tempchild, -1);
                                    getParentFile()->addObjectToFile(tempchild->getVariableBindingSetData(), -1);
                                    auto objects = static_cast<DataIconManager *>(tempchild)->getChildren();
                                    while (!objects.isEmpty()){
                                        auto obj = objects.last();
                                        if (!static_cast<BehaviorFile *>(getParentFile())->existsInBehavior(obj)){
                                            obj->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
                                            obj->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
                                            getParentFile()->addObjectToFile(obj, -1);
                                            getParentFile()->addObjectToFile(obj->getVariableBindingSetData(), -1);
                                            children = obj->getChildren();
                                        }
                                        objects.removeLast();
                                        objects = objects + children;
                                        children.clear();
                                    }
                                }
                            }
                            k = tempchildren.size();
                            j = -1;
                        }
                    }
                }
            }
        }
        setIsMerged(true);
    }else{
        LogFile::writeToLog("DataIconManager::injectWhileMerging() hkbGenerator: nullptr!");
    }
}

TreeGraphicsItem * DataIconManager::reconnectToNext(){
    if (!icons.isEmpty()){
        if (icons.size() > 1){
            auto iconToBeRemoved = icons.at(1);
            if (iconToBeRemoved){
                if (iconToBeRemoved->parentItem() && icons.first()){
                    auto children = iconToBeRemoved->parentItem()->childItems();
                    icons.first()->setParent((TreeGraphicsItem *)iconToBeRemoved->parentItem(), children.indexOf(iconToBeRemoved));
                    return iconToBeRemoved;
                }
            }
        }
    }else{
        LogFile::writeToLog("DataIconManager::reconnectToNext(): 'icons' is empty!!!");
    }
    return nullptr;
}

void DataIconManager::appendIcon(TreeGraphicsItem *icon){
    if (icon){
        (icons.isEmpty() || !icons.contains(icon)) ? icons.append(icon) : LogFile::writeToLog("DataIconManager::appendIcon(): icon was not appended!!!");
    }
}

void DataIconManager::removeIcon(TreeGraphicsItem *icon){
    (!icons.isEmpty()) ? icons.removeAll(icon) : LogFile::writeToLog("DataIconManager::removeIcon(): 'icons' is empty!!!");
}
