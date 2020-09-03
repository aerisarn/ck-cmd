#ifndef HKBMANUALSELECTORGENERATOR_H
#define HKBMANUALSELECTORGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
class hkbManualSelectorGenerator final: public hkbGenerator
{
    friend class ManualSelectorGeneratorUI;
public:
    hkbManualSelectorGenerator(HkxFile *parent, long ref = 0);
    hkbManualSelectorGenerator& operator=(const hkbManualSelectorGenerator&) = delete;
    hkbManualSelectorGenerator(const hkbManualSelectorGenerator &) = delete;
    ~hkbManualSelectorGenerator();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    qint8 getSelectedGeneratorIndex() const;
    void setSelectedGeneratorIndex(const qint8 &value);
    qint8 getCurrentGeneratorIndex() const;
    void setCurrentGeneratorIndex(const qint8 &value);
    bool swapChildren(int index1, int index2);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    bool merge(HkxObject *recessiveObject);
    QVector <DataIconManager *> getChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int index, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    QVector <HkxSharedPtr> generators;
    qint8 selectedGeneratorIndex;
    qint8 currentGeneratorIndex;
    mutable std::mutex mutex;
};
}
#endif // HKBMANUALSELECTORGENERATOR_H
