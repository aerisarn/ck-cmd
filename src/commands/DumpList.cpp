#include "stdafx.h"

#include <commands/DumpList.h>

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/hkfutils.h>
#include <core/log.h>
#include <cstdio>
#include <sys/stat.h>

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/Reflection/Registry/hkDynamicClassNameRegistry.h>

// Scene
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkLoader.h>

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

using namespace std;

DumpList::DumpList()
{
}

DumpList::~DumpList()
{
}

string DumpList::GetName() 
{
    return "DumpList";
}

string DumpList::GetHelp() 
{
    string name = GetName();
    transform(name.begin(), name.end(), name.begin(), ::tolower);

    // Usage: ck-cmd dumplist <infile> [-o <outfile]
    string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <infile> [-o <outfile]\r\n";

    const char help[] = 
R"(Dump the transform or float list for a given skeleton
This is useful when exporting animation to get bone list synchronized with source

Arguments:
    <infile>    Input File or directory

Options:
    -o <outfile>    Output File - Defaults to input file with '-out' appended)";

    return usage + help;
}

string DumpList::GetHelpShort() 
{
    return "Dump the transform or float list for a given skeleton";
}

bool DumpList::InternalRunCommand(const CommandSettings& settings)
{
    // TODO: SafeExecuteCmd

	string inpath; // = parsedArgs["<infile>"].asString();
	string outpath; // = parsedArgs["-o"].asString();
	hkSerializeUtil::SaveOptionBits flags = (hkSerializeUtil::SaveOptionBits)(hkSerializeUtil::SAVE_TEXT_FORMAT|hkSerializeUtil::SAVE_TEXT_NUMBERS);

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

	if (inpath.empty()){
        Log::Info(GetHelp().c_str());
		return false;
	}
	if (PathIsDirectory(inpath.c_str()))
	{
		char path[MAX_PATH];
		strcpy(path, inpath.c_str());
		PathAddBackslash(path);
		strcat(path, "*.hkx");
		GetFullPathName(path, MAX_PATH, path, NULL);
		inpath = path;
	}
	char rootDir[MAX_PATH];
	strcpy(rootDir, inpath.c_str());
	GetFullPathName(rootDir, MAX_PATH, rootDir, NULL);
	if (!PathIsDirectory(rootDir))
		PathRemoveFileSpec(rootDir);

	// explicit exclusions due to crashes
	stringlist excludes;
	excludes.push_back("*project.hkx");
	excludes.push_back("*behavior.hkx");
	excludes.push_back("*charater.hkx");
	excludes.push_back("*character.hkx");

	vector<string> files;
	FindFiles(files, inpath.c_str(), excludes);
	if (files.empty())
	{
		Log::Error("No files found in '%s'", inpath.c_str());
		return false;
	}

	hkMallocAllocator baseMalloc;
	// Need to have memory allocated for the solver. Allocate 1mb for it.
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( &baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024) );
	hkBaseSystem::init( memoryRouter, errorReport );
   LoadDefaultRegistry();

	{
		for (vector<string>::iterator itr = files.begin(); itr != files.end(); ++itr)
		{
			char infile[MAX_PATH], relpath[MAX_PATH];
			try
			{
				strcpy(infile, (*itr).c_str());
				GetFullPathName(infile, MAX_PATH, infile, NULL);

				LPCSTR extn = PathFindExtension(infile);
				if (stricmp(extn, ".hkx") != 0)
				{
					Log::Verbose("Unexpected extension. Skipping '%s'", infile);
					continue;
				}
				PathRelativePathTo(relpath, rootDir, FILE_ATTRIBUTE_DIRECTORY, infile, 0);

				char outfile[MAX_PATH];
				if (outpath.empty())
				{
					char drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
					_splitpath(infile, drive, dir, fname, ext);
					strcat(fname, "-out");
					_makepath(outfile, drive, dir, fname, ext);
					outpath = rootDir;
				}
				else
				{
					LPCSTR oextn = PathFindExtension(outpath.c_str());
					if (oextn == NULL || stricmp(oextn, ".txt") != 0)
					{
						PathCombine(outfile, outpath.c_str(), relpath);
						GetFullPathName(outfile, MAX_PATH, outfile, NULL);
					}
					else
					{
						GetFullPathName(outpath.c_str(), MAX_PATH, outfile, NULL);
					}
				}

				char outdir[MAX_PATH];
				strcpy(outdir, outfile);
				PathRemoveFileSpec(outdir);
				CreateDirectories(outdir);

				if (stricmp(infile, outfile) == 0)
				{
					char drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
					_splitpath(infile, drive, dir, fname, ext);
					strcat(fname, "-out");
					_makepath(outfile, drive, dir, fname, ".txt");
				}

				Log::Info("Converting '%s' ...", relpath );

				hkIstream stream(infile);
				hkStreamReader *reader = stream.getStreamReader();
            hkResource* resource = hkSerializeLoadResource(reader);
				if (resource == NULL)
				{
					Log::Warn("File is not loadable: '%s'", relpath);
				}
				else
				{
					if (hkRootLevelContainer* scene = resource->getContents<hkRootLevelContainer>())
					{
						if (hkaAnimationContainer *skelAnimCont = scene->findObject<hkaAnimationContainer>())
						{
							char path[MAX_PATH], drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
							int nskel = skelAnimCont->m_skeletons.getSize();
							if ( nskel == 0 )
							{
								Log::Warn("No skeletons found in resource. Skipping '%s'", relpath);
							}
							else
							{
								if (nskel>1) Log::Info("Multiple skeletons found.");
								for (int iskel=0; iskel<nskel; ++iskel)
								{
									if (nskel>1)
									{
										_splitpath(outfile, drive, dir, fname, ext);
										sprintf(fname+strlen(fname), "-%d", iskel+1);
										_makepath(path, drive, dir, fname, ext);
									}
									else
									{
										strcpy(path, outfile);
									}

									hkRefPtr<hkaSkeleton> skeleton = skelAnimCont->m_skeletons[0];
									int nbones = skeleton->m_bones.getSize();
									Log::Info("Exporting skeleton '%s' with %d bones to '%s'"
										, skeleton->m_name, skeleton->m_bones.getSize(), PathFindFileName(path));

									{
										hkOstream stream(path);
										for (int i=0; i<nbones; ++i)
										{
											string name = skeleton->m_bones[i].m_name;
											stream.printf("%s\r\n", name.c_str());
										}
									}
									if (!skeleton->m_floatSlots.isEmpty())
									{
										strcat(fname, "-floats");
										_makepath(path, drive, dir, fname, ext);

										Log::Info("Exporting skeleton '%s' with %d floats to '%s'"
											, skeleton->m_name, skeleton->m_floatSlots.getSize(), PathFindFileName(path));

										hkOstream stream(path);
										for (int i=0, n=skeleton->m_floatSlots.getSize(); i<n; ++i)
										{
											string name = skeleton->m_floatSlots[i];
											stream.printf("%s\r\n", name.c_str());
										}
									}
								}
							}

						}
					}
					resource->removeReference();
				}
			}
			catch (...)
			{
				Log::Error("Unexpected exception occurred while processing '%s'", relpath);
			}
		}
	}

	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();


	return true;
}