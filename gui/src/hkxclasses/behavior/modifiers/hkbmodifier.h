#ifndef HKBMODIFIER_H
#define HKBMODIFIER_H

#include "src/hkxclasses/hkxobject.h"
#include "src/ui/dataiconmanager.h"

class TreeGraphicsItem;

namespace UI {

class hkbModifier: public DataIconManager
{
public:
    ~hkbModifier() = default;
    hkbModifier& operator=(const hkbModifier&) = delete;
    hkbModifier(const hkbModifier &) = delete;
public:
    bool link();
    virtual QString getName() const;
    QString getClassname() const;
protected:
    hkbModifier(HkxFile *parent, long ref = -1);
};
};
#endif // HKBMODIFIER_H
