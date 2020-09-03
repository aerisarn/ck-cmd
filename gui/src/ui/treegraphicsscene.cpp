#include "treegraphicsscene.h"
#include "treegraphicsitem.h"
#include "dataiconmanager.h"
#include "src/ui/hkxclassesui/behaviorui/behaviorgraphicon.h"
#include "src/ui/behaviorgraphview.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QScrollBar>

using namespace UI;

#define MAX_NUM_GRAPH_ICONS 10000

TreeGraphicsScene::TreeGraphicsScene()
    : rootIcon(nullptr),
      selectedIcon(nullptr),
      canDeleteRoot(false)
{
    setBackgroundBrush(QBrush(Qt::gray));
    setItemIndexMethod(QGraphicsScene::NoIndex);
}

void TreeGraphicsScene::setCanDeleteRoot(bool value){
    canDeleteRoot = value;
}

void TreeGraphicsScene::selectIcon(TreeGraphicsItem *icon, BranchBehaviorEnum expand){    //Change so expand all sub branches on right click on expand box!!!!!!
    (selectedIcon) ? selectedIcon->unselect() : NULL;
    selectedIcon = icon;
    if (selectedIcon){
        if (expand > EXPAND_CONTRACT_ZERO){
            if (selectedIcon->getIsExpanded()){
                (expand == EXPAND_CONTRACT_ALL) ? contractBranch(selectedIcon, true) : contractBranch(selectedIcon);
                selectedIcon->setIsExpanded(false);
            }else{
                (expand == EXPAND_CONTRACT_ONE) ? expandBranch(selectedIcon) : expandBranch(selectedIcon, true);
            }
            if (!selectedIcon->isPrimaryIcon()){
                selectedIcon = selectedIcon->getPrimaryIcon();
                if (!selectedIcon->isVisible()){
                    auto branch = selectedIcon->getAllIconsInBranch();
                    for (auto i = 0; i < branch.size(); i++){
                        expandBranch(branch.at(i));
                    }
                }
                auto behaviorviews = views();
                (!behaviorviews.isEmpty()) ? static_cast<BehaviorGraphView *>(views().first())->refocus() : LogFile::writeToLog("TreeGraphicsScene: No views!!!");//May need to expand the branch???
            }
            selectedIcon->reposition();
        }
        selectedIcon->setIconSelected();
        emit iconSelected(selectedIcon);
    }
}

void TreeGraphicsScene::contractBranch(TreeGraphicsItem *icon, bool contractAll){
    if (icon){
        auto children = icon->childItems();
        for (auto i = 0; i < children.size(); i++){
            auto child = static_cast<TreeGraphicsItem *>(children.at(i));
            child->setVisible(false);
            child->path->setVisible(false);
            (contractAll) ? child->setIsExpanded(false), contractBranch(child, true) : contractBranch(child);
        }
    }
}

void TreeGraphicsScene::expandBranch(TreeGraphicsItem *icon, bool expandAll){
    if (icon){
        auto children = icon->childItems();
        icon->setIsExpanded(true);
        for (auto i = 0; i < children.size(); i++){
            auto child = static_cast<TreeGraphicsItem *>(children.at(i));
            child->setVisible(true);
            child->path->setVisible(true);
            if (expandAll){
                expandBranch(child, true);
            }else if (child->getIsExpanded()){
                expandBranch(child);
            }
        }
    }
}

bool TreeGraphicsScene::drawGraph(DataIconManager *rootData, bool allowDuplicates){
    QVector <TreeGraphicsItem *> parentIcons;
    QVector <short> numChildren;
    if (rootData){
        auto children = rootData->getChildren();
        rootIcon = new BehaviorGraphIcon(nullptr, rootData);
        addItem(rootIcon);
        auto objects = children;
        numChildren.append(children.size());
        parentIcons.append(rootIcon);
        while (!objects.isEmpty() && !numChildren.isEmpty() && !parentIcons.isEmpty()){
            auto newIcon = addItemToGraph(parentIcons.first(), objects.first(), 0, false, allowDuplicates, true);
            if (newIcon && newIcon->isDataDescendant(newIcon)){
                return false;
            }
            (objects.first()->hasIcons()) ? children.clear() : children = objects.first()->getChildren();
            numChildren.first()--;
            if (!numChildren.first()){
                numChildren.removeFirst();
                parentIcons.removeFirst();
            }
            objects.removeFirst();
            if (!children.isEmpty()){
                if (newIcon){
                    parentIcons.prepend(newIcon);
                    numChildren.prepend(children.size());
                    objects = children + objects;
                }
            }
        }
        auto view = views();
        if (!view.isEmpty()){
            auto thisview = view.first();
            auto rect = thisview->sceneRect();
            thisview->setSceneRect(rect.marginsAdded(QMarginsF(rect.width(), rect.height(), rect.width(), rect.height())));
            return true;
        }else{
            CRITICAL_ERROR_MESSAGE("TreeGraphicsScene::drawGraph: No view!!!");
        }
    }else{
        CRITICAL_ERROR_MESSAGE("TreeGraphicsScene::drawGraph: rootData is null!!!");
    }
    return false;
}

