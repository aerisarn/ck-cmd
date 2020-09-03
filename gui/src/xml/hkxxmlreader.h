#ifndef HKXXMLREADER_H
#define HKXXMLREADER_H

#include <QByteArray>
#include <QVector>
#include <QVector>

#include "src/utility.h"


class HkxFile;


class HkxXmlReader final
{
public:
    HkxXmlReader(HkxFile *file = nullptr);
    HkxXmlReader& operator=(const HkxXmlReader&) = delete;
    HkxXmlReader(const HkxXmlReader &) = delete;
    ~HkxXmlReader() = default;
public:
    void setFile(HkxFile *file);
    bool parse();
    void clear();
    int getNumElements() const;
    int getNumAttributesAt(int index) const;
    QByteArray getElementNameAt(int index) const;
    QByteArray getElementValueAt(int index) const;
    QByteArray getNthAttributeNameAt(int index, int nth) const;
    QByteArray getNthAttributeValueAt(int index, int nth) const;
    QByteArray findFirstValueWithAttributeValue(const QString & attributevalue) const;
private:
    enum HkxXmlParseLine {
        NoError,
        //NullPointerArgs,
        EmptyLineEndFile,
        OrphanedCharacter,
        InvalidElementName,
        InvalidElementValue,
        //ElementValueSplitOnMutipleLines,
        InvalidAttributeName,
        InvalidAttributeValue,
        OrphanedAttribute,
        MalformedAttribute,
        OrphanedElementTag,
        MalformedComment,
        UnexpectedNewlineCharacter,
        EmptyLine,
        MalformedEndElementTag,
        UnknownError
    };
    enum {
        AVERAGE_ATTRIBUTE_LENGTH = 5,
        AVERAGE_ELEMENT_TAG_LENGTH = 9,
        AVERAGE_VALUE_LENGTH = 9,
        AVERAGE_ATTRIBUTE_VALUE_LENGTH = 14
    };
private:
    HkxXmlParseLine readNextLine();
    int skipComment(const QByteArray & line, int index);
    int readValue(const QByteArray & line, int startIndex, bool isSplitOnMultipleLines);
    int readAttribute(const QByteArray & line, int startIndex);
    int readElementTag(const QByteArray & line, int startIndex, bool isIsolatedEndElemTag, bool isEndTag);
private:
    struct Attribute{
        Attribute(const QByteArray & elem): name(elem){}
        QByteArray name;
        QByteArray value;
    };
    struct Element{
        Element(const QByteArray & elem): name(elem), isContainedOnOneLine(true){}
        QByteArray name;
        QByteArray value;
        QList <Attribute> attributeList;
        bool isContainedOnOneLine;
    };
private:
    HkxFile *hkxXmlFile;
    QList <Element> elementList;
    QVector <long> indexOfElemTags;
    ulong lineNumber;
    bool isEOF;
};

#endif // HKXXMLREADER_H
