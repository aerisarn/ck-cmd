#ifndef ANIMCACHEANIMATIONINFO_H
#define ANIMCACHEANIMATIONINFO_H

#include <QtCore>

class AnimCacheAnimationInfo final
{
    friend class CacheWidget;
public:
    AnimCacheAnimationInfo(const QString & path = "", const QString & name = "", bool compute = false);
    AnimCacheAnimationInfo& operator=(const AnimCacheAnimationInfo&) = default;
    AnimCacheAnimationInfo(const AnimCacheAnimationInfo &) = default;
    ~AnimCacheAnimationInfo() = default;
public:
    bool operator ==(const AnimCacheAnimationInfo &other) const;
public:
    QString getCrcAnimationName() const;
    bool read(QFile * file);
    bool write(QFile * file, QTextStream & out) const;
private:
    void setAnimationData(const QString & path, const QString & name, bool compute);
private:
    static const QString XKH;
    QString crcPath;
    QString crcAnimationName;
};

#endif // ANIMCACHEANIMATIONINFO_H
