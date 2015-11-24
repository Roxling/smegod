#pragma once
#include "smegod_dep.h"
#include "core.h"
#include <SOIL.h>
#include <unordered_map>

class DefaultTextures {
public:
	DefaultTextures(GLuint t, GLuint b) : texture(t), bump(b) {}
	GLuint texture;
	GLuint bump;
};

class Texture : public GLResource {
protected:
	static shared_ptr<DefaultTextures> defaultInstance;

	static unique_ptr<unordered_map<string, GLuint>> cache;
	const string FOLDER = "Textures/";

	GLenum format;
	GLenum internalFormat;
	GLenum precision;
	GLenum layout;

	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int multisamples;
	unsigned int layers;
	unsigned int mipLevels;

	bool upload(const unsigned char *source, const int mipLevel = 0) const;

	void destroy();

	bool create(const unsigned char *data);

public:
	~Texture();

	//disable copy
	Texture(const Texture& that) = delete;
	Texture& operator=(Texture const&) = delete;
	Texture() = default;

	GLenum getFormat() const { return format; }
	GLenum getLayout() const { return layout; }


	Texture(string file, bool use_defaultfolder = true);
	Texture(const unsigned char *data, unsigned int width, unsigned int height, unsigned int depth, unsigned int layers, unsigned int msaa,
		unsigned int mipLevels, GLenum format, GLenum internalFormat, GLenum precision);
	static shared_ptr<DefaultTextures> getDefaults();
};

class DepthTexture : public Texture {
public:
	DepthTexture(const unsigned int width, const unsigned int height) : 
		Texture(nullptr, width, height, 0/*depth*/, 0/*layers*/, 1 /*msaa*/, 0/*mipLevels*/, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT) {
		
		glBindTexture(layout, glId);
		GL_CHECK_ERRORS();

		/* Texture options */
		glTexParameteri(layout, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(layout, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(layout, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(layout, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat c[4] = { 1.f, 1.f,1.f, 1.f };
		glTexParameterfv(layout, GL_TEXTURE_BORDER_COLOR, c);

		glBindTexture(layout, 0);
		GL_CHECK_ERRORS();
	}
};

class RenderTexture : public Texture {
public:
	RenderTexture(const unsigned int width, const unsigned int height) :
		Texture(nullptr, width, height, 0/*depth*/, 0/*layers*/, 1 /*msaa*/, 0/*mipLevels*/, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE) {}
};

class FrameBuffer : public GLResource {
public:
	FrameBuffer(vector<Texture *> *colorAttachments, Texture *depthAttachment = nullptr, Texture *stencilAttachment = nullptr, Texture *depthStencilAttachment = nullptr);

	void activate() const;

};