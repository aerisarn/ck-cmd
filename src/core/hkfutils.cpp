#include "stdafx.h"

#include <core/hkfutils.h>
#include <core/log.h>

EnumLookupType SaveFlags[] = 
{
   {hkSerializeUtil::SAVE_DEFAULT,                   "SAVE_DEFAULT"},
   {hkSerializeUtil::SAVE_TEXT_FORMAT,               "SAVE_TEXT_FORMAT"},
   {hkSerializeUtil::SAVE_SERIALIZE_IGNORED_MEMBERS, "SAVE_SERIALIZE_IGNORED_MEMBERS"},
   {hkSerializeUtil::SAVE_WRITE_ATTRIBUTES,          "SAVE_WRITE_ATTRIBUTES"},
   {hkSerializeUtil::SAVE_CONCISE,                   "SAVE_CONCISE"},
   {hkSerializeUtil::SAVE_TEXT_NUMBERS,              "SAVE_TEXT_NUMBERS"},
   {0, NULL}
};

EnumLookupType PackFlags[] = 
{
   {HKPF_XML,   "XML"},
   {HKPF_DEFAULT, "DEFAULT"},
   {HKPF_WIN32, "WIN32"},
   {HKPF_AMD64, "AMD64"},
   {HKPF_XBOX,  "XBOX"},
   {HKPF_XBOX360, "XBOX360"},
   {HKPF_TAGFILE, "TAGFILE"},
   {HKPF_TAGXML, "TAGXML"},
   {0, NULL}
};

EnumLookupType LogFlags[] = 
{
   {LOG_NONE,   "NONE"},
   {LOG_ALL,    "ALL"},
   {LOG_VERBOSE,"VERBOSE"},
   {LOG_DEBUG,  "DEBUG"},
   {LOG_INFO,   "INFO"},
   {LOG_WARN,   "WARN"},
   {LOG_ERROR,  "ERROR"},
   {0, NULL}
};


EnumLookupType TypeEnums[] = 
{
   {hkClassMember::TYPE_VOID, 	"hkClassMember::TYPE_VOID"},
   {hkClassMember::TYPE_BOOL, 	"hkClassMember::TYPE_BOOL"},
   {hkClassMember::TYPE_CHAR, 	"hkClassMember::TYPE_CHAR"},
   {hkClassMember::TYPE_INT8, 	"hkClassMember::TYPE_INT8"},
   {hkClassMember::TYPE_UINT8, 	"hkClassMember::TYPE_UINT8"},
   {hkClassMember::TYPE_INT16, 	"hkClassMember::TYPE_INT16"},
   {hkClassMember::TYPE_UINT16, 	"hkClassMember::TYPE_UINT16"},
   {hkClassMember::TYPE_INT32, 	"hkClassMember::TYPE_INT32"},
   {hkClassMember::TYPE_UINT32, 	"hkClassMember::TYPE_UINT32"},
   {hkClassMember::TYPE_INT64, 	"hkClassMember::TYPE_INT64"},
   {hkClassMember::TYPE_UINT64, 	"hkClassMember::TYPE_UINT64"},
   {hkClassMember::TYPE_REAL, 	"hkClassMember::TYPE_REAL"},
   {hkClassMember::TYPE_VECTOR4, 	"hkClassMember::TYPE_VECTOR4"},
   {hkClassMember::TYPE_QUATERNION, 	"hkClassMember::TYPE_QUATERNION"},
   {hkClassMember::TYPE_MATRIX3, 	"hkClassMember::TYPE_MATRIX3"},
   {hkClassMember::TYPE_ROTATION, 	"hkClassMember::TYPE_ROTATION"},
   {hkClassMember::TYPE_QSTRANSFORM, 	"hkClassMember::TYPE_QSTRANSFORM"},
   {hkClassMember::TYPE_MATRIX4, 	"hkClassMember::TYPE_MATRIX4"},
   {hkClassMember::TYPE_TRANSFORM, 	"hkClassMember::TYPE_TRANSFORM"},
   {hkClassMember::TYPE_ZERO, 	"hkClassMember::TYPE_ZERO"},
   {hkClassMember::TYPE_POINTER, 	"hkClassMember::TYPE_POINTER"},
   {hkClassMember::TYPE_FUNCTIONPOINTER, 	"hkClassMember::TYPE_FUNCTIONPOINTER"},
   {hkClassMember::TYPE_ARRAY, 	"hkClassMember::TYPE_ARRAY"},
   {hkClassMember::TYPE_INPLACEARRAY, 	"hkClassMember::TYPE_INPLACEARRAY"},
   {hkClassMember::TYPE_ENUM, 	"hkClassMember::TYPE_ENUM"},
   {hkClassMember::TYPE_STRUCT, 	"hkClassMember::TYPE_STRUCT"},
   {hkClassMember::TYPE_SIMPLEARRAY, 	"hkClassMember::TYPE_SIMPLEARRAY"},
   {hkClassMember::TYPE_HOMOGENEOUSARRAY, 	"hkClassMember::TYPE_HOMOGENEOUSARRAY"},
   {hkClassMember::TYPE_VARIANT, 	"hkClassMember::TYPE_VARIANT"},
   {hkClassMember::TYPE_CSTRING, 	"hkClassMember::TYPE_CSTRING"},
   {hkClassMember::TYPE_ULONG, 	"hkClassMember::TYPE_ULONG"},
   {hkClassMember::TYPE_FLAGS, 	"hkClassMember::TYPE_FLAGS"},
   {hkClassMember::TYPE_HALF, 	"hkClassMember::TYPE_HALF"},
   {hkClassMember::TYPE_STRINGPTR, 	"hkClassMember::TYPE_STRINGPTR"},
   {hkClassMember::TYPE_RELARRAY, 	"hkClassMember::TYPE_RELARRAY"},
   {0, NULL}
};

