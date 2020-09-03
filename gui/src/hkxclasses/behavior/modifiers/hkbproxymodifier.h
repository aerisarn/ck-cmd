#ifndef HKBPROXYMODIFIER_H
#define HKBPROXYMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbProxyModifier final: public hkbModifier
{
public:
    hkbProxyModifier(HkxFile *parent, long ref = 0);
    hkbProxyModifier& operator=(const hkbProxyModifier&) = delete;
    hkbProxyModifier(const hkbProxyModifier &) = delete;
    ~hkbProxyModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    struct hkProxyInfo{
        hkProxyInfo()
            : dynamicFriction(0),
              staticFriction(0),
              keepContactTolerance(0),
              keepDistance(0),
              contactAngleSensitivity(0),
              userPlanes(0),
              maxCharacterSpeedForSolver(0),
              characterStrength(0),
              characterMass(0),
              maxSlope(0),
              penetrationRecoverySpeed(0),
              maxCastIterations(0),
              refreshManifoldInCheckSupport(false)
        {
            //
        }
        qreal dynamicFriction;
        qreal staticFriction;
        qreal keepContactTolerance;
        hkQuadVariable up;
        qreal keepDistance;
        qreal contactAngleSensitivity;
        int userPlanes;
        qreal maxCharacterSpeedForSolver;
        qreal characterStrength;
        qreal characterMass;
        qreal maxSlope;
        qreal penetrationRecoverySpeed;
        int maxCastIterations;
        bool refreshManifoldInCheckSupport;
    };
private:
    static uint refCount;
    static const QString classname;
    static const QStringList PhantomType;
    static const QStringList LinearVelocityMode;
    long userData;
    QString name;
    bool enable;
    hkProxyInfo proxyInfo;
    hkQuadVariable linearVelocity;
    qreal horizontalGain;
    qreal verticalGain;
    qreal maxHorizontalSeparation;
    qreal limitHeadingDegrees;
    qreal maxVerticalSeparation;
    qreal verticalDisplacementError;
    qreal verticalDisplacementErrorGain;
    qreal maxVerticalDisplacement;
    qreal minVerticalDisplacement;
    qreal capsuleHeight;
    qreal capsuleRadius;
    qreal maxSlopeForRotation;
    int collisionFilterInfo;
    QString phantomType;
    QString linearVelocityMode;
    bool ignoreIncomingRotation;
    bool ignoreCollisionDuringRotation;
    bool ignoreIncomingTranslation;
    bool includeDownwardMomentum;
    bool followWorldFromModel;
    bool isTouchingGround;
    mutable std::mutex mutex;
};
}
#endif // HKBPROXYMODIFIER_H
