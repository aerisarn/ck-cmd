#pragma once
#ifndef _DDS_H_
#define _DDS_H_

typedef unsigned long DWORD;

#define DDS_MAGIC 0x20534444
#define DDS_SIZE 124

#define DDS_DXT1 0x31545844
#define DDS_DXT3 0x33545844
#define DDS_DXT5 0x35545844

struct DDS_PIXELFORMAT {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwABitMask;
};

typedef struct {
	DWORD           dwSize;
	DWORD           dwFlags;
	DWORD           dwHeight;
	DWORD           dwWidth;
	DWORD           dwPitchOrLinearSize;
	DWORD           dwDepth;
	DWORD           dwMipMapCount;
	DWORD           dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	DWORD           dwSurfaceFlags;
	DWORD           dwCubemapFlags;
	DWORD           dwReserved2[3];
} DDS_HEADER;


#endif