#include "config.h"
#pragma once

/*
* Define the major, minor, build, patch version numbers
*/
#define VERSION_MAJOR_INT  1
#define VERSION_MINOR_INT  3
#define VERSION_BUILD_INT  0

#ifndef VCS_REVISION_INT
#define VERSION_PATCH_INT  0
#else
#define  VERSION_PATCH_INT  VCS_REVISION_INT
#endif

#ifndef VCS_REVISION
#define VERSION_STRING "1, 3, 0, 0"
#else
#define VERSION_STRING "1, 3, 0, " VCS_REVISION
#endif

//#define DEF_VERSION_STRING(a,b,c,d) "##a, ##b , ##c , ##d"
//#define VERSION_STRING DEF_VERSION_STRING(a,b,c,d)

#define COPYRIGHTYEARINT 2011
#define COPYRIGHTYEAR "2011"

#define DLLNAME "hkxcmd"
#define PRODUCTNAME "Havok Command Line Tools"
#define RESFILETYPE VFT_APP
