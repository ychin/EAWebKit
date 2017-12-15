/****************************************************

This file includes code from the Irrlicht project, 
with some modifications. 
http://irrlicht.sourceforge.net/

The Irrlicht Engine License

Copyright © 2002-2005 Nikolaus Gebhardt

This software is provided 'as-is', without any 
express or implied warranty. In no event will the 
authors be held liable for any damages arising from 
the use of this software.

Permission is granted to anyone to use this software
for any purpose, including commercial applications,
and to alter it and redistribute it freely, subject
to the following restrictions:

    1.	The origin of this software must not be
		misrepresented; you must not claim that you
		wrote the original software. If you use this
		software in a product, an acknowledgment in
		the product documentation would be 
		appreciated but is not required.
	2.	Altered source versions must be plainly
		marked as such, and must not be 
		misrepresented as being the original 
		software.
	3.	This notice may not be removed or altered
		from any source distribution.


****************************************************/

#ifndef __DDSHELPER_H
#define __DDSHELPER_H 1

#ifdef __cplusplus
extern "C" {
#endif

    /* dependencies */
/* dds definition */
enum eDDSPixelFormat
{
	DDS_PF_ARGB8888 = 0,
	DDS_PF_DXT1     = 1,
	DDS_PF_DXT2     = 2,
	DDS_PF_DXT3     = 3,
	DDS_PF_DXT4     = 4,
	DDS_PF_DXT5     = 5,
    DDS_PF_ATC      = 6,
    DDS_PF_ATCA     = 7,
    DDS_PF_ATCI     = 8,
	DDS_PF_UNKNOWN  = 9
};

typedef enum _D3DFORMAT {
    D3DFMT_UNKNOWN              =  0,

    D3DFMT_R8G8B8               = 20,
    D3DFMT_A8R8G8B8             = 21,
    D3DFMT_X8R8G8B8             = 22,
    D3DFMT_R5G6B5               = 23,
    D3DFMT_X1R5G5B5             = 24,
    D3DFMT_A1R5G5B5             = 25,
    D3DFMT_A4R4G4B4             = 26,
    D3DFMT_R3G3B2               = 27,
    D3DFMT_A8                   = 28,
    D3DFMT_A8R3G3B2             = 29,
    D3DFMT_X4R4G4B4             = 30,
    D3DFMT_A2B10G10R10          = 31,
    D3DFMT_A8B8G8R8             = 32,
    D3DFMT_X8B8G8R8             = 33,
    D3DFMT_G16R16               = 34,
    D3DFMT_A2R10G10B10          = 35,
    D3DFMT_A16B16G16R16         = 36,

    D3DFMT_A8P8                 = 40,
    D3DFMT_P8                   = 41,

    D3DFMT_L8                   = 50,
    D3DFMT_A8L8                 = 51,
    D3DFMT_A4L4                 = 52,

    D3DFMT_V8U8                 = 60,
    D3DFMT_L6V5U5               = 61,
    D3DFMT_X8L8V8U8             = 62,
    D3DFMT_Q8W8V8U8             = 63,
    D3DFMT_V16U16               = 64,
    D3DFMT_A2W10V10U10          = 67,



    D3DFMT_D16_LOCKABLE         = 70,
    D3DFMT_D32                  = 71,
    D3DFMT_D15S1                = 73,
    D3DFMT_D24S8                = 75,
    D3DFMT_D24X8                = 77,
    D3DFMT_D24X4S4              = 79,
    D3DFMT_D16                  = 80,

    D3DFMT_D32F_LOCKABLE        = 82,
    D3DFMT_D24FS8               = 83,

#if !defined(D3D_DISABLE_9EX)
    D3DFMT_D32_LOCKABLE         = 84,
    D3DFMT_S8_LOCKABLE          = 85,
#endif // !D3D_DISABLE_9EX

    D3DFMT_L16                  = 81,

    D3DFMT_VERTEXDATA           =100,
    D3DFMT_INDEX16              =101,
    D3DFMT_INDEX32              =102,

    D3DFMT_Q16W16V16U16         =110,

    D3DFMT_R16F                 = 111,
    D3DFMT_G16R16F              = 112,
    D3DFMT_A16B16G16R16F        = 113,

    D3DFMT_R32F                 = 114,
    D3DFMT_G32R32F              = 115,
    D3DFMT_A32B32G32R32F        = 116,

    D3DFMT_CxV8U8               = 117,

#if !defined(D3D_DISABLE_9EX)
    D3DFMT_A1                   = 118,
    D3DFMT_A2B10G10R10_XR_BIAS  = 119,
    D3DFMT_BINARYBUFFER         = 199,
#endif // !D3D_DISABLE_9EX

    D3DFMT_FORCE_DWORD          =0x7fffffff
} D3DFORMAT;
/* 16bpp stuff */
#define DDS_LOW_5		0x001F;
#define DDS_MID_6		0x07E0;
#define DDS_HIGH_5		0xF800;
#define DDS_MID_555		0x03E0;
#define DDS_HI_555		0x7C00;

#define DDSD_MIPMAPCOUNT 0x00020000
#define DDSD_CAPS	0x00000001
#define DDSD_HEIGHT	0x00000002
#define DDSD_WIDTH	0x00000004
#define DDSD_PITCH	0x00000008
#define DDSD_PIXELFORMAT	0x00001000
#define DDSD_MIPMAPCOUNT	0x00020000
#define DDSD_LINEARSIZE	0x00080000
#define DDSD_DEPTH	0x00800000

#define DDPF_ALPHAPIXELS	0x00000001
#define DDPF_FOURCC	0x00000004
#define DDPF_RGB	0x00000040

#define DDSCAPS_COMPLEX	0x00000008
#define DDSCAPS_TEXTURE	0x00001000
#define DDSCAPS_MIPMAP	0x00400000

#define DDPF_ALPHAPIXELS        0x00000001
#define DDPF_FOURCC             0x00000004
#define DDPF_RGB                0x00000040
#define DDPF_ALPHA              0x00000002
#define DDPF_COMPRESSED         0x00000080
#define DDPF_LUMINANCE          0x00020000


#define DDSCAPS2_CUBEMAP	0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX	0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX	0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY	0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY	0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ	0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ	0x00008000
#define DDSCAPS2_VOLUME	0x00200000

// byte-align structures
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	define PACK_STRUCT
#endif





/* structures */
struct ddsColorKey
{
	uint32_t		colorSpaceLowValue;
	uint32_t		colorSpaceHighValue;
} PACK_STRUCT;

struct ddsCaps
{
	uint32_t		caps1;
	uint32_t		caps2;
	uint32_t		caps3;
	uint32_t		caps4;
} PACK_STRUCT;

struct ddsMultiSampleCaps
{
	uint16_t		flipMSTypes;
	uint16_t		bltMSTypes;
} PACK_STRUCT;


struct ddsPixelFormat
{
	uint32_t		size;
	uint32_t		flags;
	uint32_t		fourCC;
	union
	{
		uint32_t	rgbBitCount;
		uint32_t	yuvBitCount;
		uint32_t	zBufferBitDepth;
		uint32_t	alphaBitDepth;
		uint32_t	luminanceBitCount;
		uint32_t	bumpBitCount;
		uint32_t	privateFormatBitCount;
	};
	union
	{
		uint32_t	rBitMask;
		uint32_t	yBitMask;
		uint32_t	stencilBitDepth;
		uint32_t	luminanceBitMask;
		uint32_t	bumpDuBitMask;
		uint32_t	operations;
	};
	union
	{
		uint32_t	gBitMask;
		uint32_t	uBitMask;
		uint32_t	zBitMask;
		uint32_t	bumpDvBitMask;
		ddsMultiSampleCaps	multiSampleCaps;
	};
	union
	{
		uint32_t	bBitMask;
		uint32_t	vBitMask;
		uint32_t	stencilBitMask;
		uint32_t	bumpLuminanceBitMask;
	};
	union
	{
		uint32_t	rgbAlphaBitMask;
		uint32_t	yuvAlphaBitMask;
		uint32_t	luminanceAlphaBitMask;
		uint32_t	rgbZBitMask;
		uint32_t	yuvZBitMask;
	};
} PACK_STRUCT;


struct ddsBuffer
{
	/* magic: 'dds ' */
	char				magic[ 4 ];

	/* directdraw surface */
	uint32_t		size;
	uint32_t		flags;
	uint32_t		height;
	uint32_t		width;
	union
	{
		int32_t		pitch;
		uint32_t	linearSize;
	};
	uint32_t		backBufferCount;
	union
	{
		uint32_t	mipMapCount;
		uint32_t	refreshRate;
		uint32_t	srcVBHandle;
	};
	uint32_t		alphaBitDepth;
	uint32_t		reserved;
	uint32_t		surfaceOffset; // This was a void pointer called surface. I changed it to a fixed-width int for 64 bit compatibility.
	union
	{
		ddsColorKey	ckDestOverlay;
		uint32_t	emptyFaceColor;
	};
	ddsColorKey		ckDestBlt;
	ddsColorKey		ckSrcOverlay;
	ddsColorKey		ckSrcBlt;
	union
	{
		ddsPixelFormat	pixelFormat;
		uint32_t	fvf;
	};
	ddsCaps			caps;
	uint32_t		textureStage;

	/* data (Varying size) */
	unsigned char		data[ 4 ];
} PACK_STRUCT;


struct ddsColorBlock
{
	unsigned short		colors[ 2 ];
	unsigned char		row[ 4 ];
} PACK_STRUCT;


struct ddsAlphaBlockExplicit
{
	unsigned short		row[ 4 ];
} PACK_STRUCT;


struct ddsAlphaBlock3BitLinear
{
	unsigned char		alpha0;
	unsigned char		alpha1;
	unsigned char		stuff[ 6 ];
} PACK_STRUCT;


struct ddsColor
{
	unsigned char		r, g, b, a;
} PACK_STRUCT;



/* endian tomfoolery */
typedef union
{
	float f;
	char  c[ 4 ];
}
floatSwapUnion;

// Default alignment
#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__)
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT


int32_t DDSLittleLong( int32_t src );
signed short DDSLittleShort( int16_t src );
float DDSLittleFloat( float src );


void DDSDecodePixelFormat( ddsBuffer *dds, eDDSPixelFormat *pf );
int DDSGetInfo( ddsBuffer *dds, int32_t *width, int32_t *height, eDDSPixelFormat *pf );
void DDSGetColorBlockColors( ddsColorBlock *block, ddsColor colors[ 4 ] );
void DDSDecodeColorBlock( uint32_t *pixel, ddsColorBlock *block,  signed long width, uint32_t colors[ 4 ] );
void DDSDecodeAlphaExplicit( uint32_t *pixel, ddsAlphaBlockExplicit *alphaBlock, signed long width, unsigned long alphaZero );
void DDSDecodeAlpha3BitLinear( uint32_t *pixel, ddsAlphaBlock3BitLinear *alphaBlock, signed long width, unsigned long alphaZero );
signed long DDSDecompressDXT1( ddsBuffer *dds, signed long width, signed long height, unsigned char *pixels );
signed long DDSDecompressDXT2( ddsBuffer *dds, signed long width, signed long height, unsigned char *pixels );
signed long DDSDecompressDXT3( ddsBuffer *dds, signed long width, signed long height, unsigned char *pixels );
signed long DDSDecompressDXT4( ddsBuffer *dds, signed long width, signed long height, unsigned char *pixels );
signed long DDSDecompressDXT5( ddsBuffer *dds, signed long width, signed long height, unsigned char *pixels );
signed long DDSDecompressARGB8888( ddsBuffer *dds, signed long width, signed long height, unsigned char *pixels );
signed long DDSDecompress( ddsBuffer *dds, unsigned char *pixels );


#ifdef __cplusplus
}
#endif


#endif

