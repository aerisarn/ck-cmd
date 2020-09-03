#ifndef HKBFOOTIKCONTROLSMODIFIER_H
#define HKBFOOTIKCONTROLSMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbFootIkControlsModifier final: public hkbModifier
{
    friend class LegUI;
    friend class FootIkControlsModifierUI;
public:
    hkbFootIkControlsModifier(HkxFile *parent, long ref = 0);
    hkbFootIkControlsModifier& operator=(const hkbFootIkControlsModifier&) = delete;
    hkbFootIkControlsModifier(const hkbFootIkControlsModifier &) = delete;
    ~hkbFootIkControlsModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    struct hkControlData{
        hkControlData()
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
              verticalError(0),
              hitSomething(false),
              isPlantedMS(false)
        {
            //
        }

        hkQuadVariable groundPosition;
        int id;
        HkxSharedPtr payload;
        qreal verticalError;
        bool hitSomething;
        bool isPlantedMS;
    };
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    qreal getOnOffGain() const;
    void setOnOffGain(const qreal &value);
    qreal getGroundAscendingGain() const;
    void setGroundAscendingGain(const qreal &value);
    qreal getGroundDescendingGain() const;
    void setGroundDescendingGain(const qreal &value);
    qreal getFootPlantedGain() const;
    void setFootPlantedGain(const qreal &value);
    qreal getFootRaisedGain() const;
    void setFootRaisedGain(const qreal &value);
    qreal getFootUnlockGain() const;
    void setFootUnlockGain(const qreal &value);
    qreal getWorldFromModelFeedbackGain() const;
    void setWorldFromModelFeedbackGain(const qreal &value);
    qreal getErrorUpDownBias() const;
    void setErrorUpDownBias(const qreal &value);
    qreal getAlignWorldFromModelGain() const;
    void setAlignWorldFromModelGain(const qreal &value);
    qreal getHipOrientationGain() const;
    void setHipOrientationGain(const qreal &value);
    qreal getMaxKneeAngleDifference() const;
    void setMaxKneeAngleDifference(const qreal &value);
    qreal getAnkleOrientationGain() const;
    void setAnkleOrientationGain(const qreal &value);
    hkQuadVariable getErrorOutTranslation() const;
    void setErrorOutTranslation(const hkQuadVariable &value);
    hkQuadVariable getAlignWithGroundRotation() const;
    void setAlignWithGroundRotation(const hkQuadVariable &value);
    void addLeg(hkLeg leg = hkLeg());
    void removeLeg(int index);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    int getNumberOfLegs() const;
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    bool merge(HkxObject *recessiveObject);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    hkControlData gains;
    QVector <hkLeg> legs;
    hkQuadVariable errorOutTranslation;
    hkQuadVariable alignWithGroundRotation;
    mutable std::mutex mutex;
};
}
#endif // HKBFOOTIKCONTROLSMODIFIER_H
