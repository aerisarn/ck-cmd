#ifndef HKBMIRROREDSKELETONINFO_H
#define HKBMIRROREDSKELETONINFO_H

#include "src/hkxclasses/hkxobject.h"

#include <QVector>
namespace UI {
class BehaviorFile;

class hkbMirroredSkeletonInfo final: public HkxObject
{
public:
    hkbMirroredSkeletonInfo(HkxFile *parent, long ref = 0, int size = 0);
    hkbMirroredSkeletonInfo& operator=(const hkbMirroredSkeletonInfo&) = delete;
    hkbMirroredSkeletonInfo(const hkbMirroredSkeletonInfo &) = delete;
    ~hkbMirroredSkeletonInfo();
public:
    static const QString getClassname();
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    hkQuadVariable mirrorAxis;
    QVector <int> bonePairMap;
    mutable std::mutex mutex;
};
}
#endif // HKBMIRROREDSKELETONINFO_H
