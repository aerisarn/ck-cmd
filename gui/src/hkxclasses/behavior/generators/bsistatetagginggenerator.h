#ifndef BSISTATETAGGINGGENERATOR_H
#define BSISTATETAGGINGGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
class BSiStateTaggingGenerator final: public hkbGenerator
{
    friend class BSiStateTaggingGeneratorUI;
public:
    BSiStateTaggingGenerator(HkxFile *parent, long ref = 0);
    BSiStateTaggingGenerator& operator=(const BSiStateTaggingGenerator&) = delete;
    BSiStateTaggingGenerator(const BSiStateTaggingGenerator &) = delete;
    ~BSiStateTaggingGenerator();
public:
    QVector <DataIconManager *> getChildren() const;
    static const QString getClassname();
    QString getName() const;
private:
    void setName(const QString &newname);
    void setIStateToSetAs(int value);
    void setIPriority(int value);
    QString getDefaultGeneratorName() const;
    int getIStateToSetAs() const;
    int getIPriority() const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    ulong userData;
    QString name;
    HkxSharedPtr pDefaultGenerator;
    int iStateToSetAs;
    int iPriority;
    mutable std::mutex mutex;
};
}
#endif // BSISTATETAGGINGGENERATOR_H
