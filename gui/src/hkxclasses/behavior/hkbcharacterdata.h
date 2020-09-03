#ifndef HKBCHARACTERDATA_H
#define HKBCHARACTERDATA_H

#include <QVector>

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbCharacterStringData;
class hkbVariableValueSet;
class hkbMirroredSkeletonInfo;
class hkbFootIkDriverInfo;
class hkbHandIkDriverInfo;

class hkbCharacterData final: public HkxObject
{
    friend class CharacterFile;
    friend class CharacterPropertiesUI;
public:
    hkbCharacterData(HkxFile *parent, long ref = 0, hkbCharacterStringData *strings = nullptr, hkbVariableValueSet *values = nullptr, hkbMirroredSkeletonInfo *mirrorskelinfo = nullptr);
    hkbCharacterData& operator=(const hkbCharacterData&) = delete;
    hkbCharacterData(const hkbCharacterData &) = delete;
    ~hkbCharacterData();
public:
    QStringList getVariableNames() const;
    static const QString getClassname();
    int getWordVariableValueAt(int index);
    hkQuadVariable getQuadVariable(int index, bool *ok) const;
    HkxObject * getVariantVariable(int index) const;
    hkVariableType getVariableTypeAt(int index) const;
    QStringList getCharacterPropertyNames() const;
    QString getCharacterPropertyNameAt(int index) const;
    QString getLastCharacterPropertyName() const;
    QStringList getCharacterPropertyTypenames() const;
    hkVariableType getCharacterPropertyTypeAt(int index) const;
    void addVariable(hkVariableType type, const QString & name);
    void addVariable(hkVariableType type);
    void removeVariable(int index);
    void setCharacterPropertyNameAt(int index, const QString & name);
    void setWordVariableValueAt(int index, int value);
    void setQuadVariableValueAt(int index, hkQuadVariable value);
private:
    void setVariableNameAt(int index, const QString & name);
    void setHandIkDriverInfo(hkbHandIkDriverInfo *value);
    void setFootIkDriverInfo(hkbFootIkDriverInfo *fooik);
    QString evaluateDataValidity();
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    bool write(HkxXMLWriter *writer);
    bool merge(HkxObject *recessiveobj);
private:
    struct hkVariableInfo
    {
        struct hkRole
        {
            hkRole(): role("ROLE_DEFAULT"), flags("0"){}
            QString role;
            QString flags;
        };

        hkRole role;
        QString type;
    };

    struct hkCharacterControllerInfo{
        hkCharacterControllerInfo(): capsuleHeight(1.7), capsuleRadius(0.4), collisionFilterInfo(1){}
        qreal capsuleHeight;
        qreal capsuleRadius;
        int collisionFilterInfo;
        HkxSharedPtr characterControllerCinfo;
    };
private:
    static uint refCount;
    static const QString classname;
    static const QStringList Type;    //See hkVariableType...
    hkCharacterControllerInfo characterControllerInfo;
    hkQuadVariable modelUpMS;
    hkQuadVariable modelForwardMS;
    hkQuadVariable modelRightMS;
    QVector <hkVariableInfo> characterPropertyInfos;
    QVector <int> numBonesPerLod;
    HkxSharedPtr characterPropertyValues;
    HkxSharedPtr footIkDriverInfo;
    HkxSharedPtr handIkDriverInfo;
    HkxSharedPtr stringData;
    HkxSharedPtr mirroredSkeletonInfo;
    qreal scale;
    mutable std::mutex mutex;
};
}
#endif // HKBCHARACTERDATA_H
