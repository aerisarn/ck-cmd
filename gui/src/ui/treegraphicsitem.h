#ifndef TREEGRAPHICSITEM_H
#define TREEGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPainter>

class TreeGraphicsScene;
class DataIconManager;

class GraphicsPathItem final: public QGraphicsPathItem
{
public:
    GraphicsPathItem() = default;
    GraphicsPathItem& operator=(const GraphicsPathItem&) = delete;
    GraphicsPathItem(const GraphicsPathItem &) = delete;
    ~GraphicsPathItem() = default;
protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *){
        painter->setRenderHints(QPainter::Antialiasing);
        painter->setPen(QPen(QBrush(Qt::black), 2));
        painter->drawPath(path());
    }
};

class TreeGraphicsItem: public QGraphicsItem
{
    friend class TreeGraphicsScene;
    friend class BehaviorGraphView;
public:
    TreeGraphicsItem(TreeGraphicsItem *parent, DataIconManager *obj, int indexToInsert = -1, Qt::GlobalColor color = Qt::gray);
    TreeGraphicsItem& operator=(const TreeGraphicsItem&) = delete;
    TreeGraphicsItem(const TreeGraphicsItem &) = delete;
    ~TreeGraphicsItem();
public:
    virtual QRectF boundingRect() const;
    virtual QRectF branchExpandCollapseBox() const;
    void setBrushColor(Qt::GlobalColor color);
    void setPenColor(Qt::GlobalColor color);
    bool reorderChildren();
    TreeGraphicsItem *getChildWithData(DataIconManager *data);
    bool isCircular(TreeGraphicsItem *itemtocheck) const;
protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
    virtual void setIconSelected();
    virtual void unselect();
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void setPosition(const QPointF &pos);
    bool getIsExpanded() const;
    Qt::GlobalColor getPenColor() const;
public:
    bool isDataDescendant(TreeGraphicsItem *icon) const;
    bool isDataDescendant(DataIconManager *data) const;
    bool hasSameData(TreeGraphicsItem *icon) const;
    bool hasSameData(DataIconManager *data) const;
    int getIconIndex();
    bool hasIcons() const;
    bool isPrimaryIcon() const;
    int getIndexOfChild(TreeGraphicsItem *child) const;
    int getIndexofIconWithData(DataIconManager *data) const;
    DataIconManager *getItemData() const;
    TreeGraphicsItem * setParent(TreeGraphicsItem *newParent, int indexToInsert = -1);
    void reposition();
private:
    TreeGraphicsItem * getPrimaryIcon() const;
    QVector <TreeGraphicsItem *> getAllIconsInBranch(TreeGraphicsItem *iconToFind = nullptr) const;
    qreal getYCoordinate();
    TreeGraphicsItem *reconnectToNextDuplicate();
    TreeGraphicsItem * getNextChildAfter(TreeGraphicsItem *child);
    TreeGraphicsItem *getReplacementIcon(DataIconManager *data);
    void setIsExpanded(bool value);
    void setPathToParent();
private:
    Qt::GlobalColor brushColor;
    Qt::GlobalColor penColor;
    DataIconManager * itemData;
    bool isExpanded;
    qreal yCoordinate;
    GraphicsPathItem *path;
    QFont itemFont;
};

#endif // TREEGRAPHICSITEM_H
