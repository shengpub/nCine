#ifndef CLASS_NCINE_GLBUFFEROBJECT
#define CLASS_NCINE_GLBUFFEROBJECT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "GLHashMap.h"

namespace ncine {

/// A class to handle OpenGL buffer objects of different kinds
class GLBufferObject
{
  public:
	explicit GLBufferObject(GLenum target);
	~GLBufferObject();

	inline GLuint glHandle() const { return glHandle_; }
	inline GLenum target() const { return target_; }
	inline GLsizeiptr size() const { return size_; }

	void bind() const;
	void unbind() const;

	void bufferData(GLsizeiptr size, const GLvoid *data, GLenum usage);
	void bufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid *data);

#ifndef __ANDROID__
	void *map(GLenum access);
	GLboolean unmap();

#ifndef __APPLE__
	void bufferStorage(GLsizeiptr size, const GLvoid *data, GLbitfield flags);

	void bindBufferBase(GLuint index);
	void bindBufferRange(GLuint index, GLintptr offset, GLsizei ptrsize);

	void *mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access);
	void flushMappedBufferRange(GLintptr offset, GLsizeiptr length);

	void bindVertexBuffer(GLuint bindingIndex, GLintptr offset, GLsizei stride);
#endif

#endif
  private:
	static class GLHashMap<GLBufferObjectMappingFunc::Size, GLBufferObjectMappingFunc> boundBuffers_;

	struct BufferRange
	{
		BufferRange() : glHandle(0), offset(0), ptrsize(0) { }

		GLuint glHandle;
		GLintptr offset;
		GLsizei ptrsize;
	};

	GLuint glHandle_;
	GLenum target_;
	GLsizeiptr size_;

	static const int MaxIndexBufferRange = 128;
	/// Current bound index for buffer base. Negative if not bound.
	static GLuint boundIndexBase_[MaxIndexBufferRange];
	/// Current range and offset for buffer range index
	static BufferRange boundBufferRange_[MaxIndexBufferRange];

	/// Deleted copy constructor
	GLBufferObject(const GLBufferObject &) = delete;
	/// Deleted assignment operator
	GLBufferObject &operator=(const GLBufferObject &) = delete;
};

}

#endif
