#ifndef HKBATTACHMENTMODIFIER_H
#define HKBATTACHMENTMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbAttachmentModifier final: public hkbModifier
{
public:
    hkbAttachmentModifier(HkxFile *parent, long ref = 0);
    hkbAttachmentModifier& operator=(const hkbAttachmentModifier&) = delete;
    hkbAttachmentModifier(const hkbAttachmentModifier &) = delete;
    ~hkbAttachmentModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
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
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    hkEventPayload sendToAttacherOnAttach;
    hkEventPayload sendToAttacheeOnAttach;
    hkEventPayload sendToAttacherOnDetach;
    hkEventPayload sendToAttacheeOnDetach;
    HkxSharedPtr attachmentSetup;
    HkxSharedPtr attacherHandle;
    HkxSharedPtr attacheeHandle;
    int attacheeLayer;
    mutable std::mutex mutex;
};
}
#endif // HKBATTACHMENTMODIFIER_H
