#ifndef ANIMCACHECLIPINFO_H
#define ANIMCACHECLIPINFO_H

#include <QtCore>

class AnimCacheClipInfo final
{
    friend class CacheClipWidget;
public:
    AnimCacheClipInfo(const QString & eventname = "", const QStringList & clipgens = QStringList(), uint unk = 0);
    AnimCacheClipInfo& operator=(const AnimCacheClipInfo&) = default;
    AnimCacheClipInfo(const AnimCacheClipInfo & other);
    ~AnimCacheClipInfo() = default;
public:
    bool operator ==(const AnimCacheClipInfo & other) const;
public:
    bool read(QFile * file);
    bool write(QFile * file, QTextStream & out) const;
    void removeClipGenerator(const QString & clipname);
    QString getEventName() const;
private:
    QString eventName;
    uint unknown;   //Appears to be 1 for sprinting and normal attacks, zero otherwise in defaultmale...
    QStringList clipGenerators;
};

#endif // ANIMCACHECLIPINFO_H
