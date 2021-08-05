#ifndef SKYRIMANIMATIONROTATION_H
#define SKYRIMANIMATIONROTATION_H

#include <QtCore>

class SkyrimAnimationRotation final
{
public:
    SkyrimAnimationRotation(qreal time = 0, qreal c2 = 0, qreal c3 = 0, qreal c4 = 0, qreal c5 = 0);
    SkyrimAnimationRotation& operator=(const SkyrimAnimationRotation&) = default;
    SkyrimAnimationRotation(const SkyrimAnimationRotation &) = default;
    ~SkyrimAnimationRotation() = default;
public:
    qreal localTime;
    float x;
    float y;
    float z;
    float w;
};

#endif // SKYRIMANIMATIONROTATION_H
