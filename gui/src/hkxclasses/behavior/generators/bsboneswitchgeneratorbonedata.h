#ifndef BSBONESWITCHGENERATORBONEDATA_H
#define BSBONESWITCHGENERATORBONEDATA_H

#include "hkbgenerator.h"
namespace UI {
class BSBoneSwitchGenerator;

class BSBoneSwitchGeneratorBoneData final: public hkbGenerator
{
    friend class BSBoneSwitchGeneratorBoneDataUI;
public:
    BSBoneSwitchGeneratorBoneData(HkxFile *parent, BSBoneSwitchGenerator *parentBSG, long ref = 0);
    BSBoneSwitchGeneratorBoneData& operator=(const BSBoneSwitchGeneratorBoneData&) = delete;
    BSBoneSwitchGeneratorBoneData(const BSBoneSwitchGeneratorBoneData &) = delete;
    ~BSBoneSwitchGeneratorBoneData();
public:
    bool operator==(const BSBoneSwitchGeneratorBoneData & other);
public:
    static const QString getClassname();
    int getThisIndex() const;
    QString getName() const;
    QVector <DataIconManager *> getChildren() const;
    bool merge(HkxObject *recessiveObject);
    void setParentBSG(BSBoneSwitchGenerator *par);
private:
    QString getPGeneratorName() const;
    HkxSharedPtr getPGenerator() const;
    HkxSharedPtr getSpBoneWeight() const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    void updateReferences(long &ref);
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    BSBoneSwitchGenerator *parentBSG;
    HkxSharedPtr pGenerator;
    HkxSharedPtr spBoneWeight;
    mutable std::mutex mutex;
};
}
#endif // BSBONESWITCHGENERATORBONEDATA_H
