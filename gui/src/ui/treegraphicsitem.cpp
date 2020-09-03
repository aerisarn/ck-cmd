#include "treegraphicsitem.h"
#include "treegraphicsscene.h"
#include "dataiconmanager.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "src/utility.h"
#include "src/hkxclasses/hkxobject.h"

#define ITEM_WIDTH 500
#define ITEM_HEIGHT 50

#define MAX_NUM_GRAPH_ICONS 10000

using namespace UI;

TreeGraphicsItem::TreeGraphicsItem(TreeGraphicsItem *parent, DataIconManager *obj, int indexToInsert, Qt::GlobalColor color)
    : QGraphicsItem(parent),
      brushColor(color),
      penColor(Qt::black),
      itemData(obj),
      isExpanded(true),
      yCoordinate(0),
      path(new GraphicsPathItem),
      itemFont(QFont("Times", 16, QFont::Bold))
{
    if (!obj){
        CRITICAL_ERROR_MESSAGE("TreeGraphicsItem::TreeGraphicsItem(): Icon data is nullptr!!!");
    }
    setFlags(QGraphicsItem::ItemIsSelectable);
    auto paritem = parentItem();
    itemData->appendIcon(this);
    if (paritem){
        auto children = paritem->childItems();
        if (indexToInsert > -1 && indexToInsert < children.size()){
            for (auto i = 0; i < children.size(); i++){
                children[i]->setParentItem(nullptr);
            }
            auto index = children.indexOf(this);
            children.insert(indexToInsert, this);
            (index > indexToInsert) ? index++ : NULL;
            (index < children.size()) ? children.removeAt(index) : NULL;
            for (auto i = 0; i < children.size(); i++){
                children[i]->setParentItem(parent);
            }
        }
        setPosition(QPointF(boundingRect().width()*2, getYCoordinate()));
    }
}

TreeGraphicsItem::~TreeGraphicsItem(){
    (!itemData->hasIcons()) ? itemData->removeObjectAt(-1) : NULL;
    itemData->removeIcon(this);
    if (scene()){
        scene()->removeItem(this);
        /*if (path->scene()){
            scene()->removeItem(path);
        }*/
    }
    delete path;
    setParentItem(nullptr);
}

QRectF TreeGraphicsItem::boundingRect() const{
    return QRectF(0, 0, ITEM_WIDTH, ITEM_HEIGHT);
}

QRectF TreeGraphicsItem::branchExpandCollapseBox() const{
    return QRectF(boundingRect().width()*0.9, 0, boundingRect().width()*0.1, boundingRect().height());
}

void TreeGraphicsItem::setBrushColor(Qt::GlobalColor color){
    brushColor = color;
    (scene()) ? scene()->update(boundingRect()) : LogFile::writeToLog("TreeGraphicsItem: scene is null!!!");
}

void TreeGraphicsItem::setPenColor(Qt::GlobalColor color){
    penColor = color;
    (scene()) ? scene()->update(boundingRect()) : LogFile::writeToLog("TreeGraphicsItem: scene is null!!!");
}

void TreeGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *){
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(penColor);
    painter->setBrush(brushColor);
    painter->drawRect(boundingRect());
    painter->setFont(itemFont);
    painter->drawText(boundingRect(), itemData->getName());
    if (!childItems().isEmpty()){
        (!isExpanded) ? painter->setBrush(Qt::red) : painter->setBrush(Qt::blue);
    }else{
        painter->setBrush(Qt::blue);
    }
    painter->drawRect(branchExpandCollapseBox());
}

void TreeGraphicsItem::setIconSelected(){
    brushColor = Qt::green;
    (scene()) ? scene()->update(QRectF(scenePos(), scenePos() + QPointF(boundingRect().width(), boundingRect().height()))) : LogFile::writeToLog("TreeGraphicsItem: scene is null!!!");
}

void TreeGraphicsItem::unselect(){
    (itemData->evaluateDataValidity() != "") ? penColor = Qt::red : penColor = Qt::black;
    brushColor = Qt::gray;
    (scene()) ? scene()->update(QRectF(scenePos(), scenePos() + QPointF(boundingRect().width(), boundingRect().height()))) : LogFile::writeToLog("TreeGraphicsItem: scene is null!!!");
}

void TreeGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if (boundingRect().contains(event->pos())){
        auto thiscene = static_cast<TreeGraphicsScene *>(scene());
        if (branchExpandCollapseBox().contains(event->pos())){
            (event->modifiers() == Qt::ShiftModifier) ? thiscene->selectIcon(this, TreeGraphicsScene::EXPAND_CONTRACT_ALL) : thiscene->selectIcon(this, TreeGraphicsScene::EXPAND_CONTRACT_ONE);
        }else{
            thiscene->selectIcon(this, TreeGraphicsScene::EXPAND_CONTRACT_ZERO);
        }
    }
}

void TreeGraphicsItem::setPosition(const QPointF &pos){
    setPos(pos);
    setPathToParent();
}

qreal TreeGraphicsItem::getYCoordinate(){
    auto paritem = static_cast<TreeGraphicsItem *>(parentItem());
    if (paritem){
        yCoordinate = paritem->boundingRect().height()*2;
        auto children = paritem->childItems();
        while (!children.isEmpty()){
            if (children.first() != this){
                (children.first()->isVisible()) ? yCoordinate = yCoordinate + static_cast<TreeGraphicsItem *>(children.first())->boundingRect().height()*2 : NULL;
                auto tempList = children.first()->childItems();
                children.removeFirst();
                children = tempList + children;
            }else{
                return yCoordinate;
            }
        }
    }
    return 0;
}

bool TreeGraphicsItem::reorderChildren(){
    auto children = childItems();
    auto dataChildren = itemData->getChildren();
    for (auto j = 0; j < dataChildren.size(); j++){
        for (auto k = j + 1; k < dataChildren.size(); k++){
            (dataChildren.at(j) == dataChildren.at(k)) ? dataChildren.removeAt(k) : NULL;
        }
    }
    for (auto i = 0; i < children.size(); i++){
        children[i]->setParentItem(nullptr);
    }
    if (children.size() <= dataChildren.size()){    //Fix this...
        for (auto i = 0; i < dataChildren.size(); i++){
            for (auto k = 0; k < children.size(); k++){
                if (static_cast<TreeGraphicsItem *>(children.at(k))->getItemData() == dataChildren.at(i)){
                    children[k]->setParentItem(this);
                    break;
                }
            }
        }
        reposition();
        return true;
    }
    return false;
}

void TreeGraphicsItem::reposition(){
    auto children = childItems();
    QList <QGraphicsItem *> tempList;
    setPosition(QPointF(boundingRect().width()*2, getYCoordinate()));
    while (!children.isEmpty()){
        tempList.clear();
        if (children.first()->isVisible()){
            static_cast<TreeGraphicsItem *>(children.first())->setPosition(QPointF(boundingRect().width()*2, ((TreeGraphicsItem *)children.first())->getYCoordinate()));
            tempList = children.first()->childItems();
        }
        children.removeFirst();
        children = tempList + children;
    }
    auto parent = static_cast<TreeGraphicsItem *>(parentItem());
    auto child = this;
    while (parent){
        children = parent->childItems();
        while (!children.isEmpty() && children.first() != child){
            children.removeFirst();
        }
        while (!children.isEmpty()){
            tempList.clear();
            auto firstchild = static_cast<TreeGraphicsItem *>(children.first());
            if (firstchild->isVisible()){
                firstchild->setPosition(QPointF(boundingRect().width()*2, firstchild->getYCoordinate()));
                tempList = firstchild->childItems();
            }
            children.removeFirst();
            children = tempList + children;
        }
        child = parent;
        parent = static_cast<TreeGraphicsItem *>(parent->parentItem());
    }
    (scene()) ? scene()->update() : LogFile::writeToLog("TreeGraphicsItem: scene is null!!!");
}

bool TreeGraphicsItem::isDataDescendant(TreeGraphicsItem *icon) const{
    if (icon){
        auto parent = static_cast<TreeGraphicsItem *>(parentItem());
        if (parent){
            if (parent->hasSameData(icon)){
                return true;
            }else{
                if (parent->isDataDescendant(icon)){
                    return true;
                }
            }
        }
    }
    return false;
}

