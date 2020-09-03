#include "behaviorgraphicon.h"
#include "src/ui/treegraphicsscene.h"
#include "src/ui/dataiconmanager.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "src/utility.h"
#include "src/hkxclasses/hkxobject.h"

using namespace UI;

#define ITEM_WIDTH 500
#define ITEM_HEIGHT 50

#define OUTLINE_PEN_WIDTH 5
#define TEXT_PEN_WIDTH 2

const QRectF BehaviorGraphIcon::button = QRectF(ITEM_WIDTH*0.9, 0, ITEM_WIDTH*0.1, ITEM_HEIGHT);
const QLineF BehaviorGraphIcon::vert = QLineF(ITEM_WIDTH*0.95, ITEM_HEIGHT*0.1, ITEM_WIDTH*0.95, ITEM_HEIGHT*0.8);
const QLineF BehaviorGraphIcon::horiz = QLineF(ITEM_WIDTH*0.91, ITEM_HEIGHT*0.5, ITEM_WIDTH*0.99, ITEM_HEIGHT*0.5);
const QRectF BehaviorGraphIcon::textRec = QRectF(ITEM_WIDTH*0.25, ITEM_HEIGHT*0.2, ITEM_WIDTH*0.75, ITEM_HEIGHT*0.8);
const QFont BehaviorGraphIcon::font = QFont("Helvetica [Cronyx]", 16);
const QBrush BehaviorGraphIcon::brush = QBrush(Qt::green);
const QBrush BehaviorGraphIcon::buttonBrush = QBrush(Qt::gray);
const QRectF BehaviorGraphIcon::ellipse = QRectF(ITEM_WIDTH*0.1, ITEM_HEIGHT*0.1, ITEM_WIDTH*0.1, ITEM_HEIGHT*0.8);
const QRectF BehaviorGraphIcon::square = QRectF(ITEM_WIDTH*0.1, ITEM_HEIGHT*0.1, ITEM_WIDTH*0.1, ITEM_HEIGHT*0.8);

BehaviorGraphIcon::BehaviorGraphIcon(TreeGraphicsItem *parent, DataIconManager *obj, int indexToInsert, Qt::GlobalColor color)
    : TreeGraphicsItem(parent, obj, indexToInsert, color)
{
    arrowHead << QPointF(boundingRect().width()*0.9125, boundingRect().height()*0.5)
              << QPointF(boundingRect().width()*0.95, boundingRect().height()*0.1)
              << QPointF(boundingRect().width()*0.9875, boundingRect().height()*0.5);
    polygon << QPointF(boundingRect().width() * 0.1, boundingRect().height() * 0.1)
            << QPointF(boundingRect().width() * 0.1, boundingRect().height() * 0.9)
            << QPointF(boundingRect().width() * 0.2, boundingRect().height() * 0.5)
            << QPointF(boundingRect().width() * 0.1, boundingRect().height() * 0.1);
    rGrad.setCenter(boundingRect().topLeft());
    rGrad.setCenterRadius(boundingRect().width());
    rGrad.setColorAt(0.0, Qt::white);
    pen.setWidth(OUTLINE_PEN_WIDTH);
    textPen.setWidth(TEXT_PEN_WIDTH);
    if (getItemData()->getType() == HkxObject::TYPE_MODIFIER){
        path.addEllipse(ellipse);
        rGrad.setColorAt(1.0, Qt::magenta);
    }else{
        auto sig = getItemData()->getSignature();
        switch (sig){
        case HKB_BLENDER_GENERATOR:
            path.addRect(square), rGrad.setColorAt(1.0, Qt::darkRed); break;
        case HKB_BLENDER_GENERATOR_CHILD:
            path.addRect(square), rGrad.setColorAt(1.0, Qt::darkMagenta); break;
        case HKB_POSE_MATCHING_GENERATOR:
            path.addRect(square), rGrad.setColorAt(1.0, Qt::darkRed); break;
        case BS_BONE_SWITCH_GENERATOR:
            path.addRect(square), rGrad.setColorAt(1.0, Qt::darkRed); break;
        case BS_BONE_SWITCH_GENERATOR_BONE_DATA:
            path.addRect(square), rGrad.setColorAt(1.0, Qt::darkMagenta); break;
        case HKB_STATE_MACHINE:
            path.addPolygon(polygon), rGrad.setColorAt(1.0, Qt::darkBlue); break;
        case HKB_STATE_MACHINE_STATE_INFO:
            path.addPolygon(polygon), rGrad.setColorAt(1.0, Qt::cyan); break;
        case HKB_CLIP_GENERATOR:
            path.addEllipse(ellipse), rGrad.setColorAt(1.0, Qt::yellow); break;
        case HKB_BEHAVIOR_REFERENCE_GENERATOR:
            path.addEllipse(ellipse), rGrad.setColorAt(1.0, Qt::darkYellow); break;
        case BS_SYNCHRONIZED_CLIP_GENERATOR:
            path.addEllipse(ellipse), rGrad.setColorAt(1.0, Qt::yellow); break;
        default:
            path.addPolygon(polygon), rGrad.setColorAt(1.0, Qt::gray);
        }
    }
}

