#ifndef TREEGRAPHICSSCENE_H
#define TREEGRAPHICSSCENE_H

#include <QGraphicsScene>

class DataIconManager;
class TreeGraphicsItem;

class TreeGraphicsScene final: public QGraphicsScene
{
    Q_OBJECT
    friend class TreeGraphicsView;
    friend class TreeGraphicsItem;
    friend class BehaviorGraphView;
public:
    TreeGraphicsScene();
    TreeGraphicsScene& operator=(const TreeGraphicsScene&) = delete;
    TreeGraphicsScene(const TreeGraphicsScene &) = delete;
    ~TreeGraphicsScene() = default;
public:
    void setCanDeleteRoot(bool value);
    enum BranchBehaviorEnum{
        EXPAND_CONTRACT_ZERO,
        EXPAND_CONTRACT_ONE,
        EXPAND_CONTRACT_ALL
    };
signals:
    void iconSelected(TreeGraphicsItem *selected);
private:
    void selectIcon(TreeGraphicsItem *icon, BranchBehaviorEnum expand);
    void contractBranch(TreeGraphicsItem *icon, bool contractAll = false);
    void expandBranch(TreeGraphicsItem *icon, bool expandAll = false);
    bool drawGraph(DataIconManager *rootData, bool allowDuplicates);
    TreeGraphicsItem * addItemToGraph(TreeGraphicsItem *selectedIcon, DataIconManager *data, int indexToInsert, bool inject = false, bool allowDuplicates = false, bool isFirstDraw = false);
    bool reconnectIcon(TreeGraphicsItem *oldIconParent, DataIconManager *dataToReplace, int replaceindex, DataIconManager *replacementData, bool removeData = true);
    bool removeItemFromGraph(TreeGraphicsItem *item, int indexToRemove, bool removeData = true, bool removeAllSameData = false, DataIconManager *dataToKeep = nullptr);
private:
    TreeGraphicsItem *rootIcon;
    TreeGraphicsItem *selectedIcon;
    bool canDeleteRoot;
};

#endif // TREEGRAPHICSSCENE_H
