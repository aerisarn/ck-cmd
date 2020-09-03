#ifndef HKBPROJECTSTRINGDATA_H
#define HKBPROJECTSTRINGDATA_H

#include <QStringList>

#include "src/hkxclasses/hkxobject.h"

class HkxFile;

namespace UI {

class hkbProjectStringData final: public HkxObject
{
public:
    hkbProjectStringData(HkxFile *parent, long ref = 0, const QString & characterfilename = "");
    hkbProjectStringData& operator=(const hkbProjectStringData&) = delete;
    hkbProjectStringData(const hkbProjectStringData &) = delete;
    ~hkbProjectStringData();
public:
    static const QString getClassname();
    QString getCharacterFilePathAt(int index) const;
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    QStringList animationFilenames;
    QStringList behaviorFilenames;
    QStringList characterFilenames;
    QStringList eventNames;
    QString animationPath;
    QString behaviorPath;
    QString characterPath;
    QString fullPathToSource;
    mutable std::mutex mutex;
};
}
#endif // HKBPROJECTSTRINGDATA_H
