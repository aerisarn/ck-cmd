#ifndef HKBVARIABLEBINDINGSET_H
#define HKBVARIABLEBINDINGSET_H

#include "src/hkxclasses/hkxobject.h"

class BehaviorFile;

namespace UI {

class hkbVariableBindingSet: public HkxObject
{
    friend class HkDynamicObject;
public:
    struct hkBinding
    {
        enum BindingType {
            BINDING_TYPE_VARIABLE=0,
            BINDING_TYPE_CHARACTER_PROPERTY=1
        };
        hkBinding(const QString & path = "", int varIndex = -1, int bit = -1, BindingType type = BINDING_TYPE_VARIABLE)
            : memberPath(path), variableIndex(varIndex), bitIndex(bit), bindingType(type){}
        QString memberPath;
        int variableIndex;
        int bitIndex;
        BindingType bindingType;
    };
public:
    hkbVariableBindingSet(HkxFile *parent, long ref = -1);
    hkbVariableBindingSet& operator=(const hkbVariableBindingSet&) = delete;
    hkbVariableBindingSet(const hkbVariableBindingSet &) = delete;
    ~hkbVariableBindingSet();
public:
    static const QString getClassname();
    int getVariableIndexOfBinding(const QString & path) const;
    hkBinding::BindingType getBindingType(int index) const;
    hkBinding::BindingType getBindingType(const QString & path) const;
    int getNumberOfBindings() const;
    bool isVariableRefed(int variableindex) const;
    void removeBinding(const QString & path);
    bool addBinding(const QString & path, int varIndex, hkBinding::BindingType type = hkBinding::BINDING_TYPE_VARIABLE);
    void removeBinding(int varIndex);
    void updateVariableIndices(int index);
    void mergeVariableIndex(int oldindex, int newindex);
    bool merge(HkxObject *recessiveObject);
private:
    bool link();
    QString evaluateDataValidity();
    bool readData(const HkxXmlReader & reader, long & index);
    QString getPathOfBindingAt(int index);
    bool write(HkxXMLWriter *writer);
    bool fixMergedIndices(BehaviorFile *dominantfile);
private:
    static uint refCount;
    static const QString classname;
    QVector <hkBinding> bindings;
    int indexOfBindingToEnable;
    mutable std::mutex mutex;
};
}
#endif // HKBVARIABLEBINDINGSET_H
