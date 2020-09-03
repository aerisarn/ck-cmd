#ifndef BSEVENTONFALSETOTRUEMODIFIER_H
#define BSEVENTONFALSETOTRUEMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbStringEventPayload;

class BSEventOnFalseToTrueModifier final: public hkbModifier
{
    friend class BSEventOnFalseToTrueModifierUI;
public:
    BSEventOnFalseToTrueModifier(HkxFile *parent, long ref = 0);
    BSEventOnFalseToTrueModifier& operator=(const BSEventOnFalseToTrueModifier&) = delete;
    BSEventOnFalseToTrueModifier(const BSEventOnFalseToTrueModifier &) = delete;
    ~BSEventOnFalseToTrueModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    bool getBEnableEvent1() const;
    void setBEnableEvent1(bool value);
    bool getBVariableToTest1() const;
    void setBVariableToTest1(bool value);
    int getEventToSend1ID() const;
    void setEventToSend1ID(int value);
    hkbStringEventPayload * getEventToSend1Payload() const;
    void setEventToSend1Payload(hkbStringEventPayload *value);

    bool getBEnableEvent2() const;
    void setBEnableEvent2(bool value);
    bool getBVariableToTest2() const;
    void setBVariableToTest2(bool value);
    int getEventToSend2ID() const;
    void setEventToSend2ID(int value);
    hkbStringEventPayload * getEventToSend2Payload() const;
    void setEventToSend2Payload(hkbStringEventPayload *value);


    bool getBEnableEvent3() const;
    void setBEnableEvent3(bool value);
    bool getBVariableToTest3() const;
    void setBVariableToTest3(bool value);
    int getEventToSend3ID() const;
    void setEventToSend3ID(int value);
    hkbStringEventPayload * getEventToSend3Payload() const;
    void setEventToSend3Payload(hkbStringEventPayload *value);

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
    bool bEnableEvent1;
    bool bVariableToTest1;
    hkEventPayload eventToSend1;
    bool bEnableEvent2;
    bool bVariableToTest2;
    hkEventPayload eventToSend2;
    bool bEnableEvent3;
    bool bVariableToTest3;
    hkEventPayload eventToSend3;
    mutable std::mutex mutex;
};
}
#endif // BSEVENTONFALSETOTRUEMODIFIER_H
