#ifndef HKBGENERATOR_H
#define HKBGENERATOR_H

#include "src/hkxclasses/hkxobject.h"
#include "src/ui/dataiconmanager.h"

class TreeGraphicsItem;

namespace UI {

class hkbGenerator: public DataIconManager
{
public:
    ~hkbGenerator() = default;
    hkbGenerator& operator=(const hkbGenerator&) = delete;
    hkbGenerator(const hkbGenerator &) = delete;
public:
    bool link();
    virtual QString getName() const;
    QString getClassname() const;
protected:
    hkbGenerator(HkxFile *parent, long ref = -1);
private:
    mutable std::mutex mutex;
};
}
#endif // HKBGENERATOR_H
