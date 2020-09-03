#ifndef HKBCHARACTERSTRINGDATA_H
#define HKBCHARACTERSTRINGDATA_H

#include <QStringList>

#include "src/hkxclasses/hkxobject.h"

class HkxFile;

namespace UI {

class hkbCharacterStringData final: public HkxObject
{
    friend class hkbCharacterData;
    friend class AnimationsUI;
public:
    hkbCharacterStringData(HkxFile *parent, long ref = 0);
    hkbCharacterStringData& operator=(const hkbCharacterStringData&) = delete;
    hkbCharacterStringData(const hkbCharacterStringData &) = delete;
    ~hkbCharacterStringData();
public:
    int getAnimationIndex(const QString & name) const;
    QStringList getAnimationNames() const;
    QString getCharacterPropertyNameAt(int index) const;
    QString getAnimationNameAt(int index) const;
    QStringList getCharacterPropertyNames() const;
    QString getLastCharacterPropertyName() const;
    static const QString getClassname();
    int getCharacterPropertyIndex(const QString &name) const;
    void setName(const QString &value);
    void setRigName(const QString &value);
    void setRagdollName(const QString &value);
    void setBehaviorFilename(const QString &value);
    void addAnimation(const QString &name = "");
    QString getBehaviorFilename() const;
    QString getRigName() const;
    QString getRagdollName() const;
    int getNumberOfAnimations() const;
private:
    void setVariableNameAt(int index, const QString & name);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    void removeCharacterPropertyNameAt(int index);
    void generateAppendCharacterPropertyName(const QString &type);
    int addCharacterPropertyName(const QString & name, bool * wasadded = nullptr);
    bool write(HkxXMLWriter *writer);
    bool merge(hkbCharacterStringData *obj);
    void setCharacterPropertyNameAt(int index, const QString &name);
private:
    static uint refCount;
    static const QString classname;
    QStringList deformableSkinNames;
    QStringList rigidSkinNames;
    QStringList animationNames;
    QStringList animationFilenames;
    QStringList characterPropertyNames;
    QStringList retargetingSkeletonMapperFilenames;
    QStringList lodNames;
    QStringList mirroredSyncPointSubstringsA;
    QStringList mirroredSyncPointSubstringsB;
    QString name;
    QString rigName;
    QString ragdollName;
    QString behaviorFilename;
    mutable std::mutex mutex;
};
}
#endif // HKBCHARACTERSTRINGDATA_H
