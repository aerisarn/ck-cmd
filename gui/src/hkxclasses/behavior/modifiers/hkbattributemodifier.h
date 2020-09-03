#ifndef HKBATTRIBUTEMODIFIER_H
#define HKBATTRIBUTEMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbAttributeModifier final: public hkbModifier
{
public:
    hkbAttributeModifier(HkxFile *parent, long ref = 0);
    hkbAttributeModifier& operator=(const hkbAttributeModifier&) = delete;
    hkbAttributeModifier(const hkbAttributeModifier &) = delete;
    ~hkbAttributeModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    struct hkAttributeMod{
        hkAttributeMod()
            : attributeIndex(-1),
              attributeValue(0)
        {
            //
        }

        int attributeIndex;
        qreal attributeValue;
    };
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    QVector <hkAttributeMod> assignments;
    mutable std::mutex mutex;
};
}
#endif // HKBATTRIBUTEMODIFIER_H