EnumLookupType CTypeEnums[] = 
{
   {hkClassMember::TYPE_VOID, 	"hkRefVariant"},
   {hkClassMember::TYPE_BOOL, 	"hkBool"},
   {hkClassMember::TYPE_CHAR, 	"hkChar"},
   {hkClassMember::TYPE_INT8, 	"hkInt8"},
   {hkClassMember::TYPE_UINT8, 	"hkUint8"},
   {hkClassMember::TYPE_INT16, 	"hkInt16"},
   {hkClassMember::TYPE_UINT16, 	"hkUint16"},
   {hkClassMember::TYPE_INT32, 	"hkInt32"},
   {hkClassMember::TYPE_UINT32, 	"hkUint32"},
   {hkClassMember::TYPE_INT64, 	"hkInt64"},
   {hkClassMember::TYPE_UINT64, 	"hkUint64"},
   {hkClassMember::TYPE_REAL, 	"hkReal"},
   {hkClassMember::TYPE_VECTOR4, 	"hkVector4"},
   {hkClassMember::TYPE_QUATERNION, 	"hkQuaternion"},
   {hkClassMember::TYPE_MATRIX3, 	"hkMatrix3"},
   {hkClassMember::TYPE_ROTATION, 	"hkRotation"},
   {hkClassMember::TYPE_QSTRANSFORM, 	"hkQsTransform"},
   {hkClassMember::TYPE_MATRIX4, 	"hkMatrix4"},
   {hkClassMember::TYPE_TRANSFORM, 	"hkTransform"},
   {hkClassMember::TYPE_ENUM, 	"enum unknown"},
   {hkClassMember::TYPE_VARIANT, 	"hkRefVariant"},
   {hkClassMember::TYPE_CSTRING, 	"char*"},
   {hkClassMember::TYPE_ULONG, 	"hkUlong"},
   {hkClassMember::TYPE_FLAGS, 	"hkFlags"},
   {hkClassMember::TYPE_HALF, 	"hkHalf"},
   {hkClassMember::TYPE_STRINGPTR, 	"hkStringPtr"},
   {0, NULL}
};


EnumLookupType TypeFlags[] = 
{
   {hkClassMember::FLAGS_NONE, 	"hkClassMember::FLAGS_NONE"},
   {hkClassMember::ALIGN_8, 	"hkClassMember::ALIGN_8"},
   {hkClassMember::ALIGN_16, 	"hkClassMember::ALIGN_16"},
   {hkClassMember::NOT_OWNED, 	"hkClassMember::NOT_OWNED"},
   {hkClassMember::SERIALIZE_IGNORED, 	"hkClassMember::SERIALIZE_IGNORED"},
   {0, NULL}
};


hkPackfileWriter::Options GetWriteOptionsFromFormat(hkPackFormat format)
{
   hkPackfileWriter::Options options;
   options.m_layout = hkStructureLayout::MsvcWin32LayoutRules;

   switch(format)
   {
   case HKPF_XML:
   case HKPF_DEFAULT:
   case HKPF_WIN32:
      options.m_layout = hkStructureLayout::MsvcWin32LayoutRules;
      break;
   case HKPF_AMD64:
      options.m_layout = hkStructureLayout::MsvcAmd64LayoutRules;
      break;
   case HKPF_XBOX:
      options.m_layout = hkStructureLayout::MsvcXboxLayoutRules;
      break;
   case HKPF_XBOX360:
      options.m_layout = hkStructureLayout::Xbox360LayoutRules;
      break;
   }
   return options;
}

