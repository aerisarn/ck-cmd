#ifndef SKYRIMANIMATIONTRANSLATION_H
#define SKYRIMANIMATIONTRANSLATION_H

#include <QtCore>

class SkyrimAnimationTranslation final
{
public:
    SkyrimAnimationTranslation(qreal x = 0, qreal y = 0, qreal z = 0, qreal w = 0);
    SkyrimAnimationTranslation& operator=(const SkyrimAnimationTranslation&) = default;
    SkyrimAnimationTranslation(const SkyrimAnimationTranslation &) = default;
    ~SkyrimAnimationTranslation() = default;
public:
    qreal localTime;
    qreal x;
    qreal y;
    qreal z;
};

#endif // SKYRIMANIMATIONTRANSLATION_H
