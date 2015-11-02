#include "textures.h"

shared_ptr<Texture> Texture::DEFAULT = make_shared<Texture>();

Texture::Texture(string file)
{
	if (!Globals::File_Exists(FOLDER+file)) {
		cout << "File " << FOLDER + file << " does not exist. Can not load texture." << endl;
		return;
	}
	int width;
	int height;
	unsigned char *image = SOIL_load_image((FOLDER+file).c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	/* Texture options */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); 
}