void HK_CALL errorReport(const char* msg, void* userContext)
{
   Log::Error("%s", msg);
}


hkResource* hkSerializeUtilLoad( hkStreamReader* stream
                                , hkSerializeUtil::ErrorDetails* detailsOut/*=HK_NULL*/
                                , const hkClassNameRegistry* classReg/*=HK_NULL*/
                                , hkSerializeUtil::LoadOptions options/*=hkSerializeUtil::LOAD_DEFAULT*/ )
{
   __try
   {
      return hkSerializeUtil::load(stream, detailsOut, classReg, options);
   }
   __except (EXCEPTION_EXECUTE_HANDLER)
   {
      if (detailsOut == NULL)
         detailsOut->id = hkSerializeUtil::ErrorDetails::ERRORID_LOAD_FAILED;
      return NULL;
   }
}


hkResult hkSerializeUtilSave( hkPackFormat pkFormat, hkVariant &root, hkOstream &stream
                         , hkSerializeUtil::SaveOptionBits flags
                         , const hkPackfileWriter::Options& packFileOptions )
{
   hkResult res;
   __try
   {
      if (pkFormat == HKPF_TAGXML || pkFormat == HKPF_TAGFILE)
      {
         //res = hkSerializeUtil::save( root.m_object, *root.m_class, stream.getStreamWriter(), flags );
         res = hkSerializeUtil::saveTagfile(root.m_object, *root.m_class, stream.getStreamWriter(), HK_NULL, flags );
      }
      else
      {
         res = hkSerializeUtil::savePackfile(root.m_object, *root.m_class, stream.getStreamWriter(), packFileOptions, HK_NULL, flags );
      }
   }
   __except (EXCEPTION_EXECUTE_HANDLER)
   {
      res = HK_FAILURE;
   }
   return res;
}

hkResult LoadDefaultRegistry()
{
   hkVersionPatchManager patchManager;
   {
      extern void HK_CALL CustomRegisterPatches(hkVersionPatchManager& patchManager);
      CustomRegisterPatches(patchManager);
   } 
   hkDefaultClassNameRegistry &defaultRegistry = hkDefaultClassNameRegistry::getInstance();
   {
      extern void HK_CALL CustomRegisterDefaultClasses();
      extern void HK_CALL ValidateClassSignatures();
      CustomRegisterDefaultClasses();
      ValidateClassSignatures();
   }
   return HK_SUCCESS;
}

hkResult hkSerializeLoad(hkStreamReader *reader
                                , hkVariant &root
                                , hkResource *&resource)
{
   hkTypeInfoRegistry &defaultTypeRegistry = hkTypeInfoRegistry::getInstance();
   hkDefaultClassNameRegistry &defaultRegistry = hkDefaultClassNameRegistry::getInstance();

   hkBinaryPackfileReader bpkreader;
   hkXmlPackfileReader xpkreader;
   resource = NULL;
   hkSerializeUtil::FormatDetails formatDetails;
   hkSerializeUtil::detectFormat( reader, formatDetails );
   hkBool32 isLoadable = hkSerializeUtil::isLoadable( reader );
   if (!isLoadable && formatDetails.m_formatType != hkSerializeUtil::FORMAT_TAGFILE_XML)
   {
      return HK_FAILURE;
   }
   else
   {
      switch ( formatDetails.m_formatType )
      {
      case hkSerializeUtil::FORMAT_PACKFILE_BINARY:
         {
            bpkreader.loadEntireFile(reader);
            bpkreader.finishLoadedObjects(defaultTypeRegistry);
            if ( hkPackfileData* pkdata = bpkreader.getPackfileData() )
            {
               hkArray<hkVariant>& obj = bpkreader.getLoadedObjects();
               for ( int i =0,n=obj.getSize(); i<n; ++i)
               {
                  hkVariant& value = obj[i];
                  if ( value.m_class->hasVtable() )
                     defaultTypeRegistry.finishLoadedObject(value.m_object, value.m_class->getName());
               }
               resource = pkdata;
               resource->addReference();
            }
            root = bpkreader.getTopLevelObject();
         }
         break;

      case hkSerializeUtil::FORMAT_PACKFILE_XML:
         {
            xpkreader.loadEntireFileWithRegistry(reader, &defaultRegistry);
            if ( hkPackfileData* pkdata = xpkreader.getPackfileData() )
            {
               hkArray<hkVariant>& obj = xpkreader.getLoadedObjects();
               for ( int i =0,n=obj.getSize(); i<n; ++i)
               {
                  hkVariant& value = obj[i];
                  if ( value.m_class->hasVtable() )
                     defaultTypeRegistry.finishLoadedObject(value.m_object, value.m_class->getName());
               }
               resource = pkdata;
               resource->addReference();
               root = xpkreader.getTopLevelObject();
            }
         }
         break;

      case hkSerializeUtil::FORMAT_TAGFILE_BINARY:
      case hkSerializeUtil::FORMAT_TAGFILE_XML:
      default:
         {
            hkSerializeUtil::ErrorDetails detailsOut;
            hkSerializeUtil::LoadOptions loadflags = hkSerializeUtil::LOAD_FAIL_IF_VERSIONING;
            resource = hkSerializeUtilLoad(reader, &detailsOut, &defaultRegistry, loadflags);
            root.m_object = resource->getContents<hkRootLevelContainer>();
            if (root.m_object != NULL)
               root.m_class = &((hkRootLevelContainer*)root.m_object)->staticClass();
         }
         break;
      }
   }
   return root.m_object != NULL ? HK_SUCCESS : HK_FAILURE;
}

