#ifndef HKXXMLWRITER_H
#define HKXXMLWRITER_H

#include "src/utility.h"

#include <QStringList>
#include <QFile>
#include <QTextStream>

class HkxFile;

class HkxXMLWriter final
{
    friend class BehaviorFile;
public:
    HkxXMLWriter(HkxFile *file = nullptr);
    HkxXMLWriter& operator=(const HkxXMLWriter&) = delete;
    HkxXMLWriter(const HkxXMLWriter &) = delete;
    ~HkxXMLWriter() = default;
public:
    bool writeToXMLFile();
    void setFile(HkxFile *file);
    void writeHeader(const QString & version, const QString & encoding);
    bool writeLine(const QString & tag, const QStringList & attribs, const QStringList & attribValues, const QString & value, bool nullValueAllowed = false);
    bool writeLine(const QString & tag, bool opening);
    bool writeLine(const QString & value);
public:
    static const QString version;
    static const QString encoding;
    static const QString classversion;
    static const QString contentsversion;
    static const QString toplevelobject;
    static const QString filetype;
    static const QString section;
    static const QString object;
    static const QString parameter;
    static const QString string;
    static const QString name;
    static const QString clas;
    static const QString signature;
    static const QString numelements;
private:
    HkxFile *hkxXmlFile;
    QFile newfile;
    QTextStream stream;
    int nestLevel;
};

#endif // HKXXMLWRITER_H
