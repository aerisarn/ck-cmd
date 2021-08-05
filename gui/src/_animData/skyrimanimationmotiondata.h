#ifndef SKYRIMANIMATIONMOTIONDATA_H
#define SKYRIMANIMATIONMOTIONDATA_H

#include <QTextStream>

#include "skyrimanimationtranslation.h"
#include "skyrimanimationrotation.h"

class ProjectAnimData;
class QFile;

namespace UI {
	class SkyrimAnimationDataUI;
}

class SkyrimAnimationMotionData final
{
    friend class UI::SkyrimAnimationDataUI;
    friend class ProjectAnimData;
public:
    SkyrimAnimationMotionData(ProjectAnimData *par, uint ind = 0, qreal dur = 0, const QVector <SkyrimAnimationTranslation> & trans = QVector <SkyrimAnimationTranslation>(), const QVector <SkyrimAnimationRotation> & rots = QVector <SkyrimAnimationRotation>());
    SkyrimAnimationMotionData& operator=(const SkyrimAnimationMotionData&) = default;
    SkyrimAnimationMotionData(const SkyrimAnimationMotionData &) = default;
    ~SkyrimAnimationMotionData() = default;
public:
    bool operator==(const SkyrimAnimationMotionData & other) const;
public:
    void setAnimationIndex(const uint &value);
    void setDuration(const qreal &value);
private:
    bool read(QFile * file, ulong &lineCount);
    bool write(QFile * file, QTextStream & out) const;
    uint lineCount() const;
    void addTranslation(const SkyrimAnimationTranslation & trans = SkyrimAnimationTranslation());
    bool removeTranslation(int index);
    void addRotation(const SkyrimAnimationRotation & rot = SkyrimAnimationRotation());
    bool removeRotation(int index);
private:
    ProjectAnimData *parent;
    uint animationIndex;
    qreal duration;
    QVector <SkyrimAnimationTranslation> translations;
    QVector <SkyrimAnimationRotation> rotations;
};

#endif // SKYRIMANIMATIONMOTIONDATA_H
