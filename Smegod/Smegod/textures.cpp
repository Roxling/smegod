#include "textures.h"

Texture::Texture(string file)
{
	int width;
	int height;
	unsigned char *image = SOIL_load_image((FOLDER+file).c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); 
}