//Appends "data" to the 'itemData' of "selectedIcon" after creating a new icon representing "data" and appending it to "selectedIcon"...
TreeGraphicsItem * TreeGraphicsScene::addItemToGraph(TreeGraphicsItem *selectedIcon, DataIconManager *data, int indexToInsert, bool inject, bool allowDuplicates, bool isFirstDraw){
    TreeGraphicsItem *newIcon = nullptr;
    if (selectedIcon){
        auto parent = static_cast<TreeGraphicsItem *>(selectedIcon->parentItem());
        if (data){
            if (selectedIcon){
                if (!allowDuplicates){
                    auto children = selectedIcon->childItems();
                    for (auto i = 0; i < children.size(); i++){
                        auto itemdata = static_cast<TreeGraphicsItem *>(children.at(i))->getItemData();
                        if (itemdata == data){
                            itemdata = selectedIcon->getItemData();
                            if (!inject && !isFirstDraw){
                                itemdata->insertObjectAt(indexToInsert, data);
                            }else if (parent && inject){
                                itemdata->wrapObjectAt(indexToInsert, data, parent->getItemData());
                                //selectedIcon->setParent(newIcon, newIcon->getIndexofIconWithData(selectedIcon->itemData));//Not sure...
                            }
                            return nullptr;
                        }
                    }
                }
            }
            if (!selectedIcon->isPrimaryIcon()){
                if (selectedIcon->getPrimaryIcon()){
                    selectedIcon = selectedIcon->getPrimaryIcon();
                }else{
                    return nullptr;    //Error...
                }
            }
            if (!inject){
                if (isFirstDraw){
                    newIcon = new BehaviorGraphIcon(selectedIcon, data, selectedIcon->getIndexofIconWithData(data));
                }else{
                    newIcon = new BehaviorGraphIcon(selectedIcon, data, indexToInsert);
                    selectedIcon->getItemData()->insertObjectAt(indexToInsert, data);
                }
            }else if (parent){
                auto tempdata = selectedIcon->getItemData();
                newIcon = new BehaviorGraphIcon(parent, data, parent->getIndexofIconWithData(tempdata));
                tempdata->wrapObjectAt(indexToInsert, data, parent->getItemData());
                selectedIcon->setParent(newIcon, newIcon->getIndexofIconWithData(tempdata));
            }else{
                delete newIcon;
                return nullptr;
            }
            if (newIcon->isDataDescendant(newIcon)){
                delete newIcon;
                return nullptr;
            }
            (!newIcon->scene()) ? addItem(newIcon) : NULL;  //newIcon is added to scene already???
            //newIcon->reposition();
            (!newIcon->path->scene()) ? addItem(newIcon->path) : NULL;
        }
    }else{
        CRITICAL_ERROR_MESSAGE("TreeGraphicsScene::addItemToGraph(): selectedIcon is nullptr!!!");
    }
    return newIcon;
}

bool TreeGraphicsScene::reconnectIcon(TreeGraphicsItem *oldIconParent, DataIconManager *dataToReplace, int replaceindex, DataIconManager *replacementData, bool removeData){
    if (oldIconParent){
        if (!dataToReplace || (dataToReplace == replacementData && !oldIconParent->hasSameData(replacementData))){
            addItemToGraph(oldIconParent, replacementData, replaceindex);
            oldIconParent->reposition();
        }else if (dataToReplace != replacementData){
            auto iconToReplace = oldIconParent->getChildWithData(dataToReplace);
            if (replacementData == nullptr){
                removeItemFromGraph(iconToReplace, replaceindex, removeData);
            }else if ((!oldIconParent->isDataDescendant(replacementData) && !oldIconParent->hasSameData(replacementData))){
                auto replacementIcon = oldIconParent->getReplacementIcon(replacementData);
                (iconToReplace) ? removeItemFromGraph(iconToReplace, replaceindex, removeData, false, replacementData) : NULL;
                if (replacementIcon){
                    auto oldParent = replacementIcon->setParent(oldIconParent, replaceindex);
                    addItemToGraph(oldParent, replacementData, replaceindex);
                    oldIconParent->getItemData()->insertObjectAt(replaceindex, replacementData);
                }else{
                    addItemToGraph(oldIconParent, replacementData, replaceindex);
                }
                if (!oldIconParent->reorderChildren()){
                    return false;
                }
            }
        }
    }
    return true;
}

