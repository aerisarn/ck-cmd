#ifndef DATAICONMANAGER_H
#define DATAICONMANAGER_H

#include <QString>
#include <QVector>

#include "src/hkxclasses/hkxobject.h"

class TreeGraphicsItem;
class BehaviorFile;

namespace UI {
	class hkbModifier;
}

class DataIconManager: public UI::HkDynamicObject
{
    friend class TreeGraphicsItem;
    friend class TreeGraphicsScene;
protected:
    DataIconManager(HkxFile *parent, long ref = -1);
public:
    DataIconManager& operator=(const DataIconManager&) = delete;
    DataIconManager(const DataIconManager &) = delete;
    ~DataIconManager() = default;
public:
    virtual QString getName() const;
    virtual bool hasChildren() const;
    bool hasIcons() const;
    int getNumberOfIcons() const;
    TreeGraphicsItem * getFirstIcon() const;
    bool hasSameSignatureAndName(const DataIconManager * other) const;
    virtual int getIndexOfObj(DataIconManager *) const;
    bool isCircularLoop() const;
    virtual QVector <DataIconManager *> getChildren() const;
    void updateIconNames();
    void setIconValidity(bool valid);
    void setFocusOnTopIcon();
protected:
    virtual bool insertObjectAt(int , DataIconManager *);
    virtual bool removeObjectAt(int );
    void injectWhileMerging(HkxObject *recessiveobj);
    bool merge(HkxObject *recessiveObject);
private:
    bool wrapObjectAt(int index, DataIconManager *obj, DataIconManager *parentObj);
    TreeGraphicsItem *reconnectToNext();
    void appendIcon(TreeGraphicsItem *icon);
    void removeIcon(TreeGraphicsItem *icon);
private:
    QVector <TreeGraphicsItem *> icons;
};

#endif // DATAICONMANAGER_H
