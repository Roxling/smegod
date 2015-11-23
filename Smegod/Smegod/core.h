#pragma once

#include "smegod_dep.h"

class GLResource {
protected:
	GLuint glId;
public:
	GLuint getGlId() const { return glId; }
};

#define DEBUG_LEVEL (0)

#if DEBUG_LEVEL >= 1
#	define GL_CHECK_ERRORS()				check_errors(__FILE__, __FUNCTION__, __LINE__)
#else
#	define GL_CHECK_ERRORS()
#endif

/* Ensures that GL hasn't generated any errors */
bool check_errors(const string file, const string function, int line);

