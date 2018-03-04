#include <cstdio> // for sscanf()
#include <cstring> // for checkGLExtension()
#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include "GfxCapabilities.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GfxCapabilities::GfxCapabilities()
	: glMajorVersion_(0),
	  glMinorVersion_(0),
	  glReleaseVersion_(0)
{
	for (unsigned int i = 0; i < GLIntValues::COUNT; i++)
		glIntValues_[i] = 0;

	for (unsigned int i = 0; i < GLExtensions::COUNT; i++)
		glExtensions_[i] = false;

	init();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

int GfxCapabilities::glVersion(IGfxCapabilities::GLVersion version) const
{
	switch (version)
	{
		case GLVersion::MAJOR:		return glMajorVersion_;
		case GLVersion::MINOR:		return glMinorVersion_;
		case GLVersion::RELEASE:	return glReleaseVersion_;

		default: return 0;
	}
}

int GfxCapabilities::value(GLIntValues::Enum valueName) const
{
	int value = 0;

	if (valueName >= 0 && valueName < GLIntValues::COUNT)
		value = glIntValues_[valueName];

	return value;
}

bool GfxCapabilities::hasExtension(GLExtensions::Enum extensionName) const
{
	bool extensionAvailable = false;

	if (extensionName >= 0 && extensionName < GLExtensions::COUNT)
		extensionAvailable = glExtensions_[extensionName];

	return extensionAvailable;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GfxCapabilities::init()
{
	const char *pVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
#ifndef __ANDROID__
	sscanf(pVersion, "%2d.%2d.%2d", &glMajorVersion_, &glMinorVersion_, &glReleaseVersion_);
#else
	sscanf(pVersion, "OpenGL ES %2d.%2d", &glMajorVersion_, &glMinorVersion_);
#endif

	glGetIntegerv(GL_CONTEXT_FLAGS, &glIntValues_[GLIntValues::CONTEXT_FLAGS]);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glIntValues_[GLIntValues::MAX_TEXTURE_SIZE]);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &glIntValues_[GLIntValues::MAX_TEXTURE_IMAGE_UNITS]);
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &glIntValues_[GLIntValues::MAX_UNIFORM_BLOCK_SIZE]);
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &glIntValues_[GLIntValues::MAX_UNIFORM_BUFFER_BINDINGS]);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &glIntValues_[GLIntValues::MAX_VERTEX_UNIFORM_BLOCKS]);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &glIntValues_[GLIntValues::MAX_FRAGMENT_UNIFORM_BLOCKS]);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &glIntValues_[GLIntValues::UNIFORM_BUFFER_OFFSET_ALIGNMENT]);
	glGetIntegerv(GL_MAX_LABEL_LENGTH, &glIntValues_[GLIntValues::MAX_LABEL_LENGTH]);

	const unsigned int NumExtensionsToCheck = 6;
	const char *extensionNames[NumExtensionsToCheck] = {
		"GL_KHR_debug", "GL_EXT_texture_compression_s3tc", "GL_OES_compressed_ETC1_RGB8_texture",
		"GL_AMD_compressed_ATC_texture", "GL_IMG_texture_compression_pvrtc", "GL_KHR_texture_compression_astc_ldr"
	};

	for (unsigned int i = 0; i < NumExtensionsToCheck; i++)
		glExtensions_[i] = false;

	checkGLExtensions(extensionNames, glExtensions_, NumExtensionsToCheck);
}

void GfxCapabilities::logGLInfo()
{
	LOGI("--- OpenGL device info ---");
	LOGI_X("Vendor: %s", glGetString(GL_VENDOR));
	LOGI_X("Renderer: %s", glGetString(GL_RENDERER));
	LOGI_X("OpenGL Version: %s", glGetString(GL_VERSION));
	LOGI_X("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	LOGI("--- OpenGL device info ---");
}

void GfxCapabilities::logGLExtensions()
{
	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	LOGI("--- OpenGL extensions ---");
	for (GLuint i = 0; i < static_cast<GLuint>(numExtensions); i++)
		LOGI_X("Extension %u: %s", i, glGetStringi(GL_EXTENSIONS, i));
	LOGI("--- OpenGL extensions ---");
}

void GfxCapabilities::logGLCaps() const
{
	LOGI("--- OpenGL device capabilities ---");
	LOGI_X("GL_CONTEXT_FLAGS: %d", glIntValues_[GLIntValues::CONTEXT_FLAGS]);
	LOGI_X("GL_MAX_TEXTURE_SIZE: %d", glIntValues_[GLIntValues::MAX_TEXTURE_SIZE]);
	LOGI_X("GL_MAX_TEXTURE_IMAGE_UNITS: %d", glIntValues_[GLIntValues::MAX_TEXTURE_IMAGE_UNITS]);
	LOGI_X("GL_MAX_UNIFORM_BLOCK_SIZE: %d", glIntValues_[GLIntValues::MAX_UNIFORM_BLOCK_SIZE]);
	LOGI_X("GL_MAX_UNIFORM_BUFFER_BINDINGS: %d", glIntValues_[GLIntValues::MAX_UNIFORM_BUFFER_BINDINGS]);
	LOGI_X("GL_MAX_VERTEX_UNIFORM_BLOCKS: %d", glIntValues_[GLIntValues::MAX_VERTEX_UNIFORM_BLOCKS]);
	LOGI_X("GL_MAX_FRAGMENT_UNIFORM_BLOCKS: %d", glIntValues_[GLIntValues::MAX_FRAGMENT_UNIFORM_BLOCKS]);
	LOGI_X("GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT: %d", glIntValues_[GLIntValues::UNIFORM_BUFFER_OFFSET_ALIGNMENT]);
	LOGI_X("GL_MAX_LABEL_LENGTH: %d", glIntValues_[GLIntValues::MAX_LABEL_LENGTH]);
	LOGI("---");
	LOGI_X("GL_KHR_debug: %d", glExtensions_[GLExtensions::KHR_DEBUG]);
	LOGI_X("GL_EXT_texture_compression_s3tc: %d", glExtensions_[GLExtensions::EXT_TEXTURE_COMPRESSION_S3TC]);
	LOGI_X("GL_OES_compressed_ETC1_RGB8_texture: %d", glExtensions_[GLExtensions::OES_COMPRESSED_ETC1_RGB8_TEXTURE]);
	LOGI_X("GL_AMD_compressed_ATC_texture: %d", glExtensions_[GLExtensions::AMD_COMPRESSED_ATC_TEXTURE]);
	LOGI_X("GL_IMG_texture_compression_pvrtc: %d", glExtensions_[GLExtensions::IMG_TEXTURE_COMPRESSION_PVRTC]);
	LOGI_X("GL_KHR_texture_compression_astc_ldr: %d", glExtensions_[GLExtensions::KHR_TEXTURE_COMPRESSION_ASTC_LDR]);
	LOGI("--- OpenGL device capabilities ---");
}

void GfxCapabilities::checkGLExtensions(const char *extensionNames[], bool results[], unsigned int numExtensionsToCheck) const
{
	GLint numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	for (GLuint i = 0; i < static_cast<GLuint>(numExtensions); i++)
	{
		const char *extension = reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, i));
		const size_t extLength = strlen(extension);

		for (unsigned int j = 0; j < numExtensionsToCheck; j++)
		{
			const size_t nameLength = strlen(extensionNames[j]);
			if (results[j] == false && (nameLength == extLength) && (strncmp(extensionNames[j], extension, extLength) == 0))
				results[j] = true;
		}
	}
}

}