bool TreeGraphicsItem::isDataDescendant(DataIconManager *data) const{
    if (data){
        auto parent = static_cast<TreeGraphicsItem *>(parentItem());
        if (parent){
            if (parent->hasSameData(data)){
                return true;
            }else{
                if (parent->isDataDescendant(data)){
                    return true;
                }
            }
        }
    }
    return false;
}

bool TreeGraphicsItem::isPrimaryIcon() const{
    if (itemData){
        if (!itemData->icons.isEmpty()){
            if (itemData->icons.first() == this || itemData->icons.size() < 2){
                return true;
            }
        }
    }else{
        LogFile::writeToLog("TreeGraphicsItem: itemdata is null!!");
    }
    return false;
}

TreeGraphicsItem *TreeGraphicsItem::getPrimaryIcon() const{
    if (itemData){
        if (!itemData->icons.isEmpty()){
            return itemData->icons.first();
        }
    }else{
        LogFile::writeToLog("TreeGraphicsItem: itemdata is null!!");
    }
    return nullptr;
}

TreeGraphicsItem * TreeGraphicsItem::setParent(TreeGraphicsItem *newParent, int indexToInsert){//Swap paths!!!
    auto oldParent = static_cast<TreeGraphicsItem *>(parentItem());
    if (oldParent){
        auto children = oldParent->childItems();
        for (auto i = 0; i < children.size(); i++){
            children[i]->setParentItem(nullptr);
        }
        if (children.removeAll(this) != 1){
            CRITICAL_ERROR_MESSAGE("TreeGraphicsItem::setParent(): Error!!!");
        }
        for (auto i = 0; i < children.size(); i++){
            children[i]->setParentItem(oldParent);
        }
        children.clear();
        if (newParent){
            children = newParent->childItems();
            auto index = newParent->getIndexofIconWithData(itemData);
            if (indexToInsert > -1 && indexToInsert < children.size()){
                for (auto i = 0; i < children.size(); i++){
                    children[i]->setParentItem(nullptr);
                }
                (index < children.size()) ? children.removeAt(index) : NULL;
                children.insert(indexToInsert, this);
                for (auto i = 0; i < children.size(); i++){
                    children[i]->setParentItem(newParent);
                }
            }else{
                for (auto i = 0; i < children.size(); i++){
                    children[i]->setParentItem(newParent);
                }
                setParentItem(newParent);
            }
        }else{
            setParentItem(nullptr);
        }
        reposition();
    }else{
        CRITICAL_ERROR_MESSAGE("TreeGraphicsItem::setParent(): Error!!!");
    }
    return oldParent;
}

TreeGraphicsItem * TreeGraphicsItem::reconnectToNextDuplicate(){
    if (itemData){
        return itemData->reconnectToNext();
    }else{
        LogFile::writeToLog("TreeGraphicsItem: itemdata is null!!");
    }
    return nullptr;
}

bool TreeGraphicsItem::hasIcons() const{
    if (itemData){
        return itemData->hasIcons();
    }else{
        LogFile::writeToLog("TreeGraphicsItem: itemdata is null!!");
    }
    return false;
}

TreeGraphicsItem *TreeGraphicsItem::getNextChildAfter(TreeGraphicsItem *child){
    auto children = childItems();
    auto index = children.indexOf(child) + 1;
    if (index >= 0 && index < children.size()){
        return static_cast<TreeGraphicsItem *>(children.at(index));
    }
    return nullptr;
}

bool TreeGraphicsItem::hasSameData(TreeGraphicsItem *icon) const{
    if (itemData){
        if (itemData == icon->itemData){
            return true;
        }
    }else{
        LogFile::writeToLog("TreeGraphicsItem: itemdata is null!!");
    }
    return false;
}

bool TreeGraphicsItem::hasSameData(DataIconManager *data) const{
    if (data && itemData == data){
        return true;
    }
    return false;
}

int TreeGraphicsItem::getIconIndex(){
    if (itemData){
        return itemData->icons.indexOf(this);
    }else{
        LogFile::writeToLog("TreeGraphicsItem: itemdata is null!!");
    }
    return -1;
}

