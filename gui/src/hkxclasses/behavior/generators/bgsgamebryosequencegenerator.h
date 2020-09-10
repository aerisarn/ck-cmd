#ifndef BGSGAMEBRYOSEQUENCEGENERATOR_H
#define BGSGAMEBRYOSEQUENCEGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
class BGSGamebryoSequenceGenerator final: public hkbGenerator
{
    friend class BGSGamebryoSequenceGeneratorUI;
public:
    BGSGamebryoSequenceGenerator(HkxFile *parent, long ref = 0);
    BGSGamebryoSequenceGenerator& operator=(const BGSGamebryoSequenceGenerator&) = delete;
    BGSGamebryoSequenceGenerator(const BGSGamebryoSequenceGenerator &) = delete;
    ~BGSGamebryoSequenceGenerator();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    void setPSequence(const QString &value);
    void setEBlendModeFunction(int index);
    void setFPercent(const qreal &value);
    QString getPSequence() const;
    QString getEBlendModeFunction() const;
    qreal getFPercent() const;
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
	virtual bool readData(const HkxBinaryHandler&, const void*);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
private:
    static const QStringList BlendModeFunction;
    static const QString classname;
    static uint refCount;
    ulong userData;
    QString name;
    QString pSequence;
    QString eBlendModeFunction;
    qreal fPercent;
    mutable std::mutex mutex;
};
}
#endif // BGSGAMEBRYOSEQUENCEGENERATOR_H
