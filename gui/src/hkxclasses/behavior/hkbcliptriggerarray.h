#ifndef HKBCLIPTRIGGERARRAY_H
#define HKBCLIPTRIGGERARRAY_H

#include "src/hkxclasses/hkxobject.h"
class BehaviorFile;

namespace UI {

class hkbStringEventPayload;

class hkbClipTriggerArray final: public HkxObject
{
    friend class hkbClipGenerator;
    friend class ClipGeneratorUI;
    friend class ClipTriggerUI;
public:
    hkbClipTriggerArray(HkxFile *parent, long ref = -1);
    hkbClipTriggerArray& operator=(const hkbClipTriggerArray&) = delete;
    hkbClipTriggerArray(const hkbClipTriggerArray &) = delete;
    ~hkbClipTriggerArray();
public:
    int getNumberOfTriggers() const;
    static const QString getClassname();
    int getLastTriggerIndex() const;
private:
    struct HkTrigger
    {
        HkTrigger()
            : localTime(0),
              relativeToEndOfClip(false),
              acyclic(false),
              isAnnotation(false)
        {
            //
        }
        qreal localTime;
        hkEventPayload event;
        bool relativeToEndOfClip;
        bool acyclic;
        bool isAnnotation;
    };
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    bool merge(HkxObject *recessiveObject);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    void addTrigger(const HkTrigger & trigger = HkTrigger());
    void removeTrigger(int index);
    void setTriggerIdAt(int index, int id);
    void setPayloadAt(int index, hkbStringEventPayload *load);
    void setLocalTimeAt(int index, qreal time);
    void setRelativeToEndOfClipAt(int index, bool value);
    void setAcyclicAt(int index, bool value);
    void setIsAnnotationAt(int index, bool value);
    int getTriggerIdAt(int index) const;
    hkbStringEventPayload * getPayloadAt(int index) const;
    qreal getLocalTimeAt(int index) const;
    bool getRelativeToEndOfClipAt(int index) const;
    bool getAcyclicAt(int index) const;
    bool getIsAnnotationAt(int index) const;
private:
    static uint refCount;
    static QString const classname;
    QVector <HkTrigger> triggers;
    mutable std::mutex mutex;
};
}
#endif // HKBCLIPTRIGGERARRAY_H
