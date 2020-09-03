#ifndef HKBBEHAVIORGRAPHSTRINGDATA_H
#define HKBBEHAVIORGRAPHSTRINGDATA_H

#include <QStringList>

#include "src/hkxclasses/hkxobject.h"

class BehaviorFile;

namespace UI {

class hkbBehaviorGraphStringData final: public HkxObject
{
    friend class hkbBehaviorGraphData;
    friend class BehaviorVariablesUI;
    friend class EventsUI;
    friend class BehaviorFile;
public:
    hkbBehaviorGraphStringData(HkxFile *parent, long ref = 0);
    hkbBehaviorGraphStringData& operator=(const hkbBehaviorGraphStringData&) = delete;
    hkbBehaviorGraphStringData(const hkbBehaviorGraphStringData &) = delete;
    ~hkbBehaviorGraphStringData();
public:
    static const QString getClassname();
    int getNumberOfEvents() const;
    int getNumberOfVariables() const;
    QString getVariableNameAt(int index) const;
    QString getEventNameAt(int index) const;
    QStringList getVariableNames() const;
    QStringList getEventNames() const;
    int getIndexOfEventName(const QString & name) const;
    int getIndexOfVariableName(const QString & name) const;
    QString getCharacterPropertyNameAt(int index) const;
    int getCharacterPropertyIndex(const QString &name) const;
    void generateAppendVariableName(const QString &type);
    void generateAppendEventName();
private:
    void setVariableNameAt(int index, const QString & name);
    void setEventNameAt(int index, const QString & name);
    bool removeEventNameAt(int index);
    void removeVariableNameAt(int index);
    int addCharacterPropertyName(const QString & name, bool * wasadded = nullptr);
    int addEventName(const QString & name, bool * wasadded = nullptr);
    bool addVariableName(const QString & name, bool * wasadded = nullptr);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    QStringList eventNames;
    QStringList attributeNames;
    QStringList variableNames;
    QStringList characterPropertyNames;
    mutable std::mutex mutex;
};
}
#endif // HKBBEHAVIORGRAPHSTRINGDATA_H
