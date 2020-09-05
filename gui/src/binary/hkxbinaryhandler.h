#ifndef HKXBINARYREADER_H
#define HKXBINARYREADER_H

#include <QByteArray>
#include <QVector>
#include <QVector>

#include "src/utility.h"

#include <core\hkxpch.h>
#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/Reflection/Registry/hkDynamicClassNameRegistry.h>

class HkxFile;


class HkxBinaryHandler final
{
public:
	HkxBinaryHandler(HkxFile *file = nullptr);
	HkxBinaryHandler& operator=(const HkxBinaryHandler&) = delete;
	HkxBinaryHandler(const HkxBinaryHandler &) = delete;
    ~HkxBinaryHandler() = default;
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
    HkxFile *hkxBinaryFile;
	hkArray<hkVariant> objects;
};

#endif // HKXBINARYREADER_H
