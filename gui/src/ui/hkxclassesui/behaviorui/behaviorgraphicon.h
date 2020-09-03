#ifndef BEHAVIORGRAPHICON_H
#define BEHAVIORGRAPHICON_H

#include "src/ui/treegraphicsitem.h"

#include <QRadialGradient>
#include <QPen>

class BehaviorGraphIcon: public TreeGraphicsItem
{
public:
    BehaviorGraphIcon(TreeGraphicsItem *parent, DataIconManager *obj, int indexToInsert = -1, Qt::GlobalColor color = Qt::gray);
protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;
    void unselect();
    void setIconSelected();
private:
    static const QRectF button;
    static const QRectF textRec;
    static const QFont font;
    static const QBrush brush;
    static const QBrush buttonBrush;
    static const QLineF vert;
    static const QLineF horiz;
    static const QRectF ellipse;
    static const QRectF square;
    QPen pen;
    QPolygonF polygon;
    QPolygonF arrowHead;
    QRadialGradient rGrad;
    QPen textPen;
    QPainterPath path;
};

#endif // BEHAVIORGRAPHICON_H