hkResult hkSerializeLoad(hkStreamReader *reader
	, hkVariant &root
	, hkResource *&resource, hkArray<hkVariant>& obj)
{
	hkTypeInfoRegistry &defaultTypeRegistry = hkTypeInfoRegistry::getInstance();
	hkDefaultClassNameRegistry &defaultRegistry = hkDefaultClassNameRegistry::getInstance();

	hkBinaryPackfileReader bpkreader;
	hkXmlPackfileReader xpkreader;
	resource = NULL;
	hkSerializeUtil::FormatDetails formatDetails;
	hkSerializeUtil::detectFormat(reader, formatDetails);
	hkBool32 isLoadable = hkSerializeUtil::isLoadable(reader);
	if (!isLoadable && formatDetails.m_formatType != hkSerializeUtil::FORMAT_TAGFILE_XML)
	{
		return HK_FAILURE;
	}
	else
	{
		switch (formatDetails.m_formatType)
		{
		case hkSerializeUtil::FORMAT_PACKFILE_BINARY:
		{
			bpkreader.loadEntireFile(reader);
			bpkreader.finishLoadedObjects(defaultTypeRegistry);
			if (hkPackfileData* pkdata = bpkreader.getPackfileData())
			{
				obj = bpkreader.getLoadedObjects();
				for (int i = 0, n = obj.getSize(); i<n; ++i)
				{
					hkVariant& value = obj[i];
					if (value.m_class->hasVtable())
						defaultTypeRegistry.finishLoadedObject(value.m_object, value.m_class->getName());
				}
				resource = pkdata;
				resource->addReference();
			}
			root = bpkreader.getTopLevelObject();
		}
		break;

		case hkSerializeUtil::FORMAT_PACKFILE_XML:
		{
			xpkreader.loadEntireFileWithRegistry(reader, &defaultRegistry);
			if (hkPackfileData* pkdata = xpkreader.getPackfileData())
			{
				obj = xpkreader.getLoadedObjects();
				for (int i = 0, n = obj.getSize(); i<n; ++i)
				{
					hkVariant& value = obj[i];
					if (value.m_class->hasVtable())
						defaultTypeRegistry.finishLoadedObject(value.m_object, value.m_class->getName());
				}
				resource = pkdata;
				resource->addReference();
				root = xpkreader.getTopLevelObject();
			}
		}
		break;

		case hkSerializeUtil::FORMAT_TAGFILE_BINARY:
		case hkSerializeUtil::FORMAT_TAGFILE_XML:
		default:
		{
			hkSerializeUtil::ErrorDetails detailsOut;
			hkSerializeUtil::LoadOptions loadflags = hkSerializeUtil::LOAD_FAIL_IF_VERSIONING;
			resource = hkSerializeUtilLoad(reader, &detailsOut, &defaultRegistry, loadflags);
			root.m_object = resource->getContents<hkRootLevelContainer>();
			if (root.m_object != NULL)
				root.m_class = &((hkRootLevelContainer*)root.m_object)->staticClass();
		}
		break;
		}
	}
	return root.m_object != NULL ? HK_SUCCESS : HK_FAILURE;
}

hkResource *hkSerializeLoadResource(hkStreamReader *reader)
{
   hkResource *resource = NULL;
   hkVariant root;   
   hkSerializeLoad(reader, root, resource);
   return resource;
}

hkResource *hkSerializeLoadResource(hkStreamReader *reader, hkArray<hkVariant>& obj)
{
	hkResource *resource = NULL;
	hkVariant root;
	hkSerializeLoad(reader, root, resource, obj);
	return resource;
}