#ifndef HKBFOOTIKDRIVERINFO_H
#define HKBFOOTIKDRIVERINFO_H

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbFootIkDriverInfo final: public HkxObject
{
    friend class FootIkDriverInfoUI;
    friend class FootIkDriverInfoLegUI;
public:
    hkbFootIkDriverInfo(HkxFile *parent, long ref = 0);
    hkbFootIkDriverInfo& operator=(const hkbFootIkDriverInfo&) = delete;
    hkbFootIkDriverInfo(const hkbFootIkDriverInfo &) = delete;
    ~hkbFootIkDriverInfo();
public:
    static const QString getClassname();
private:
    struct hkbFootIkDriverInfoLeg{
        hkbFootIkDriverInfoLeg(hkbFootIkDriverInfo *par = nullptr)
            : parent(par),
              footPlantedAnkleHeightMS(0),
              footRaisedAnkleHeightMS(0),
              maxAnkleHeightMS(0),
              minAnkleHeightMS(0),
              maxKneeAngleDegrees(0),
              minKneeAngleDegrees(0),
              maxAnkleAngleDegrees(0),
              hipIndex(-1),
              kneeIndex(-1),
              ankleIndex(-1)
        {
            //
        }

        hkbFootIkDriverInfo *parent;
        hkQuadVariable kneeAxisLS;
        hkQuadVariable footEndLS;
        qreal footPlantedAnkleHeightMS;
        qreal footRaisedAnkleHeightMS;
        qreal maxAnkleHeightMS;
        qreal minAnkleHeightMS;
        qreal maxKneeAngleDegrees;
        qreal minKneeAngleDegrees;
        qreal maxAnkleAngleDegrees;
        int hipIndex;
        int kneeIndex;
        int ankleIndex;
    };
private:
    qreal getRaycastDistanceUp() const;
    void setRaycastDistanceUp(const qreal &value);
    qreal getRaycastDistanceDown() const;
    void setRaycastDistanceDown(const qreal &value);
    qreal getOriginalGroundHeightMS() const;
    void setOriginalGroundHeightMS(const qreal &value);
    qreal getVerticalOffset() const;
    void setVerticalOffset(const qreal &value);
    int getCollisionFilterInfo() const;
    void setCollisionFilterInfo(int value);
    qreal getForwardAlignFraction() const;
    void setForwardAlignFraction(const qreal &value);
    qreal getSidewaysAlignFraction() const;
    void setSidewaysAlignFraction(const qreal &value);
    qreal getSidewaysSampleWidth() const;
    void setSidewaysSampleWidth(const qreal &value);
    bool getLockFeetWhenPlanted() const;
    void setLockFeetWhenPlanted(bool value);
    bool getUseCharacterUpVector() const;
    void setUseCharacterUpVector(bool value);
    bool getIsQuadrupedNarrow() const;
    void setIsQuadrupedNarrow(bool value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    void addLeg();
    void removeLegAt(int index);
    hkbFootIkDriverInfoLeg * getLegAt(int index);
    int getNumberOfLegs() const;
private:
    static uint refCount;
    static const QString classname;
    QVector <hkbFootIkDriverInfoLeg> legs;
    qreal raycastDistanceUp;
    qreal raycastDistanceDown;
    qreal originalGroundHeightMS;
    qreal verticalOffset;
    int collisionFilterInfo;
    qreal forwardAlignFraction;
    qreal sidewaysAlignFraction;
    qreal sidewaysSampleWidth;
    bool lockFeetWhenPlanted;
    bool useCharacterUpVector;
    bool isQuadrupedNarrow;
    mutable std::mutex mutex;
};
}
#endif // HKBFOOTIKDRIVERINFO_H
