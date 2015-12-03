#include "textures.h"

unique_ptr<unordered_map<string, GLuint>> Texture::cache = make_unique<unordered_map<string, GLuint>>();

shared_ptr<DefaultTextures> Texture::defaultInstance;

shared_ptr<DefaultTextures> Texture::getDefaults()
{
	if (!Texture::defaultInstance) {
		Texture def("notex.png");
		Texture def_bump("nobump.png");
		Texture::defaultInstance = make_shared<DefaultTextures>(def.getGlId(), def_bump.getGlId());
	}
	return Texture::defaultInstance;
}


Texture::~Texture()
{
	//destroy();
}

void Texture::destroy()
{
	if (glId != 0) {
		glDeleteTextures(1, &glId);
		GL_CHECK_ERRORS();
		glId = 0;
	}
}


bool Texture::upload(const unsigned char *source, const int mipLevel) const
{
	glBindTexture(layout, glId);
	GL_CHECK_ERRORS();

	switch (layout) {
	case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
		glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, multisamples, internalFormat, width, height, layers, GL_TRUE);
		break;
	case GL_TEXTURE_2D_MULTISAMPLE:
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisamples, internalFormat, width, height, GL_TRUE);
		break;
	case GL_TEXTURE_3D:
		glTexImage3D(GL_TEXTURE_3D, mipLevel, internalFormat, width, height, depth, 0, format, precision, source);
		break;
	case GL_TEXTURE_2D:
		glTexImage2D(GL_TEXTURE_2D, mipLevel, internalFormat, width, height, 0, format, precision, source);
		break;
	case GL_TEXTURE_1D:
		glTexImage1D(GL_TEXTURE_1D, mipLevel, internalFormat, width, 0, format, precision, source);
		break;
	case GL_TEXTURE_2D_ARRAY:
		glTexImage3D(GL_TEXTURE_2D_ARRAY, mipLevel, internalFormat, width, height, layers, 0, format, precision, source);
		break;
	case GL_TEXTURE_1D_ARRAY:
		glTexImage2D(GL_TEXTURE_1D_ARRAY, mipLevel, internalFormat, width, height, 0, format, precision, source);
		break;
	default:
		cout << "Invalid texture format: 0x" << hex << layout << dec << endl;
		return false;
	}

	GL_CHECK_ERRORS();
	return true;
}

