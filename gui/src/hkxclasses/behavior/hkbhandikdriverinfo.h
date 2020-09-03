#ifndef HKBHANDIKDRIVERINFO_H
#define HKBHANDIKDRIVERINFO_H

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbHandIkDriverInfo final: public HkxObject
{
    friend class HandIkDriverInfoUI;
    friend class HandIkDriverInfoHandUI;
public:
    hkbHandIkDriverInfo(HkxFile *parent, long ref = 0);
    hkbHandIkDriverInfo& operator=(const hkbHandIkDriverInfo&) = delete;
    hkbHandIkDriverInfo(const hkbHandIkDriverInfo &) = delete;
    ~hkbHandIkDriverInfo();
public:
    static const QString getClassname();
private:
    int getNumberOfHands() const;
    QString getFadeInOutCurve() const;
    void setFadeInOutCurve(int index);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    void addHand();
    void removeHandAt(int index);
private:
    struct hkbHandIkDriverInfoHand{
        hkbHandIkDriverInfoHand()
            : maxElbowAngleDegrees(0),
              minElbowAngleDegrees(0),
              shoulderIndex(-1),
              shoulderSiblingIndex(-1),
              elbowIndex(-1),
              elbowSiblingIndex(-1),
              wristIndex(-1),
              enforceEndPosition(false),
              enforceEndRotation(false)
        {
            //
        }
        hkQuadVariable elbowAxisLS;
        hkQuadVariable backHandNormalLS;
        hkQuadVariable handOffsetLS;
        hkQuadVariable handOrienationOffsetLS;
        qreal maxElbowAngleDegrees;
        qreal minElbowAngleDegrees;
        int shoulderIndex;
        int shoulderSiblingIndex;
        int elbowIndex;
        int elbowSiblingIndex;
        int wristIndex;
        bool enforceEndPosition;
        bool enforceEndRotation;
        QString localFrameName;
    };
    hkbHandIkDriverInfoHand * getHandAt(int index);
private:
    static uint refCount;
    static const QString classname;
    static const QStringList BlendCurve;
    QVector <hkbHandIkDriverInfoHand> hands;
    QString fadeInOutCurve;
    mutable std::mutex mutex;
};
}
#endif // HKBHANDIKDRIVERINFO_H
