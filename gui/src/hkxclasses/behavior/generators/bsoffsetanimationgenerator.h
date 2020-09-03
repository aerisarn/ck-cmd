#ifndef BSOFFSETANIMATIONGENERATOR_H
#define BSOFFSETANIMATIONGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
class BSOffsetAnimationGenerator final: public hkbGenerator
{
    friend class BSOffsetAnimationGeneratorUI;
public:
    BSOffsetAnimationGenerator(HkxFile *parent, long ref = 0);
    BSOffsetAnimationGenerator& operator=(const BSOffsetAnimationGenerator&) = delete;
    BSOffsetAnimationGenerator(const BSOffsetAnimationGenerator &) = delete;
    ~BSOffsetAnimationGenerator();
public:
    QString getName() const;
    static const QString getClassname();
private:
    QString getDefaultGeneratorName() const;
    QString getOffsetClipGeneratorName() const;
    void setName(const QString &newname);
    qreal getFOffsetVariable() const;
    void setFOffsetVariable(const qreal &value);
    qreal getFOffsetRangeStart() const;
    void setFOffsetRangeStart(const qreal &value);
    qreal getFOffsetRangeEnd() const;
    void setFOffsetRangeEnd(const qreal &value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    QVector <DataIconManager *> getChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int index, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    ulong userData;
    QString name;
    HkxSharedPtr pDefaultGenerator;
    HkxSharedPtr pOffsetClipGenerator;
    qreal fOffsetVariable;
    qreal fOffsetRangeStart;
    qreal fOffsetRangeEnd;
    mutable std::mutex mutex;
};
}
#endif // BSOFFSETANIMATIONGENERATOR_H