bool TreeGraphicsScene::removeItemFromGraph(TreeGraphicsItem *item, int indexToRemove, bool removeData, bool removeAllSameData, DataIconManager *dataToKeep){
    QList <QGraphicsItem *> children;   //Storage for all referenced icons in the branch whose root is "item"...
    QList <QGraphicsItem *> tempList;   //Storage for the children of the first icon stored in "children"...
    QList <QGraphicsItem *> iconsToRemove;  //Storage for all icons to be removed from the graph...
    TreeGraphicsItem *itemToDeleteParent = nullptr;
    TreeGraphicsItem *itemToDelete = nullptr;  //Represents any icons to be removed that had children that were adopted by another icon representing the same data...
    QVector <DataIconManager *> childrenData;//Used to count the data references if less than 2 remove the icon...
    int dataCount = 0;
    TreeGraphicsItem *iconChild = nullptr;
    int count = 0;  //Used to prevent possible infinite looping due to icons referencing ancestors...
    int index = -1; //Used to store the index of the position of "itemToDelete" in "children"...
    if (item){
        itemToDeleteParent = static_cast<TreeGraphicsItem *>(item->parentItem());
        if (itemToDeleteParent){
            childrenData = itemToDeleteParent->getItemData()->getChildren();
            if (!childrenData.isEmpty()){
                dataCount = childrenData.count(item->getItemData());
            }
            if (dataCount < 2 || removeAllSameData){
                children.append(item);
                for (; count < MAX_NUM_GRAPH_ICONS, !children.isEmpty(); count++){  //Start cycling through children...
                    itemToDelete = nullptr;
                    iconChild = static_cast<TreeGraphicsItem *>(children.first());
                    if (iconChild->hasSameData(dataToKeep)){
                        iconChild->setParent(itemToDeleteParent, itemToDeleteParent->getIndexofIconWithData(item->getItemData()));
                        itemToDelete = iconChild->reconnectToNextDuplicate();
                        iconsToRemove.append(itemToDelete);
                        tempList.clear();
                    }else{
                        tempList = iconChild->childItems();
                        if (!tempList.isEmpty() && iconChild->isPrimaryIcon() && iconChild->hasIcons()){  //"item" has children and has data that is referenced by other icons...
                            itemToDelete = iconChild->reconnectToNextDuplicate();   //Reconnect "item" to the parent of the next icon that references it's data...
                            index = children.indexOf(itemToDelete);
                            if (index != -1){//Problem here?
                                children.replace(index, iconChild); //"itemToDelete" is the
                            }
                            iconsToRemove.append(itemToDelete);
                            tempList.clear();
                        }else{
                            iconsToRemove.append(iconChild);
                        }
                    }
                    children.removeFirst();
                    children = tempList + children;
                }
                if (removeAllSameData){
                    for (auto i = childrenData.size() - 1; i >= 0; i--){
                        if (childrenData.at(i) == item->getItemData()){
                            itemToDeleteParent->getItemData()->removeObjectAt(i);
                        }
                    }
                }else if (removeData){
                    itemToDeleteParent->getItemData()->removeObjectAt(indexToRemove);
                }
                for (auto i = iconsToRemove.size() - 1; i >= 0; i--){//Duplicates sometimes...
                    delete iconsToRemove.at(i);
                }
                itemToDeleteParent->reposition();
            }else if (removeData){
                itemToDeleteParent->getItemData()->removeObjectAt(indexToRemove);
                if (!itemToDeleteParent->reorderChildren()){
                    return false;
                }
            }
        }else if (canDeleteRoot){  //Icon with no parent must be the root...
            delete item;
            rootIcon = nullptr;
        }
        if (count < MAX_NUM_GRAPH_ICONS){
            return true;
        }
    }
    return false;
}


