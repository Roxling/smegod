#include "textures.h"

unique_ptr<unordered_map<string, GLuint>> Texture::cache = make_unique<unordered_map<string, GLuint>>();

shared_ptr<DefaultTextures> Texture::defaultInstance;


shared_ptr<DefaultTextures> Texture::getDefaults()
{
	if (!Texture::defaultInstance) {
		Texture def("notex.png");
		Texture def_bump("nobump.png");
		Texture::defaultInstance = make_shared<DefaultTextures>(def.texture_id, def_bump.texture_id);
	}
	return Texture::defaultInstance;
}


Texture::Texture(string file, bool use_defaultfolder)
{
	if (use_defaultfolder) {
		file = FOLDER + file;
	}

	auto lookup = cache->find(file);
	if (lookup != cache->end()) {
		texture_id = lookup->second;
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

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	/* Texture options */
	if (channels > 3) { //Has alpha channel. Use clamp to edge to avoid wierd borders.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); 


	cache->insert({ file, texture_id });
}