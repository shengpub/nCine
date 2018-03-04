#ifdef __ANDROID__
	#include <android/api-level.h>
#endif
#include "common_macros.h"
#include "TextureFormat.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureFormat::TextureFormat(GLenum internalFormat)
	: internalFormat_(internalFormat), format_(-1),
	  type_(-1), isCompressed_(false)
{
	findExternalFormat();
	checkFormatSupport();
}

TextureFormat::TextureFormat(GLenum internalFormat, GLenum type)
	: internalFormat_(internalFormat), format_(-1),
	  type_(-1), isCompressed_(false)
{
	findExternalFormat();
	checkFormatSupport();
	// Overriding the type found by `findExternalFormat()`
	type_ = type;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void TextureFormat::bgrFormat()
{
	if (format_ == GL_RGBA)
	{
#ifndef __ANDROID__
		format_ = GL_BGRA;
#else
		format_ = GL_BGRA_EXT;
#endif
	}
#ifndef __ANDROID__
	else if (format_ == GL_RGB)
		format_ = GL_BGR;
#endif
}

unsigned long TextureFormat::calculateMipSizes(GLenum internalFormat, int width, int height, int mipMapCount, unsigned long *mipDataOffsets, unsigned long *mipDataSizes)
{
	unsigned int blockWidth = 1; // Compression block width in pixels
	unsigned int blockHeight = 1; // Compression block height in pixels
	unsigned int bpp = 1; // Bits per pixel
	unsigned int blockSize = 0; // Compression block size in byts
	unsigned int minDataSize = 1; // Minimum data size in bytes

	switch (internalFormat)
	{
		case GL_RGBA:
#ifndef __ANDROID__
		case GL_RGBA8:
#else
		case GL_RGBA8_OES:
#endif
			bpp = 32;
			break;
		case GL_RGB:
#ifndef __ANDROID__
		case GL_RGB8:
#else
		case GL_RGB8_OES:
#endif
			bpp = 24;
			break;
		case GL_RG:
		case GL_LUMINANCE_ALPHA:
#ifndef __APPLE__
		case GL_RGB565:
#endif
		case GL_RGB5_A1:
		case GL_RGBA4:
#if defined(__ANDROID__) && __ANDROID_API__ >= 21
		case GL_LUMINANCE8_ALPHA8_OES:
#endif
			bpp = 16;
			break;
		case GL_RED:
		case GL_LUMINANCE:
		case GL_ALPHA:
#if defined(__ANDROID__) && __ANDROID_API__ >= 21
		case GL_LUMINANCE8_OES:
		case GL_ALPHA8_OES:
#endif
			bpp = 8;
			break;
#ifndef __ANDROID__
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			// max(1, width / 4) x max(1, height / 4) x 8(DXT1)
			blockWidth = 4;
			blockHeight = 4;
			bpp = 4;
			minDataSize = 8;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			// max(1, width / 4) x max(1, height / 4) x 16(DXT2-5)
			blockWidth = 4;
			blockHeight = 4;
			bpp = 8;
			minDataSize = 16;
			break;
#else
		case GL_ETC1_RGB8_OES:
			blockWidth = 4;
			blockHeight = 4;
			bpp = 4;
			minDataSize = 8;
			break;

		case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
		case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
			// ((width_in_texels+3)/4) * ((height_in_texels+3)/4) * 16
			blockWidth = 4;
			blockHeight = 4;
			bpp = 8;
			minDataSize = 16;
			break;
		case GL_ATC_RGB_AMD:
			// ((width_in_texels+3)/4) * ((height_in_texels+3)/4) * 8
			blockWidth = 4;
			blockHeight = 4;
			bpp = 4;
			minDataSize = 8;
			break;

		case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
			blockWidth = 8;
			blockHeight = 4;
			bpp = 2;
			minDataSize = 2 * 2 * ((blockWidth * blockHeight * bpp) / 8);
			break;
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
			blockWidth = 4;
			blockHeight = 4;
			bpp = 4;
			minDataSize = 2 * 2 * ((blockWidth * blockHeight * bpp) / 8);
			break;
	#if __ANDROID_API__ >= 21
		case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
			blockWidth = 4; blockHeight = 4;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
			blockWidth = 5; blockHeight = 4;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
			blockWidth = 5; blockHeight = 5;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
			blockWidth = 6; blockHeight = 5;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
			blockWidth = 6; blockHeight = 6;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
			blockWidth = 8; blockHeight = 5;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
			blockWidth = 8; blockHeight = 6;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
			blockWidth = 8; blockHeight = 8;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
			blockWidth = 10; blockHeight = 5;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
			blockWidth = 10; blockHeight = 6;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
			blockWidth = 10; blockHeight = 8;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
			blockWidth = 10; blockHeight = 10;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
			blockWidth = 12; blockHeight = 10;
			blockSize = 16; minDataSize = 16;
			break;
		case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
			blockWidth = 12; blockHeight = 12;
			blockSize = 16; minDataSize = 16;
			break;
	#endif
#endif
		default:
			FATAL_MSG_X("MIP maps not supported for internal format: 0x%x", internalFormat);
			break;
	}

	int levelWidth = width;
	int levelHeight = height;
	unsigned long dataSizesSum = 0;

	ASSERT(mipDataOffsets);
	ASSERT(mipDataSizes);

	for (int i = 0; i < mipMapCount; i++)
	{
		mipDataOffsets[i] = dataSizesSum;
		mipDataSizes[i] = (blockSize > 0) ?
		                  (levelWidth / blockWidth) * (levelHeight / blockHeight) * blockSize :
		                  (levelWidth / blockWidth) * (levelHeight / blockHeight) * ((blockWidth * blockHeight  * bpp) / 8);

		// Clamping to the minimum valid size
		if (mipDataSizes[i] < minDataSize)
			mipDataSizes[i] = minDataSize;

		levelWidth /= 2;
		levelHeight /= 2;
		dataSizesSum += mipDataSizes[i];
	}

	return dataSizesSum;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextureFormat::findExternalFormat()
{
	bool found = false;

#ifndef __ANDROID__
	if (found == false)
		found = integerFormat();
	if (found == false)
		found = nonIntegerFormat();
	if (found == false)
		found = floatFormat();
	if (found == false)
		found = compressedFormat();
#else
	if (found == false)
		found = oesFormat();
	if (found == false)
		found = oesFormatApi21();
	if (found == false)
		found = oesCompressedFormat();
#endif

	FATAL_ASSERT_MSG_X(found, "Unknown internal format: 0x%x", internalFormat_);

	LOGI_X("Internal format: 0x%x - type: 0x%x", internalFormat_, type_);
}

#ifndef __ANDROID__
bool TextureFormat::integerFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_RGBA:
		case GL_RGBA8:
		case 4:
			format_ = GL_RGBA;
			break;
		case GL_RGB:
		case GL_RGB8:
		case 3:
			format_ = GL_RGB;
			break;
		case GL_RG:
		case GL_LUMINANCE_ALPHA:
		case GL_LUMINANCE8_ALPHA8:
		case 2:
			format_ = GL_RG;
			break;
		case GL_RED:
		case GL_LUMINANCE:
		case GL_LUMINANCE8:
		case GL_INTENSITY:
		case GL_INTENSITY8:
		case GL_ALPHA:
		case GL_ALPHA8:
		case 1:
			format_ = GL_RED;
			break;
		case GL_DEPTH_COMPONENT:
		case GL_DEPTH_COMPONENT16_ARB:
		case GL_DEPTH_COMPONENT24_ARB:
		case GL_DEPTH_COMPONENT32_ARB:
			format_ = GL_DEPTH_COMPONENT;
			break;
		default:
			found = false;
			break;
	}

	if (found)
		type_ = GL_UNSIGNED_BYTE;

	return found;
}