QRectF BehaviorGraphIcon::boundingRect() const{
    return QRectF(0, 0, ITEM_WIDTH, ITEM_HEIGHT);
}

void BehaviorGraphIcon::setIconSelected(){
    rGrad.setColorAt(1.0, Qt::green);
    auto thisscene = scene();
    if (thisscene){
        thisscene->update(QRectF(scenePos(), scenePos() + QPointF(boundingRect().width(), boundingRect().height())));
    }else{
        LogFile::writeToLog("BehaviorGraphIcon::setIconSelected(): The scene is nullptr!!");
    }
}

void BehaviorGraphIcon::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *){
    auto hkxdata = getItemData();
    if (hkxdata){
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setFont(font);
        pen.setColor(getPenColor());
        painter->setPen(pen);
        painter->setBrush(rGrad);
        painter->drawRoundedRect(boundingRect(), 4, 4);
        painter->setBrush(brush);
        painter->drawPath(path);
        painter->setPen(textPen);
        painter->drawText(textRec, getItemData()->getName());
        painter->setBrush(buttonBrush);
        if (hkxdata->hasChildren()){
            painter->drawRect(button);
            if (!hkxdata->getNumberOfIcons()){
                LogFile::writeToLog("BehaviorGraphIcon::paint(): Icon data has no icons!!!");
            }else if (hkxdata->getFirstIcon() == this){
                if (!childItems().isEmpty()){
                    painter->drawLine(horiz);
                    (!getIsExpanded() || !childItems().first()->isVisible()) ? painter->drawLine(vert) : NULL;
                }
            }else{
                painter->drawPolygon(arrowHead);
                painter->drawLine(vert);
            }
        }
    }else{
        LogFile::writeToLog("BehaviorGraphIcon::paint(): itemData is nullptr!!");
    }
}

void BehaviorGraphIcon::unselect(){
    if (getItemData()->getType() == HkxObject::TYPE_MODIFIER){
        rGrad.setColorAt(1.0, Qt::magenta);
    }else{
        auto sig = getItemData()->getSignature();
        switch (sig){
        case HKB_BLENDER_GENERATOR:
            rGrad.setColorAt(1.0, Qt::darkRed); break;
        case HKB_BLENDER_GENERATOR_CHILD:
            rGrad.setColorAt(1.0, Qt::darkMagenta); break;
        case HKB_POSE_MATCHING_GENERATOR:
            rGrad.setColorAt(1.0, Qt::darkRed); break;
        case BS_BONE_SWITCH_GENERATOR:
            rGrad.setColorAt(1.0, Qt::darkRed); break;
        case BS_BONE_SWITCH_GENERATOR_BONE_DATA:
            rGrad.setColorAt(1.0, Qt::darkMagenta); break;
        case HKB_STATE_MACHINE:
            rGrad.setColorAt(1.0, Qt::darkBlue); break;
        case HKB_STATE_MACHINE_STATE_INFO:
            rGrad.setColorAt(1.0, Qt::cyan); break;
        case HKB_CLIP_GENERATOR:
            rGrad.setColorAt(1.0, Qt::yellow); break;
        case HKB_BEHAVIOR_REFERENCE_GENERATOR:
            rGrad.setColorAt(1.0, Qt::darkYellow); break;
        case BS_SYNCHRONIZED_CLIP_GENERATOR:
            rGrad.setColorAt(1.0, Qt::yellow); break;
        default:
            rGrad.setColorAt(1.0, Qt::gray);
        }
    }
    auto thisscene = scene();
    if (thisscene){
        thisscene->update(QRectF(scenePos(), scenePos() + QPointF(boundingRect().width(), boundingRect().height())));
    }else{
        LogFile::writeToLog("BehaviorGraphIcon::unselect(): The scene is nullptr!!");
    }
}
