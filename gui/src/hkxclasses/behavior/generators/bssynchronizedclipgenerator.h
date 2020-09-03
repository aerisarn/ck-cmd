#ifndef BSSYNCHRONIZEDCLIPGENERATOR_H
#define BSSYNCHRONIZEDCLIPGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
class BSSynchronizedClipGenerator final: public hkbGenerator
{
    friend class BSSynchronizedClipGeneratorUI;
public:
    BSSynchronizedClipGenerator(HkxFile *parent, long ref = 0);
    BSSynchronizedClipGenerator& operator=(const BSSynchronizedClipGenerator&) = delete;
    BSSynchronizedClipGenerator(const BSSynchronizedClipGenerator &) = delete;
    ~BSSynchronizedClipGenerator();
public:
    QString getName() const;
    static const QString getClassname();
private:
    QString getClipGeneratorName() const;
    void setName(const QString &newname);
    QString getSyncAnimPrefix() const;
    void setSyncAnimPrefix(const QString &value);
    bool getBSyncClipIgnoreMarkPlacement() const;
    void setBSyncClipIgnoreMarkPlacement(bool value);
    qreal getFGetToMarkTime() const;
    void setFGetToMarkTime(const qreal &value);
    qreal getFMarkErrorThreshold() const;
    void setFMarkErrorThreshold(const qreal &value);
    bool getBLeadCharacter() const;
    void setBLeadCharacter(bool value);
    bool getBReorientSupportChar() const;
    void setBReorientSupportChar(bool value);
    bool getBApplyMotionFromRoot() const;
    void setBApplyMotionFromRoot(bool value);
    int getSAnimationBindingIndex() const;
    void setSAnimationBindingIndex(int value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    QVector <DataIconManager *> getChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    ulong userData;
    QString name;
    HkxSharedPtr pClipGenerator;
    QString syncAnimPrefix;
    bool bSyncClipIgnoreMarkPlacement;
    qreal fGetToMarkTime;
    qreal fMarkErrorThreshold;
    bool bLeadCharacter;
    bool bReorientSupportChar;
    bool bApplyMotionFromRoot;
    int sAnimationBindingIndex;
    mutable std::mutex mutex;
};
}
#endif // BSSYNCHRONIZEDCLIPGENERATOR_H