bool TextureFormat::nonIntegerFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_RGB5_A1:
			format_ = GL_RGBA;
			type_ = GL_UNSIGNED_SHORT_5_5_5_1;
			break;
		case GL_RGBA4:
			format_ = GL_RGBA;
			type_ = GL_UNSIGNED_SHORT_4_4_4_4;
			break;
#ifndef __APPLE__
		case GL_RGB565:
			format_ = GL_RGB;
			type_ = GL_UNSIGNED_SHORT_5_6_5;
			break;
#endif
		default:
			found = false;
			break;
	}

	return found;
}

bool TextureFormat::floatFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_RGBA16F_ARB:
		case GL_RGBA32F_ARB:
			format_ = GL_RGBA;
			break;
		case GL_RGB16F_ARB:
		case GL_RGB32F_ARB:
			format_ = GL_RGB;
			break;
		default:
			found = false;
			break;
	}

	if (found)
		type_ = GL_FLOAT;

	return found;
}

bool TextureFormat::compressedFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_COMPRESSED_RGBA:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			format_ = GL_RGBA;
			break;
		case GL_COMPRESSED_RGB:
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			format_ = GL_RGB;
			break;
		default:
			found = false;
			break;
	}

	if (found)
	{
		type_ = GL_UNSIGNED_BYTE;
		isCompressed_ = true;
	}

	return found;
}
#else
bool TextureFormat::oesFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_RGBA8_OES:
		case GL_RGBA:
			format_ = GL_RGBA;
			break;
		case GL_RGB8_OES:
		case GL_RGB:
			format_ = GL_RGB;
			break;
		case GL_LUMINANCE_ALPHA:
			format_ = GL_LUMINANCE_ALPHA;
			break;
		case GL_LUMINANCE:
			format_ = GL_LUMINANCE;
			break;
		case GL_ALPHA:
			format_ = GL_ALPHA;
			break;
		// No GL_INTENSITY on OpenGL ES 2
		default:
			found = false;
			break;
	}

	if (found)
		type_ = GL_UNSIGNED_BYTE;

	return found;
}

