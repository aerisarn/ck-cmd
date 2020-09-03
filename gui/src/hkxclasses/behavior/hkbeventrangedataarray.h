#ifndef HKBEVENTRANGEDATAARRAY_H
#define HKBEVENTRANGEDATAARRAY_H

#include "src/hkxclasses/hkxobject.h"

//class hkbEventsFromRangeModifier;
namespace UI {
class hkbEventRangeDataArray final: public HkxObject
{
    friend class EventRangeDataUI;
    friend class EventsFromRangeModifierUI;
    friend class hkbEventsFromRangeModifier;
public:
    hkbEventRangeDataArray(HkxFile *parent, long ref = -1);
    hkbEventRangeDataArray& operator=(const hkbEventRangeDataArray&) = delete;
    hkbEventRangeDataArray(const hkbEventRangeDataArray &) = delete;
    ~hkbEventRangeDataArray();
public:
    static const QString getClassname();
    int getLastEventDataIndex() const;
    int getNumberOfRanges() const;
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
private:
    struct hkbEventRangeData
    {
        static const QStringList EventRangeMode;
        hkbEventRangeData() : upperBound(0), eventMode(EventRangeMode.first()){}
        qreal upperBound;
        hkEventPayload event;
        QString eventMode;
    };
    void addEventRange(const hkbEventRangeData & data = hkbEventRangeData());
    void setEventRangeIdAt(int index, int id);
    int getEventRangeIdAt(int index);
    void removeEventRange(int index);
private:
    static uint refCount;
    static const QString classname;
    //hkbEventsFromRangeModifier *parent;
    QVector <hkbEventRangeData> eventData;
    mutable std::mutex mutex;
};
}
#endif // HKBEVENTRANGEDATAARRAY_H
