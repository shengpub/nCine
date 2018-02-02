#include "GLShaderUniforms.h"
#include "GLShaderProgram.h"
#include "GLUniformCache.h"
#include "nctl/HashMapIterator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLUniformCache GLShaderUniforms::uniformNotFound_;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderUniforms::GLShaderUniforms()
	: shaderProgram_(nullptr), uniformCaches_(UniformCachesHashSize)
{
}

GLShaderUniforms::GLShaderUniforms(GLShaderProgram *shaderProgram)
	: shaderProgram_(nullptr), uniformCaches_(UniformCachesHashSize)
{
	setProgram(shaderProgram);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderUniforms::setProgram(GLShaderProgram *shaderProgram)
{
	shaderProgram_ = shaderProgram;
	uniformCaches_.clear();
	importUniforms();
}

GLUniformCache *GLShaderUniforms::uniform(const char *name)
{
	ASSERT(name);
	GLUniformCache *uniformCache = nullptr;

	if (shaderProgram_)
	{
		uniformCache = uniformCaches_.find(name);

		if (uniformCache == nullptr)
		{
			// Returning the dummy uniform cache to prevent the application from crashing
			uniformCache = &uniformNotFound_;
			LOGW_X("Uniform \"%s\" not found in shader program %u", name, shaderProgram_->glHandle());
		}
	}
	else
		LOGE_X("Cannot find uniform \"%s\", no shader program associated", name);

	return uniformCache;
}

void GLShaderUniforms::commitUniforms()
{
	if (shaderProgram_)
	{
		shaderProgram_->use();
		forEach(uniformCaches_.begin(), uniformCaches_.end(), [](GLUniformCache &uniform){ uniform.commitValue(); });
	}
	else
		LOGE("No shader program associated");
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderUniforms::importUniforms()
{
	const unsigned int count = shaderProgram_->uniforms_.size();
	if (count > UniformCachesHashSize)
		LOGW_X("More active uniforms (%d) than hashmap buckets (%d)", count, UniformCachesHashSize);

	unsigned int nextFreeFloat = 0;
	unsigned int nextFreeInt = 0;
	for (const GLUniform &uniform : shaderProgram_->uniforms_)
	{
		GLUniformCache uniformCache(&uniform);

		switch (uniform.basicType())
		{
			case GL_FLOAT:
			{
				if (nextFreeFloat < UniformFloatBufferSize)
				{
					uniformCache.setDataPointer(&uniformsFloatBuffer_[nextFreeFloat]);
					nextFreeFloat += uniform.numComponents(); // tight packing, not aligned
				}
				else
					LOGW_X("Shader %u - No more floating point buffer space for uniform \"%s\"", shaderProgram_->glHandle(), uniform.name());

				break;
			}
			case GL_INT:
			{
				if (nextFreeInt < UniformIntBufferSize)
				{
					uniformCache.setDataPointer(&uniformsIntBuffer_[nextFreeInt]);
					nextFreeInt += uniform.numComponents(); // tight packing, not aligned
				}
				else
					LOGW_X("Shader %u - No more integer buffer space for uniform \"%s\"", shaderProgram_->glHandle(), uniform.name());

				break;
			}
		}

		uniformCaches_[uniform.name()] = uniformCache;
	}
}

}
