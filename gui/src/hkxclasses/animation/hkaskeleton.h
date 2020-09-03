#ifndef HKASKELETON_H
#define HKASKELETON_H

#include "src/hkxclasses/hkxobject.h"

#include <QVector>

class BehaviorFile;
class SkeletonFile;

namespace UI {

class hkaSkeleton final: public HkxObject
{
    friend class SkeletonUI;
    friend class SkeletonFile;
public:
    hkaSkeleton(HkxFile *parent, long ref = 0);
    hkaSkeleton& operator=(const hkaSkeleton&) = delete;
    hkaSkeleton(const hkaSkeleton &) = delete;
    ~hkaSkeleton();
public:
    static const QString getClassname();
    QStringList getBoneNames() const;
    QString getLocalFrameName(int boneIndex) const;
private:
    QString getName() const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool addLocalFrame(const QString & name);
    bool removeLocalFrame(int boneIndex);
    void setLocalFrameName(int boneIndex, const QString & name);
private:
    struct hkBone{
        hkBone():lockTranslation(false){}
        QString name;
        bool lockTranslation;
    };
    struct hkLocalFrame{
        hkLocalFrame():boneIndex(-1){}
        HkxSharedPtr localFrame;
        int boneIndex;
    };
private:
    static uint refCount;
    static const QString classname;
    QString name;
    QVector <int> parentIndices;
    QVector <hkBone> bones;
    QString referencePose;
    QVector <qreal> referenceFloats;
    QStringList floatSlots;
    QVector <hkLocalFrame> localFrames;
    mutable std::mutex mutex;
};
}
#endif // HKASKELETON_H
