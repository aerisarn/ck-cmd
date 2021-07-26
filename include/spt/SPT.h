
#define _HAS_STD_BYTE 0
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <niflib.h>
#include <obj\NiObject.h>

#include <filesystem>

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

struct SEmbeddedTexCoords {
	size_t numEmbeddedTextures;
	const float* coords;
};

void sptconvert(const fs::path& spt_file, const fs::path& export_path, const map<string,string>& metadata);