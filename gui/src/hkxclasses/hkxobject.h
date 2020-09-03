#ifndef HKXOBJECT_H
#define HKXOBJECT_H

#include <QSharedData>

#include "src/utility.h"

class HkxXmlReader;
class HkxXMLWriter;
class BehaviorFile;
class CharacterFile;
class HkxFile;

class TreeGraphicsItem;

namespace UI {

class HkxSharedPtr;
class hkbGenerator;
class hkbVariableBindingSet;

class HkxObject: public QSharedData
{
    friend class BehaviorFile;
    friend class CharacterFile;
public:
    enum HkxType {
        TYPE_OTHER=0,
        TYPE_GENERATOR,
        TYPE_MODIFIER
    };
public:
    virtual ~HkxObject() = default;
    HkxObject& operator=(const HkxObject&) = delete;
    HkxObject(const HkxObject &obj) = delete;
public:
    void setIsWritten(bool written = true);
    int getIndexOfGenerator(const HkxSharedPtr & gen) const;
    QString getParentFilename() const;
    HkxFile * getParentFile() const;
    void setIsFileChanged(bool ischanged);
    void setReference(int ref);
    HkxSignature getSignature() const;
    QString getReferenceString() const;
    HkxType getType() const;
    bool isDataValid() const;
    long getReference() const;
    virtual bool link() = 0;
    virtual void updateReferences(long &);
    virtual bool write(HkxXMLWriter *);
    virtual bool merge(HkxObject *);
    virtual void mergeEventIndex(int, int);
    virtual void updateEventIndices(int);
    virtual void fixMergedEventIndices(BehaviorFile *);
    virtual bool fixMergedIndices(BehaviorFile *);
    virtual QString evaluateDataValidity();
    virtual void unlink();
    virtual bool readData(const HkxXmlReader &, long &);
    virtual bool isEventReferenced(int ) const;
    virtual bool isVariableReferenced(int ) const;
    virtual QVector <HkxObject *> getChildrenOtherTypes() const;
protected:
    HkxObject(HkxFile *parent, long ref = -1);
protected:
    void setDataValidity(bool isValid);
    void setType(HkxSignature sig, HkxType type);
    void setIsMerged(bool value);
    void setRefsUpdated(bool value);
    void setParentFile(HkxFile *parent);
    bool getIsWritten() const;
    bool getRefsUpdated() const;
    QString getBoolAsString(bool b) const;
    bool getIsMerged() const;
    bool readMultipleVector4(const QByteArray &lineIn,  QVector <hkQuadVariable> & vectors) const;
    bool readReferences(const QByteArray &line, QVector <HkxSharedPtr> & children) const;
    bool readIntegers(const QByteArray &line, QVector<int> & ints) const;
    bool toBool(const QByteArray &line, bool *ok) const;
    bool readDoubles(const QByteArray &line, QVector<qreal> & doubles) const;
    hkVector3 readVector3(const QByteArray &lineIn, bool *ok) const;
    hkQuadVariable readVector4(const QByteArray &lineIn, bool *ok) const;
    hkQsTransform readQsTransform(const QByteArray &lineIn, bool *ok) const;
private:
    HkxFile *parentFile;
    long reference;
    HkxSignature signature;
    HkxType typeCheck;
    bool dataValid;
    bool isWritten;
    bool isMerged;
    bool refsUpdated;
    mutable std::mutex mutex;
};

class HkxSharedPtr final: public QExplicitlySharedDataPointer <HkxObject>
{
public:
    HkxSharedPtr(HkxObject *obj = nullptr, long ref = -1);
    bool operator==(const HkxSharedPtr & other) const;
    long getShdPtrReference() const;
    bool readShdPtrReference(long index, const HkxXmlReader & reader);
private:
    void setShdPtrReference(long ref);
private:
    long smtreference;
};

struct hkEventPayload{
    hkEventPayload(): id(-1){}
    int id;
    HkxSharedPtr payload;
};

class HkDynamicObject: public HkxObject
{
public:
    ~HkDynamicObject() = default;
    HkDynamicObject& operator=(const HkDynamicObject&) = delete;
    HkDynamicObject(const HkDynamicObject &) = delete;
    bool linkVar();
    void addBinding(const QString & path, int varIndex, bool isProperty = false);
    void removeBinding(const QString & path);
    void removeBinding(int varIndex);
    HkxSharedPtr& getVariableBindingSet();
    hkbVariableBindingSet * getVariableBindingSetData() const;
    bool fixMergedIndices(BehaviorFile *dominantfile);
    void updateReferences(long &ref);
    QString evaluateDataValidity();
protected:
    void unlink();
    bool isVariableReferenced(int variableindex) const;
    bool merge(HkxObject *recessiveObject);
    void setBindingReference(int ref);
    void mergeVariableIndices(int oldindex, int newindex);
protected:
    HkDynamicObject(HkxFile *parent, long ref = -1);
private:
    HkxSharedPtr variableBindingSet;
    mutable std::mutex mutex;
};
}
#endif // HKXOBJECT_H
