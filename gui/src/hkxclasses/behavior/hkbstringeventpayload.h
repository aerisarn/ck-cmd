#ifndef HKBSTRINGEVENTPAYLOAD_H
#define HKBSTRINGEVENTPAYLOAD_H

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbStringEventPayload final: public HkxObject
{
public:
    hkbStringEventPayload(HkxFile *parent, const QString & string = "", long ref = -1);
    hkbStringEventPayload& operator=(const hkbStringEventPayload&) = delete;
    hkbStringEventPayload(const hkbStringEventPayload &) = delete;
    ~hkbStringEventPayload();
public:
    static const QString getClassname();
    QString getData() const;
    void setData(const QString &value);
private:
    bool readData(const HkxXmlReader & reader, long & index);
    QString evaluateDataValidity();
    bool link(){return true;}
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    QString data;
    mutable std::mutex mutex;
};
}
#endif // HKBSTRINGEVENTPAYLOAD_H
