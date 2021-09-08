#pragma once

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

union alpha_flags
{

	enum gl_blend_modes {
		GL_ONE = 0,
		GL_ZERO = 1,
		GL_SRC_COLOR = 2,
		GL_ONE_MINUS_SRC_COLOR = 3,
		GL_DST_COLOR = 4,
		GL_ONE_MINUS_DST_COLOR = 5,
		GL_SRC_ALPHA = 6,
		GL_ONE_MINUS_SRC_ALPHA = 7,
		GL_DST_ALPHA = 8,
		GL_ONE_MINUS_DST_ALPHA = 9,
		GL_SRC_ALPHA_SATURATE = 10
	};

	enum gl_test_modes {
		GL_ALWAYS = 0,
		GL_LESS = 1,
		GL_EQUAL = 2,
		GL_LEQUAL = 3,
		GL_GREATER = 4,
		GL_NOTEQUAL = 5,
		GL_GEQUAL = 6,
		GL_NEVER = 7
	};

	struct {
		// Bit 0 : color blending enable
		unsigned color_blending_enable : 1;
		// Bits 1-4 : source blend mode
		unsigned source_blend_mode : 4;
		// Bits 5-8 : destination blend mode
		unsigned destination_blend_mode : 4;
		// Bit 9 : alpha test enable
		unsigned alpha_test_enable : 1;
		// Bit 10-12 : alpha test mode
		unsigned alpha_test_mode : 3;
		// Bit 13 : no sorter flag ( disables triangle sorting )
		unsigned no_sorter_flag : 1;

		bool readsAlphaSource() {
			return
				source_blend_mode == GL_SRC_ALPHA ||
				source_blend_mode == GL_ONE_MINUS_SRC_ALPHA ||
				source_blend_mode == GL_DST_ALPHA ||
				source_blend_mode == GL_ONE_MINUS_DST_ALPHA ||
				source_blend_mode == GL_SRC_ALPHA_SATURATE;
		}

	} bits;
	unsigned int value;
};

void sptconvert(const fs::path& spt_file, const fs::path& export_path, const map<string,string>& metadata);