#include "stdafx.h"

#include <commands/Test.h>

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/log.h>
#include <cstdio>
#include <sys/stat.h>

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/Reflection/Registry/hkDynamicClassNameRegistry.h>
#include <Common/Base/Reflection/Registry/hkDefaultClassNameRegistry.h>


// Scene
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkNativePackfileUtils.h>
#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Version/hkVersionPatchManager.h>

// Physics
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

// Animation
#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Animation/Animation/Rig/hkaPose.h>
#include <Animation/Ragdoll/Controller/PoweredConstraint/hkaRagdollPoweredConstraintController.h>
#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
#include <Animation/Ragdoll/Utils/hkaRagdollUtils.h>

// Serialize
#include <Common/Serialize/Util/hkSerializeUtil.h>

#include "hkbProjectData_2.h"
#include "hkbProjectStringData_1.h"

#define RETURN_FAIL_IF(COND, MSG) \
	HK_MULTILINE_MACRO_BEGIN \
	if(COND) { HK_ERROR(0x53a6a026, MSG); return 1; } \
	HK_MULTILINE_MACRO_END

using namespace std;

Test::Test()
{
}

Test::~Test()
{
}

string Test::GetName() 
{
    return "Test";
}

string Test::GetHelp() 
{
    string name = GetName();
    transform(name.begin(), name.end(), name.begin(), ::tolower);

    // Usage: ck-cmd test <infile> [-o <outfile>] [-d <level>] [-f <flags> ...]
    string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <infile> [-o <outfile>] [-d <level>] [-f <flags> ...]\r\n";

    const char help[] = 
R"(Test

Arguments:
    <infile>    Input File or directory

Options:
    -o <outfile>    Output File - Defaults to input file with '-out' appended
    -d <level>      Debug Level: ERROR, WARN, INFO, DEBUG, VERBOSE 
                    [default: INFO]
    -f <flags>      Havok saving flags: SAVE_DEFAULT, SAVE_TEXT_FORMAT, SAVE_SERIALIZE_IGNORED_MEMBERS, SAVE_WRITE_ATTRIBUTES, SAVE_CONCISE, SAVE_TEXT_NUMBERS 
                    [default: SAVE_TEXT_FORMAT SAVE_TEXT_NUMBERS]

Havok saving flags:
    SAVE_DEFAULT                    All flags default to OFF, enable whichever are needed
    SAVE_TEXT_FORMAT                Use text (usually XML) format, default is binary format if available
    SAVE_SERIALIZE_IGNORED_MEMBERS  Write members which are usually ignored
    SAVE_WRITE_ATTRIBUTES           Include extended attributes in metadata, default is to write minimum metadata
    SAVE_CONCISE                    Doesn't provide any extra information which would make the file easier to interpret. E.g. additionally write hex floats as text comments
    SAVE_TEXT_NUMBERS               Floating point numbers output as text, not as binary. Makes them easily readable/editable, but values may not be exact)";

    return usage + help;
}

string Test::GetHelpShort() 
{
    return "Test";
}

namespace
{

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
}

typedef void (__stdcall * DumpClassesAllDecl)();


enum hkPackFormat
{
   HKPF_XML,
   HKPF_DEFAULT,
   HKPF_WIN32,
   HKPF_AMD64,
   HKPF_XBOX,
   HKPF_XBOX360,
};

static hkPackfileWriter::Options GetWriteOptionsFromFormat(hkPackFormat format)
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
static void HK_CALL errorReport(const char* msg, void* userContext)
{
	Log::Error("%s", msg);
}

