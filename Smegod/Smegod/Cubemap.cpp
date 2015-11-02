#include "cubemap.h"


Cubemap::Cubemap(string prefix, string suffix)
{
	vector<string> textures_faces = { prefix + "posx" + suffix, prefix + "negx" + suffix, prefix + "posy" + suffix, prefix + "negy" + suffix, prefix + "posz" + suffix, prefix + "negz" + suffix };
	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	//TODO: ensure size = 6

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
	for (GLuint i = 0; i < textures_faces.size(); i++) {
		if (!Globals::File_Exists(textures_faces[i])) {
			cout << "File " << textures_faces[i] << " does not exist. Can not load texture in cubemap." << endl;
			return;
		}
		image = SOIL_load_image(textures_faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
			);
		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}
