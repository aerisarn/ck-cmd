#ifndef BSBONESWITCHGENERATOR_H
#define BSBONESWITCHGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
class BSBoneSwitchGenerator final: public hkbGenerator
{
    friend class BSBoneSwitchGeneratorUI;
public:
    BSBoneSwitchGenerator(HkxFile *parent, long ref = 0);
    BSBoneSwitchGenerator& operator=(const BSBoneSwitchGenerator&) = delete;
    BSBoneSwitchGenerator(const BSBoneSwitchGenerator &) = delete;
    ~BSBoneSwitchGenerator();
public:
    static const QString getClassname();
    bool hasGenerator() const;
    QString getDefaultGeneratorName() const;
    QString getName() const;
    int getIndexOfObj(DataIconManager *obj) const;
private:
    bool swapChildren(int index1, int index2);
    void setName(const QString &newname);
    void updateChildIconNames() const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    bool merge(HkxObject *recessiveObject);
    QVector <DataIconManager *> getChildren() const;
    bool insertObjectAt(int index, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static const QString classname;
    static uint refCount;
    long userData;
    QString name;
    HkxSharedPtr pDefaultGenerator;
    QVector <HkxSharedPtr> ChildrenA;
    mutable std::mutex mutex;
};
}
#endif // BSBONESWITCHGENERATOR_H
