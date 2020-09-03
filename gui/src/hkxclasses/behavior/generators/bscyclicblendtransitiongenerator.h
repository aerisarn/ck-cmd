#ifndef BSCYCLICBLENDTRANSITIONGENERATOR_H
#define BSCYCLICBLENDTRANSITIONGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
class BSCyclicBlendTransitionGenerator final: public hkbGenerator
{
    friend class BSCyclicBlendTransitionGeneratorUI;
public:
    BSCyclicBlendTransitionGenerator(HkxFile *parent, long ref = 0);
    BSCyclicBlendTransitionGenerator& operator=(const BSCyclicBlendTransitionGenerator&) = delete;
    BSCyclicBlendTransitionGenerator(const BSCyclicBlendTransitionGenerator &) = delete;
    ~BSCyclicBlendTransitionGenerator();
public:
    static const QString getClassname();
    QString getName() const;
private:
    void nullEventToCrossBlend();
    void nullEventToFreezeBlendValue();
    void setEBlendCurve(int index);
    void setFTransitionDuration(const qreal &value);
    void setFBlendParameter(const qreal &value);
    void setName(const QString &newname);
    QString getBlenderGeneratorName() const;
    qreal getFTransitionDuration() const;
    QString getEBlendCurve() const;
    int getEventToFreezeBlendValueId() const;
    int getEventToCrossBlendId() const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    void updateReferences(long &ref);
    bool merge(HkxObject *recessiveObject);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    QVector <DataIconManager *> getChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    static const QStringList BlendCurve;
    ulong userData;
    QString name;
    HkxSharedPtr pBlenderGenerator;
    hkEventPayload eventToFreezeBlendValue;
    hkEventPayload eventToCrossBlend;
    qreal fBlendParameter;
    qreal fTransitionDuration;
    QString eBlendCurve;
    mutable std::mutex mutex;
};
}
#endif // BSCYCLICBLENDTRANSITIONGENERATOR_H