bool TextureFormat::oesFormatApi21()
{
#if __ANDROID_API__ < 21
	bool found = false;
#else
	bool found = true;

	switch (internalFormat_)
	{
		case GL_LUMINANCE8_ALPHA8_OES:
			format_ = GL_LUMINANCE_ALPHA;
			break;
		case GL_LUMINANCE8_OES:
			format_ = GL_LUMINANCE;
			break;
		case GL_ALPHA8_OES:
			format_ = GL_ALPHA;
			break;
		default:
			found = false;
			break;
	}

	switch (internalFormat_)
	{
		case GL_LUMINANCE8_ALPHA8_OES:
		case GL_LUMINANCE8_OES:
		case GL_ALPHA8_OES:
			type_ = GL_UNSIGNED_BYTE;
			break;
		default:
			found = false;
			break;
	}
#endif
	return found;
}

bool TextureFormat::oesCompressedFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
		case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
#if __ANDROID_API__ >= 21
		case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
		case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
		case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
		case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
		case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
#endif
			format_ = GL_RGBA;
			break;
		case GL_ETC1_RGB8_OES:
		case GL_ATC_RGB_AMD:
		case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
			format_ = GL_RGB;
			break;
		default:
			found = false;
			break;
	}

	if (found)
	{
		type_ = GL_UNSIGNED_BYTE;
		isCompressed_ = true;
	}

	return found;
}
#endif

void TextureFormat::checkFormatSupport() const
{
	const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();

	switch (internalFormat_)
	{
#ifndef __ANDROID__
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		{
			const bool hasS3tc = gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::EXT_TEXTURE_COMPRESSION_S3TC);
			FATAL_ASSERT_MSG(hasS3tc, "GL_EXT_texture_compression_s3tc not available");
			break;
		}
#else
		case GL_ETC1_RGB8_OES:
		{
			const bool hasEct1 = gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::OES_COMPRESSED_ETC1_RGB8_TEXTURE);
			FATAL_ASSERT_MSG(hasEct1, "GL_OES_compressed_etc1_rgb8_texture not available");
			break;
		}
		case GL_ATC_RGB_AMD:
		case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
		case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
		{
			const bool hasAtc = gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::AMD_COMPRESSED_ATC_TEXTURE);
			FATAL_ASSERT_MSG(hasAtc, "GL_AMD_compressed_ATC_texture not available");
			break;
		}
		case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
		{
			const bool hasPvr = gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::IMG_TEXTURE_COMPRESSION_PVRTC);
			FATAL_ASSERT_MSG(hasPvr, "GL_IMG_texture_compression_pvrtc not available");
			break;
		}
	#if __ANDROID_API__ >= 21
		case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
		case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
		case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
		case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
		case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
		{
			const bool hasAstc = gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::KHR_TEXTURE_COMPRESSION_ASTC_LDR);
			FATAL_ASSERT_MSG(hasAstc, "GL_KHR_texture_compression_astc_ldr not available");
			break;
		}
	#endif
#endif
	}
}

}
