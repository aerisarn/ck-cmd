#ifndef HKBFOOTIKMODIFIER_H
#define HKBFOOTIKMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbFootIkModifier final: public hkbModifier
{
    //friend class FootIkModifierUI;
public:
    hkbFootIkModifier(HkxFile *parent, long ref = 0);
    hkbFootIkModifier& operator=(const hkbFootIkModifier&) = delete;
    hkbFootIkModifier(const hkbFootIkModifier &) = delete;
    ~hkbFootIkModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    int getNumberOfLegs()const;
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    bool merge(HkxObject *recessiveObject);
private:
    struct hkGains{
        hkGains()
            : onOffGain(0),
              groundAscendingGain(0),
              groundDescendingGain(0),
              footPlantedGain(0),
              footRaisedGain(0),
              footUnlockGain(0),
              worldFromModelFeedbackGain(0),
              errorUpDownBias(0),
              alignWorldFromModelGain(0),
              hipOrientationGain(0),
              maxKneeAngleDifference(0),
              ankleOrientationGain(0)
        {
            //
        }

        qreal onOffGain;
        qreal groundAscendingGain;
        qreal groundDescendingGain;
        qreal footPlantedGain;
        qreal footRaisedGain;
        qreal footUnlockGain;
        qreal worldFromModelFeedbackGain;
        qreal errorUpDownBias;
        qreal alignWorldFromModelGain;
        qreal hipOrientationGain;
        qreal maxKneeAngleDifference;
        qreal ankleOrientationGain;
    };

    struct hkLeg{
        hkLeg()
            : id(-1),
              footPlantedAnkleHeightMS(0),
              footRaisedAnkleHeightMS(0),
              maxAnkleHeightMS(0),
              minAnkleHeightMS(0),
              maxKneeAngleDegrees(0),
              minKneeAngleDegrees(0),
              verticalError(0),
              maxAnkleAngleDegrees(0),
              hipIndex(-1),
              kneeIndex(-1),
              ankleIndex(-1),
              hitSomething(false),
              isPlantedMS(false),
              isOriginalAnkleTransformMSSet(false)
        {
            //
        }

        hkQsTransform originalAnkleTransformMS;
        hkQuadVariable kneeAxisLS;
        hkQuadVariable footEndLS;
        int id;
        HkxSharedPtr payload;
        qreal footPlantedAnkleHeightMS;
        qreal footRaisedAnkleHeightMS;
        qreal maxAnkleHeightMS;
        qreal minAnkleHeightMS;
        qreal maxKneeAngleDegrees;
        qreal minKneeAngleDegrees;
        qreal verticalError;
        qreal maxAnkleAngleDegrees;
        int hipIndex;
        int kneeIndex;
        int ankleIndex;
        bool hitSomething;
        bool isPlantedMS;
        bool isOriginalAnkleTransformMSSet;
    };
private:
    static uint refCount;
    static const QString classname;
    static const QStringList AlignMode;
    long userData;
    QString name;
    bool enable;
    hkGains gains;
    QVector <hkLeg> legs;
    qreal raycastDistanceUp;
    qreal raycastDistanceDown;
    qreal originalGroundHeightMS;
    qreal errorOut;
    hkQuadVariable errorOutTranslation;
    hkQuadVariable alignWithGroundRotation;
    qreal verticalOffset;
    int collisionFilterInfo;
    qreal forwardAlignFraction;
    qreal sidewaysAlignFraction;
    qreal sidewaysSampleWidth;
    bool useTrackData;
    bool lockFeetWhenPlanted;
    bool useCharacterUpVector;
    QString alignMode;
    mutable std::mutex mutex;
};
}

#endif // HKBFOOTIKMODIFIER_H