static hkResource* hkSerializeUtilLoad( hkStreamReader* stream
								, hkSerializeUtil::ErrorDetails* detailsOut=HK_NULL
								, const hkClassNameRegistry* classReg=HK_NULL
								, hkSerializeUtil::LoadOptions options=hkSerializeUtil::LOAD_DEFAULT )
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
bool Test::InternalRunCommand(const CommandSettings& settings)
{
    // TODO: SafeExecuteCmd

	string inpath; // = parsedArgs["<infile>"].asString();
	string outpath; // = parsedArgs["-o"].asString();

	hkSerializeUtil::SaveOptionBits flags; // = (hkSerializeUtil::SaveOptionBits)StringToFlags(parsedArgs["-f"].asStringList(), SaveFlags, hkSerializeUtil::SAVE_DEFAULT);
    //Log::SetLogLevel((LogLevel)StringToEnum(parsedArgs["-d"].asString(), LogFlags, LOG_INFO));

	//slist<hkxcmd *> plugins;

    /*
#pragma region Handle Input Args
	for (int i = 0; i < argc; i++)
	{
		char *arg = argv[i];
		if (arg == NULL)
			continue;
		if (arg[0] == '-' || arg[0] == '/')
		{

			switch (tolower(arg[1]))
			{
			case 'f':
				{
					const char *param = arg+2;
					if (*param == ':' || *param=='=') ++param;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					flags = (hkSerializeUtil::SaveOptionBits)StringToFlags(param, SaveFlags, hkSerializeUtil::SAVE_DEFAULT);
				} break;

			case 'd':
				{
					const char *param = arg+2;
					if (*param == ':' || *param=='=') ++param;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
					{
						Log::SetLogLevel(LOG_DEBUG);
						break;
					}
					else
					{
						Log::SetLogLevel((LogLevel)StringToEnum(param, LogFlags, LOG_INFO));
					}
				} break;

			case 'o':
				{
					const char *param = arg+2;
					if (*param == ':' || *param=='=') ++param;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					if (outpath.empty())
					{
						outpath = param;
					}
					else
					{
						Log::Error("Output file already specified as '%s'", outpath.c_str());
					}
				} break;

			case 'i':
				{
					const char *param = arg+2;
					if (*param == ':' || *param=='=') ++param;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					if (inpath.empty())
					{
						inpath = param;
					}
					else
					{
						Log::Error("Input file already specified as '%s'", inpath.c_str());
					}
				} break;

			default:
				Log::Error("Unknown argument specified '%s'", arg);
				break;
			}
		}
		else if (inpath.empty())
		{
			inpath = arg;
		}
		else if (outpath.empty())
		{
			outpath = arg;
		}
	}
#pragma endregion
   */

   hkMallocAllocator baseMalloc;
   // Need to have memory allocated for the solver. Allocate 1mb for it.
   hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( &baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024) );
   hkBaseSystem::init( memoryRouter, errorReport );

   // Normally all the patches would be added to the global singleton but
   // for this example we'll use a private manager to keep the scope small
   // and not leave useless patches in the global registry
   {
      hkVersionPatchManager patchManager;
      {
         extern void HK_CALL CustomRegisterPatches(hkVersionPatchManager& patchManager);
         CustomRegisterPatches(patchManager);
      }
      hkDefaultClassNameRegistry &dynamicRegistry = hkDefaultClassNameRegistry::getInstance();
      {
         extern void HK_CALL CustomRegisterDefaultClasses();
         extern void HK_CALL ValidateClassSignatures();
         CustomRegisterDefaultClasses();
         ValidateClassSignatures();
      } 

      hkArray<char> inputBuffer;
      {
         hkRefPtr<hkaBoneAttachment> battach = new hkaBoneAttachment();
         battach->m_name = "asdf";
         battach->m_originalSkeletonName = "";

         hkRootLevelContainer rootCont;
         hkRefPtr<hkbProjectStringData> stringData = new hkbProjectStringData();
         hkRefPtr<hkbProjectData> projectData = new hkbProjectData();

         projectData->m_defaultEventMode = hkbTransitionEffect::EVENT_MODE_IGNORE_FROM_GENERATOR;
         projectData->m_worldUpWS = hkVector4(0,0,1,0);
         projectData->m_stringData = stringData.val();
         hkStringPtr fname("Characters Female\\DefaultFemale.hkx");

         stringData->m_characterFilenames.append(&fname,1);
         stringData->m_animationPath = "";
         stringData->m_characterPath = "";
         stringData->m_behaviorPath = "";
         stringData->m_rootPath = "";
         stringData->m_fullPathToSource = "";

         rootCont.m_namedVariants.pushBack( hkRootLevelContainer::NamedVariant("hkbProjectData", (void *)projectData.val(), &projectData.val()->staticClass()) );
         //rootCont.m_namedVariants.pushBack( hkRootLevelContainer::NamedVariant("TestMe", (void *)battach.val(), &battach.val()->staticClass()) );
         

         hkResult res;
         res = hkSerializeUtil::savePackfile(&rootCont, rootCont.staticClass(), hkOstream(inputBuffer).getStreamWriter(), GetWriteOptionsFromFormat(HKPF_WIN32), NULL,
            (hkSerializeUtil::SaveOptionBits)(hkSerializeUtil::SAVE_DEFAULT) );
         
         hkPackfileWriter::Options packFileOptions = GetWriteOptionsFromFormat(HKPF_XML);
         res = hkSerializeUtil::savePackfile(&rootCont, rootCont.staticClass(), hkOstream("test.xml").getStreamWriter(), packFileOptions, NULL, 
            (hkSerializeUtil::SaveOptionBits)(hkSerializeUtil::SAVE_TEXT_NUMBERS | hkSerializeUtil::SAVE_TEXT_FORMAT) );


         res = hkSerializeUtil::savePackfile(&rootCont, rootCont.staticClass(), hkOstream("test.hkx").getStreamWriter(), GetWriteOptionsFromFormat(HKPF_WIN32), NULL,
            (hkSerializeUtil::SaveOptionBits)(hkSerializeUtil::SAVE_DEFAULT) );
      }

      hkRootLevelContainer * container;
      hkArray<char> dataBuffer;
      {
         int bufferSize = hkNativePackfileUtils::getRequiredBufferSize( inputBuffer.begin(), inputBuffer.getSize() );
         dataBuffer.reserveExactly(bufferSize);
         container = static_cast<hkRootLevelContainer*>(
            hkNativePackfileUtils::load(inputBuffer.begin(), inputBuffer.getSize(), dataBuffer.begin(), dataBuffer.getCapacity())
            );
      }
   }

   hkBaseSystem::quit();
	hkMemoryInitUtil::quit();


	return true;
}

//static bool SafeExecuteCmd(hkxcmdLine &cmdLine)
//{
//   __try{
//      return ExecuteCmd(cmdLine);
//   } __except (EXCEPTION_EXECUTE_HANDLER){
//      return false;
//   }
//}
