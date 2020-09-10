#ifndef HKXFILE_H
#define HKXFILE_H

#include "src/utility.h"
#include "src/xml/hkxxmlreader.h"
#include "src/xml/hkxxmlwriter.h"
#include "src/binary/hkxbinaryhandler.h"
#include "src/hkxclasses/hkxobject.h"

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/Reflection/Registry/hkDynamicClassNameRegistry.h>

#include <QFile>

#include <core\hkxpch.h>
#include <core/hkxcmd.h>
#include <core/hkfutils.h>

class MainWindow;



	class HkxFile : public QFile
	{
	public:
		HkxFile & operator=(const HkxFile&) = delete;
		HkxFile(const HkxFile &) = delete;
		virtual ~HkxFile() = default;
	public:
		void closeFile();
		virtual bool addObjectToFile(UI::HkxObject *obj, long ref) = 0;
		QString getRootObjectReferenceString() const;
		bool getIsChanged() const;
		QString getFileName() const;
		void setIsChanged(bool wasEdited);
		void setHKXFileName(const QString &name);
		UI::HkxSharedPtr & getRootObject();
	protected:
		HkxFile(MainWindow *window, const QString & name);
		MainWindow *getUI() const;
		virtual bool parse();
		virtual bool parseBinary() = 0;
		virtual bool link();
		void setRootObject(UI::HkxSharedPtr & obj);
		bool appendAndReadData(long & index, UI::HkxObject * obj);
		HkxXmlReader & getReader();
		HkxXMLWriter & getWriter();
		HkxBinaryHandler & getBinaryHandler();

	private:
		MainWindow * ui;
		UI::HkxSharedPtr rootObject;
		HkxXmlReader reader;
		HkxXMLWriter writer;
		HkxBinaryHandler binaryHandler;

		bool changed;
		QString fileNameWithoutPath;
		mutable std::mutex mutex;
	};

#endif // HKXFILE_H
