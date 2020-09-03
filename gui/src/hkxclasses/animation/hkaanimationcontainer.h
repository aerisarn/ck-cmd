#ifndef HKAANIMATIONCONTAINER_H
#define HKAANIMATIONCONTAINER_H

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkaAnimationContainer final: public HkxObject
{
public:
    hkaAnimationContainer(HkxFile *parent, long ref = 0);
    hkaAnimationContainer& operator=(const hkaAnimationContainer&) = delete;
    hkaAnimationContainer(const hkaAnimationContainer &) = delete;
    ~hkaAnimationContainer();
public:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    static const QString getClassname();
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    QVector <HkxSharedPtr> skeletons;
    //animations, bindings, attachments, skins, etc...
    mutable std::mutex mutex;
};
}
#endif // HKAANIMATIONCONTAINER_H
