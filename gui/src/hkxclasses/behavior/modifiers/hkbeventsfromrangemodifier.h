#ifndef HKBEVENTSFROMRANGEMODIFIER_H
#define HKBEVENTSFROMRANGEMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbEventRangeDataArray;

class hkbEventsFromRangeModifier final: public hkbModifier
{
    friend class EventsFromRangeModifierUI;
public:
    hkbEventsFromRangeModifier(HkxFile *parent, long ref = 0);
    hkbEventsFromRangeModifier& operator=(const hkbEventsFromRangeModifier&) = delete;
    hkbEventsFromRangeModifier(const hkbEventsFromRangeModifier &) = delete;
    ~hkbEventsFromRangeModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    qreal getInputValue() const;
    void setInputValue(const qreal &value);
    qreal getLowerBound() const;
    void setLowerBound(const qreal &value);
    hkbEventRangeDataArray * getEventRanges() const;
    void setEventRanges(hkbEventRangeDataArray *value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    int getNumberOfRanges() const;
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    bool merge(HkxObject *recessiveObject);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    qreal inputValue;
    qreal lowerBound;
    HkxSharedPtr eventRanges;
    mutable std::mutex mutex;
};
}
#endif // HKBEVENTSFROMRANGEMODIFIER_H
