#ifndef TREEGRAPHICSVIEW_H
#define TREEGRAPHICSVIEW_H

#include <QGraphicsView>

#include <mutex>
#include <condition_variable>

class QMenu;
class TreeGraphicsScene;
class DataIconManager;
class TreeGraphicsItem;

class TreeGraphicsView: public QGraphicsView
{
    Q_OBJECT
    friend class BehaviorGraphView;
public:
    TreeGraphicsView(QMenu *menu);
    TreeGraphicsView& operator=(const TreeGraphicsView&) = delete;
    TreeGraphicsView(const TreeGraphicsView &) = delete;
    ~TreeGraphicsView() = default;
public:
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void zoom(int delta);
    bool drawGraphMT(DataIconManager *rootData, bool allowDuplicates, int &taskcount, std::mutex &mutex, std::condition_variable &conditionVar);
    bool drawGraph(DataIconManager *rootData, bool allowDuplicates);
    TreeGraphicsItem * getSelectedItem() const;
    DataIconManager * getSelectedData() const;
    bool reconnectIcon(TreeGraphicsItem *oldIconParent, DataIconManager *dataToReplace, int replaceindex, DataIconManager *replacementData, bool removeData = true);
    bool removeItemFromGraph(TreeGraphicsItem *item, int indexToRemove, bool removeData = true, bool removeAllSameData = false);
    TreeGraphicsItem * addItemToGraph(TreeGraphicsItem *selectedIcon, DataIconManager *data, int indexToInsert, bool inject = false, bool allowDuplicates = false);
signals:
    void iconSelected(TreeGraphicsItem *selected);
protected:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void contractBranch();
    void expandBranch();
    void selectRoot();
private:
    QMenu *popUpMenu;
    TreeGraphicsScene *treeScene;
    const qreal initScale;
    const qreal iconFocusScale;
    qreal currentScale;
    const qreal scaleUpFactor;
    const qreal scaleDownFactor;
};

#endif // TREEGRAPHICSVIEW_H
