#pragma once
#include "smegod_dep.h"
#include "core.h"
#include <SOIL.h>
#include <gli/gli.hpp>
#include <unordered_map>

class Texture;

class DefaultTextures {
public:
	DefaultTextures(shared_ptr<Texture> t, shared_ptr<Texture> b, shared_ptr<Texture> s) : texture(t), bump(b), spec(s) {}
	shared_ptr<Texture> texture;
	shared_ptr<Texture> bump;
	shared_ptr<Texture> spec;
};

class Texture : public GLResource {
protected:
	static shared_ptr<DefaultTextures> defaultInstance;

	static unique_ptr<unordered_map<string, shared_ptr<Texture>>> cache;
	const string FOLDER = "Textures/";

	GLenum format;
	GLenum internalFormat;
	GLenum precision;
	GLenum target;

	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int multisamples;
	unsigned int layers;
	unsigned int mipLevels;

	bool upload(const unsigned char *source, const int mipLevel = 0) const;

	void destroy();

	bool create(const unsigned char *data);

	Texture(string file, bool use_defaultfolder = true, bool SRGBA = false);
public:
	~Texture();

	static shared_ptr<Texture> loadFromFile(string file, bool use_defaultfolder = true, bool SRGBA = false);

	//disable copy
	Texture(const Texture& that) = delete;
	Texture& operator=(Texture const&) = delete;
	Texture() = default;

	GLenum getFormat() const { return format; }
	GLenum getTarget() const { return target; }


	Texture(const unsigned char *data, unsigned int width, unsigned int height, unsigned int depth, unsigned int layers, unsigned int msaa,
		unsigned int mipLevels, GLenum format, GLenum internalFormat, GLenum precision);
	static shared_ptr<DefaultTextures> getDefaults();
};

class ArrayTexture : public Texture {
public:
	ArrayTexture(string tmpl, int num, const unsigned int width, const unsigned int height, GLenum format, GLenum internalFormat, GLenum precision);
};


class DDSTexture : public Texture {
public:
	DDSTexture(string filename);
};

class TestTexture : public Texture {
public:
	TestTexture() {
		GLuint texture = 0;

		GLsizei width = 2;
		GLsizei height = 2;
		GLsizei layerCount = 2;
		GLsizei mipLevelCount = 1;

		//Read you texels here. In the current example, we have 2*2*2 = 8 texels, with each texel being 4 GLubytes.
		GLubyte texels[16] =
		{
			//Texels for first image.
			0,   0,   0,   255,
			255, 0,   0,   255,
			0,   255, 0,   255,
			0,   0,   255, 255
		};
		GLubyte texels2[16] =
		{
			//Texels for second image.
			255, 255, 255, 255,
			255, 255,   0, 255,
			0,   255, 255, 255,
			255, 0,   255, 255
		};

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
		//Allocate the storage.
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, width, height, layerCount);
		//Upload pixel data.
		//The first 0 refers to the mipmap level (level 0, since there's only 1)
		//The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
		//The final 0 refers to the layer index offset (we start from index 0 and have 2 levels).
		//Altogether you can specify a 3D box subset of the overall texture, but only one mip level at a time.
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, texels);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, texels2);

		//Always set reasonable texture parameters
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		this->glId = texture;
		this->target = GL_TEXTURE_2D_ARRAY;
	}
};

class DepthTexture : public Texture {
public:
	DepthTexture(const unsigned int width, const unsigned int height) : 
		Texture(nullptr, width, height, 0/*depth*/, 0/*layers*/, 1 /*msaa*/, 0/*mipLevels*/, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT) {
		
		glBindTexture(target, glId);
		GL_CHECK_ERRORS();

		/* Texture options */
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat c[4] = { 1.f, 1.f,1.f, 1.f };
		glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, c);

		glBindTexture(target, 0);
		GL_CHECK_ERRORS();
	}
};

class RenderTexture : public Texture {
public:
	RenderTexture(const unsigned int width, const unsigned int height, GLenum format, GLenum internalFormat, GLenum precision) :
		Texture(nullptr, width, height, 0/*depth*/, 0/*layers*/, 1 /*msaa*/, 0/*mipLevels*/, format, internalFormat, precision) {

		glBindTexture(target, glId);
		GL_CHECK_ERRORS();

		/* Texture options */
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(target, 0);
		GL_CHECK_ERRORS();
	}
};

class FrameBuffer : public GLResource {
public:
	FrameBuffer(vector<shared_ptr<Texture>> &colorAttachments, shared_ptr<Texture> depthAttachment = nullptr, shared_ptr<Texture> stencilAttachment = nullptr, shared_ptr<Texture> depthStencilAttachment = nullptr);

	void activate() const;

	static void printFramebufferLimits() {
		int res;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &res);
		cout << "Max Color Attachments: " << res << endl;

		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &res);
		cout << "Max Framebuffer Width: " << res << endl;

		glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &res);
		cout << "Max Framebuffer Height: " << res << endl;

		glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &res);
		cout << "Max Framebuffer Samples: " << res << endl;

		glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &res);
		cout << "Max Framebuffer Layers: " << res << endl;
	}

};