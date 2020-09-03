#ifndef BSDIRECTATMODIFIER_H
#define BSDIRECTATMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class BSDirectAtModifier final: public hkbModifier
{
    friend class BSDirectAtModifierUI;
public:
    BSDirectAtModifier(HkxFile *parent, long ref = 0);
    BSDirectAtModifier& operator=(const BSDirectAtModifier&) = delete;
    BSDirectAtModifier(const BSDirectAtModifier &) = delete;
    ~BSDirectAtModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    bool getDirectAtTarget() const;
    void setDirectAtTarget(bool value);
    int getSourceBoneIndex() const;
    void setSourceBoneIndex(int value);
    int getStartBoneIndex() const;
    void setStartBoneIndex(int value);
    int getEndBoneIndex() const;
    void setEndBoneIndex(int value);
    qreal getLimitHeadingDegrees() const;
    void setLimitHeadingDegrees(const qreal &value);
    qreal getLimitPitchDegrees() const;
    void setLimitPitchDegrees(const qreal &value);
    qreal getOffsetHeadingDegrees() const;
    void setOffsetHeadingDegrees(const qreal &value);
    qreal getOffsetPitchDegrees() const;
    void setOffsetPitchDegrees(const qreal &value);
    qreal getOnGain() const;
    void setOnGain(const qreal &value);
    qreal getOffGain() const;
    void setOffGain(const qreal &value);
    hkQuadVariable getTargetLocation() const;
    void setTargetLocation(const hkQuadVariable &value);
    int getUserInfo() const;
    void setUserInfo(int value);
    bool getDirectAtCamera() const;
    void setDirectAtCamera(bool value);
    qreal getDirectAtCameraX() const;
    void setDirectAtCameraX(const qreal &value);
    qreal getDirectAtCameraY() const;
    void setDirectAtCameraY(const qreal &value);
    qreal getDirectAtCameraZ() const;
    void setDirectAtCameraZ(const qreal &value);
    bool getActive() const;
    void setActive(bool value);
    qreal getCurrentHeadingOffset() const;
    void setCurrentHeadingOffset(const qreal &value);
    qreal getCurrentPitchOffset() const;
    void setCurrentPitchOffset(const qreal &value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    bool directAtTarget;
    int sourceBoneIndex;
    int startBoneIndex;
    int endBoneIndex;
    qreal limitHeadingDegrees;
    qreal limitPitchDegrees;
    qreal offsetHeadingDegrees;
    qreal offsetPitchDegrees;
    qreal onGain;
    qreal offGain;
    hkQuadVariable targetLocation;
    int userInfo;
    bool directAtCamera;
    qreal directAtCameraX;
    qreal directAtCameraY;
    qreal directAtCameraZ;
    bool active;
    qreal currentHeadingOffset;
    qreal currentPitchOffset;
    mutable std::mutex mutex;
};
}
#endif // BSDIRECTATMODIFIER_H
