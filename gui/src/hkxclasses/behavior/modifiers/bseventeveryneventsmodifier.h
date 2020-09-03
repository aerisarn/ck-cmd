#ifndef BSEVENTEVERYNEVENTSMODIFIER_H
#define BSEVENTEVERYNEVENTSMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbStringEventPayload;

class BSEventEveryNEventsModifier final: public hkbModifier
{
    friend class BSEventEveryNEventsModifierUI;
public:
    BSEventEveryNEventsModifier(HkxFile *parent, long ref = 0);
    BSEventEveryNEventsModifier& operator=(const BSEventEveryNEventsModifier&) = delete;
    BSEventEveryNEventsModifier(const BSEventEveryNEventsModifier &) = delete;
    ~BSEventEveryNEventsModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    void setEventToCheckForPayload(hkbStringEventPayload *value);
    void setEventToSendPayload(hkbStringEventPayload *value);
    hkbStringEventPayload *getEventToSendPayload() const;
    hkbStringEventPayload *getEventToCheckForPayload() const;
    int getEventToCheckForID() const;
    void setEventToCheckForID(int value);
    int getEventToSendID() const;
    void setEventToSendID(int value);
    int getNumberOfEventsBeforeSend() const;
    void setNumberOfEventsBeforeSend(int value);
    int getMinimumNumberOfEventsBeforeSend() const;
    void setMinimumNumberOfEventsBeforeSend(int value);
    bool getRandomizeNumberOfEvents() const;
    void setRandomizeNumberOfEvents(bool value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
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
    hkEventPayload eventToCheckFor;
    hkEventPayload eventToSend;
    int numberOfEventsBeforeSend;
    int minimumNumberOfEventsBeforeSend;
    bool randomizeNumberOfEvents;
    mutable std::mutex mutex;
};
}
#endif // BSEVENTEVERYNEVENTSMODIFIER_H