TreeGraphicsItem *TreeGraphicsItem::getChildWithData(DataIconManager *data){
    auto children = childItems();
    for (auto i = 0; i < children.size(); i++){
        auto tempchild = static_cast<TreeGraphicsItem *>(children.at(i));
        if (tempchild->getItemData() == data){
            return tempchild;
        }
    }
    return nullptr;
}

bool TreeGraphicsItem::isCircular(TreeGraphicsItem* itemtocheck) const{
    auto parent = static_cast<TreeGraphicsItem *>(parentItem());
    if (!parent || !itemtocheck){
        return false;
    }else if (parent == itemtocheck){
        return true;
    }else{
        return parent->isCircular(itemtocheck);
    }
}

TreeGraphicsItem *TreeGraphicsItem::getReplacementIcon(DataIconManager *data){
    auto parent = static_cast<TreeGraphicsScene *>(scene())->rootIcon;
    if (parent){
        auto children = parent->childItems();
        while (!children.isEmpty()){
            auto firstchild = static_cast<TreeGraphicsItem *>(children.first());
            if (firstchild->hasSameData(data) && firstchild != this){
                return firstchild;
            }
            auto tempList = firstchild->childItems();
            children.removeFirst();
            children = tempList + children;
        }
    }
    return nullptr;
}

int TreeGraphicsItem::getIndexOfChild(TreeGraphicsItem *child) const{
    auto children = childItems();
    if (!children.isEmpty()){
        return children.indexOf(child);
    }
    return -1;
}

int TreeGraphicsItem::getIndexofIconWithData(DataIconManager *data) const{
    if (data){
        auto children = childItems();
        for (auto i = 0; i < children.size(); i++){
            if (static_cast<TreeGraphicsItem *>(children.at(i))->hasSameData(data)){
                return i;
            }
        }
    }
    return -1;
}

bool TreeGraphicsItem::getIsExpanded() const{
    return isExpanded;
}

void TreeGraphicsItem::setIsExpanded(bool value){
    isExpanded = value;
}

void TreeGraphicsItem::setPathToParent(){
    auto parent = static_cast<TreeGraphicsItem *>(parentItem());
    if (parent){
        QPainterPath newpath(scenePos());
        auto ptr = parent->getItemData();
        if (ptr->getSignature() != HKB_BLENDER_GENERATOR && ptr->getSignature() != BS_BONE_SWITCH_GENERATOR && ptr->getSignature() != HKB_POSE_MATCHING_GENERATOR){
            newpath.lineTo(QPointF(scenePos().x() - boundingRect().width(), scenePos().y() - yCoordinate + boundingRect().height()));
        }else{
            newpath.cubicTo(QPointF(scenePos().x() - boundingRect().width()/2, scenePos().y() - yCoordinate),
                            QPointF(scenePos().x() - boundingRect().width()/2, scenePos().y() + yCoordinate),
                            QPointF(scenePos().x() - boundingRect().width(), scenePos().y() - yCoordinate + boundingRect().height()));
        }
        path->setPath(newpath);
    }
}

QVector <TreeGraphicsItem *> TreeGraphicsItem::getAllIconsInBranch(TreeGraphicsItem *iconToFind) const{
    QVector <TreeGraphicsItem *> list;
    auto branchTip = this;
    if (iconToFind){
        for (auto i = 0; i < MAX_NUM_GRAPH_ICONS, branchTip; i++){
            auto tempparent = static_cast<TreeGraphicsItem *>(branchTip->parentItem());
            if (tempparent){
                if (tempparent->childItems().contains(iconToFind)){
                    branchTip = nullptr;
                }else{
                    list.append(tempparent);
                    branchTip = tempparent;
                }
            }else{
                branchTip = nullptr;
            }
        }
    }else{
        for (auto i = 0; i < MAX_NUM_GRAPH_ICONS, branchTip; i++){
            auto tempparent = static_cast<TreeGraphicsItem *>(branchTip->parentItem());
            if (tempparent){
                list.append(tempparent);
                branchTip = tempparent;
            }else{
                branchTip = nullptr;
            }
        }
    }
    return list;
}

DataIconManager *TreeGraphicsItem::getItemData() const{
    return itemData;
}

Qt::GlobalColor TreeGraphicsItem::getPenColor() const{
    return penColor;
}

