#include "treegraphicsview.h"
#include "treegraphicsscene.h"
#include "treegraphicsitem.h"
#include "src/utility.h"

#include <QWheelEvent>
#include <QMouseEvent>

TreeGraphicsView::TreeGraphicsView(QMenu *menu)
    : popUpMenu(menu),
      treeScene(new TreeGraphicsScene()),
      initScale(1),
      iconFocusScale(1.5),
      currentScale(1),
      scaleUpFactor(1.2),
      scaleDownFactor(0.8)
{
    setScene(treeScene);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    connect(treeScene, SIGNAL(iconSelected(TreeGraphicsItem*)), this, SIGNAL(iconSelected(TreeGraphicsItem*)), Qt::UniqueConnection);
}

QSize TreeGraphicsView::sizeHint() const{
    return QSize(500, 400);
}

bool TreeGraphicsView::drawGraphMT(DataIconManager *rootData, bool allowDuplicates, int &taskcount, std::mutex & mutex, std::condition_variable & conditionVar){
    auto value = treeScene->drawGraph(rootData, allowDuplicates);
    mutex.lock();
    taskcount--;
    conditionVar.notify_one();
    mutex.unlock();
    return value;
}

bool TreeGraphicsView::drawGraph(DataIconManager *rootData, bool allowDuplicates){
    return treeScene->drawGraph(rootData, allowDuplicates);
}

void TreeGraphicsView::wheelEvent(QWheelEvent *event){
    zoom(event->delta());
}

void TreeGraphicsView::zoom(int delta){
    if (delta > 0){
        scale(scaleUpFactor, scaleUpFactor);
        currentScale = currentScale*scaleUpFactor;
    }else{
        scale(scaleDownFactor, scaleDownFactor);
        currentScale = currentScale*scaleDownFactor;
    }
}

void TreeGraphicsView::mousePressEvent(QMouseEvent *event){
    (event->button() == Qt::LeftButton) ? setDragMode(QGraphicsView::ScrollHandDrag) : NULL;
    QGraphicsView::mousePressEvent(event);
}

void TreeGraphicsView::mouseReleaseEvent(QMouseEvent *event){
    (event->button() == Qt::LeftButton) ? setDragMode(QGraphicsView::NoDrag) : NULL;
    QGraphicsView::mouseReleaseEvent(event);
}

void TreeGraphicsView::contractBranch(){
    auto selected = getSelectedItem();
    if (selected){
        treeScene->contractBranch(selected, true);
        selected->reposition();
    }
}

void TreeGraphicsView::expandBranch(){
    auto selected = getSelectedItem();
    if (selected){
        treeScene->expandBranch(selected, true);
        selected->reposition();
    }
}

void TreeGraphicsView::selectRoot(){
    treeScene->selectIcon(treeScene->rootIcon, TreeGraphicsScene::EXPAND_CONTRACT_ZERO);
}

TreeGraphicsItem *TreeGraphicsView::getSelectedItem() const{
    return treeScene->selectedIcon;
}

DataIconManager *TreeGraphicsView::getSelectedData() const{
    if (treeScene->selectedIcon){
        return treeScene->selectedIcon->getItemData();
    }
    return nullptr;
}

bool TreeGraphicsView::reconnectIcon(TreeGraphicsItem *oldIconParent, DataIconManager *dataToReplace, int replaceindex, DataIconManager *replacementData, bool removeData){
    return treeScene->reconnectIcon(oldIconParent, dataToReplace, replaceindex, replacementData, removeData);
}

bool TreeGraphicsView::removeItemFromGraph(TreeGraphicsItem *item, int indexToRemove, bool removeData, bool removeAllSameData){
    if (item == treeScene->selectedIcon){
        treeScene->selectedIcon = nullptr;
    }
    return treeScene->removeItemFromGraph(item, indexToRemove, removeData, removeAllSameData);
}

TreeGraphicsItem *TreeGraphicsView::addItemToGraph(TreeGraphicsItem *selectedIcon, DataIconManager *data, int indexToInsert, bool inject, bool allowDuplicates){
    return treeScene->addItemToGraph(selectedIcon, data, indexToInsert, inject, allowDuplicates);
}
