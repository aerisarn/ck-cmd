#ifndef ANIMCACHEVARIABLE_H
#define ANIMCACHEVARIABLE_H

#include <QtCore>

class AnimCacheVariable final
{
    friend class CacheVariableWidget;
public:
    AnimCacheVariable(const QString & name = "", uint val1 = 0, uint val2 = 0);
    AnimCacheVariable& operator=(const AnimCacheVariable&) = default;
    AnimCacheVariable(const AnimCacheVariable & other);
    ~AnimCacheVariable() = default;
public:
    bool operator ==(const AnimCacheVariable & other) const;
public:
    QString getName() const;
    bool read(QFile * file);
    bool write(QFile * file, QTextStream & out) const;
private:
    QString name;
    uint minValue;
    uint maxValue;
};

#endif // ANIMCACHEVARIABLE_H
