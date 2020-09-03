#ifndef HKSIMPLELOCALFRAME_H
#define HKSIMPLELOCALFRAME_H

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkSimpleLocalFrame final: public HkxObject
{
public:
    hkSimpleLocalFrame(HkxFile *parent, const QString & string = "", long ref = -1);
    hkSimpleLocalFrame& operator=(const hkSimpleLocalFrame&) = delete;
    hkSimpleLocalFrame(const hkSimpleLocalFrame &) = delete;
    ~hkSimpleLocalFrame();
public:
    static const QString getClassname();
    QString getName() const;
    void setName(const QString &value);
private:
    bool readData(const HkxXmlReader & reader, long & index);
    QString evaluateDataValidity();
    bool link(){return true;}
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    //hkTransform transform;
    //QVector <hkLocalFrame> children;
    //hkLocalFrame parentFrame;
    //hkLocalFrameGroup group;
    QString name;
    mutable std::mutex mutex;
};
}
#endif // HKSIMPLELOCALFRAME_H