Texture::Texture(string file, bool use_defaultfolder)
{
	if (use_defaultfolder) {
		file = FOLDER + file;
	}

	auto lookup = cache->find(file);
	if (lookup != cache->end()) {
		glId = lookup->second;
		return;
	}

	if (!Globals::File_Exists(file)) {
		cout << "File " << file << " does not exist. Can not load texture." << endl;
		return;
	}
	int width;
	int height;
	int channels;
	unsigned char *image = SOIL_load_image((file).c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
	
	
	this->width = width;
	this->height = height;
	this->depth = 0;
	this->layers = 0;
	this->multisamples = 1;

	this->mipLevels = 3;

	this->internalFormat = GL_RGBA;
	this->format = GL_RGBA;
	this->precision = GL_UNSIGNED_BYTE;

	//cout << "Loading " << (file).c_str() << " size " << width << "x" << height << endl;

	create(image);

	glBindTexture(layout, glId);
	GL_CHECK_ERRORS();

	/* Texture options */
	glTexParameteri(layout, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(layout, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(layout, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(layout, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(layout, 0);
	GL_CHECK_ERRORS();

	SOIL_free_image_data(image);

	cache->insert({ file, glId });
}

Texture::Texture(const unsigned char *data, unsigned int w, unsigned int h, unsigned int d, unsigned int l, unsigned int msaa, 
	unsigned int mipLvls, GLenum format, GLenum internalFormat, GLenum precision) : 
		width(w), height(h), depth(d), layers(l), multisamples(msaa), mipLevels(mipLvls)
{
	this->format = format;
	this->internalFormat = internalFormat;
	this->precision = precision;
	create(data);
}

bool Texture::create(const unsigned char *data)
{
	glGenTextures(1, &glId);
	GL_CHECK_ERRORS();

	//Sanity checks
	assert(width > 0);
	if (height == 0) { // no height -> 1D
		assert(depth == 0);
		assert(multisamples == 0);
		if (layers > 0) {
			layout = GL_TEXTURE_1D_ARRAY;
		}
		else {
			layout = GL_TEXTURE_1D;
		}
	}
	else if (depth == 0) { // no depth -> 2D
		if (layers > 0) {
			if (multisamples > 1) {
				assert(mipLevels == 0);
				layout = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
			}
			else {
				layout = GL_TEXTURE_2D_ARRAY;
			}
		}
		else {
			if (multisamples > 1) {
				assert(mipLevels == 0);
				layout = GL_TEXTURE_2D_MULTISAMPLE;
			}
			else {
				layout = GL_TEXTURE_2D;
			}
		}
	}
	else { // ovtherwise -> 3D
		assert(layers == 0);
		assert(multisamples == 0);
		layout = GL_TEXTURE_3D;
	}

	glBindTexture(layout, glId);
	GL_CHECK_ERRORS();
	glTexParameteri(layout, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(layout, GL_TEXTURE_MAX_LEVEL, mipLevels);

	//detect format
	//this->internalFormat;

	// For transfers of depth, stencil, or depth/stencil data, you must use: 
	//  GL_DEPTH_COMPONENT, GL_STENCIL_INDEX, or GL_DEPTH_STENCIL
	// For transfers of normalized integer or floating-point color image data, you must use one of the following: 
	//	GL_RED, GL_GREEN, GL_BLUE, GL_RG, GL_RGB, GL_BGR, GL_RGBA, and GL_BGRA. 
	// For transfers of non-normalized integer data, you must use one of the following: 
	//	GL_RED_INTEGER, GL_GREEN_INTEGER, GL_BLUE_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER, and GL_BGRA_INTEGER.
	//this->format;


	// GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, 
	//  GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, 
	//  GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV
	//this->precision;

	if (!upload(data)) {
		destroy();
		return false;
	}

	if (mipLevels > 0)
		glGenerateMipmap(layout);

	glBindTexture(layout, 0);
	GL_CHECK_ERRORS();

	return true;
}

ArrayTexture::ArrayTexture(string tmpl, int num, const unsigned int width, const unsigned int height, GLenum format, GLenum internalFormat, GLenum precision) {
	this->width = width;
	this->height = height;

	this->format = format;
	this->internalFormat = internalFormat;
	this->precision = precision;
	this->layout = GL_TEXTURE_2D_ARRAY;
	mipLevels = 10;
	this->layers = num;

	glGenTextures(1, &glId);
	GL_CHECK_ERRORS();
	glBindTexture(layout, glId);
	GL_CHECK_ERRORS();

	glTexParameteri(layout, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(layout, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(layout, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(layout, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(layout, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(layout, GL_TEXTURE_MAX_LEVEL, mipLevels);

	glTexStorage3D(layout, mipLevels, precision, width, height, layers);

	tmpl = FOLDER + tmpl;

	for (int i = 0; i < num; i++) {
		char buff[1024];
		snprintf(buff, sizeof(buff), tmpl.c_str(), i);
		string file = buff;

		if (!Globals::File_Exists(file)) {
			cout << "File " << file << " does not exist. Can not load texture." << endl;
			return;
		}


		int load_width;
		int load_height;
		int load_channels;
		unsigned char *image = SOIL_load_image((file).c_str(), &load_width, &load_height, &load_channels, format == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);

		//cout << file << " " << width << "x" << height << " (" << load_channels << ") " << endl;

		assert(width == load_width);
		assert(height == load_height);

		glTexSubImage3D(layout, 0, 0, 0, i, width, height, 1, format, internalFormat, image);
		SOIL_free_image_data(image);
	}

	if (mipLevels > 0)
		glGenerateMipmap(layout);

	glBindTexture(layout, 0);
	GL_CHECK_ERRORS();
}

FrameBuffer::FrameBuffer(vector<Texture*> *colorAttachments, Texture *depthAttachment, Texture *stencilAttachment, Texture *depthStencilAttachment)
{
	glGenFramebuffers(1, &glId);
	GL_CHECK_ERRORS();
	glBindFramebuffer(GL_FRAMEBUFFER, glId);
	GL_CHECK_ERRORS();

	vector<GLenum> attachments;
	if (colorAttachments) {
		unsigned int i = 0;
		for (auto it = colorAttachments->begin(); it != colorAttachments->end(); ++it, i++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, (*it)->getLayout(), (*it)->getGlId(), 0);
			GL_CHECK_ERRORS();
			GLint result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (result != GL_FRAMEBUFFER_COMPLETE) {
				cout << "Failed to create frame buffer object: Color attachment #" << i << endl;
			}
			attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		
	}
	if (depthAttachment) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, (GLenum)depthAttachment->getLayout(), depthAttachment->getGlId(), 0);
		GL_CHECK_ERRORS();
		GLint result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (result != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Failed to create frame buffer object: Depth attachment" << endl;
		}
	}
	if (stencilAttachment) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, (GLenum)stencilAttachment->getLayout(), stencilAttachment->getGlId(), 0);
		GL_CHECK_ERRORS();
		GLint result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (result != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Failed to create frame buffer object: Stencil attachment" << endl;
		}
	}
	if (depthStencilAttachment) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, (GLenum)depthStencilAttachment->getLayout(), depthStencilAttachment->getGlId(), 0);
		GL_CHECK_ERRORS();
		GLint result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (result != GL_FRAMEBUFFER_COMPLETE) {
			cout << "Failed to create frame buffer object: Depth stencil attachment" << endl;
		}
	}
	GLsizei size =(GLsizei) attachments.size();
	if (size > 0) {
		glDrawBuffers(size, attachments.data());
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GL_CHECK_ERRORS();
}

void FrameBuffer::activate() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, glId);
	GL_CHECK_ERRORS();
}
