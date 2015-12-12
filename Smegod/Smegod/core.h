#pragma once

#include "smegod_dep.h"
#include <cassert>
#include <chrono>

class GLResource {
protected:
	GLuint glId = 0;
public:
	GLuint getGlId() const { return glId; }
};

// 1 check all gl calls
// 2 + check uniforms
// 3 + query performance
#define DEBUG_LEVEL (3)

#if DEBUG_LEVEL >= 1
#	define GL_CHECK_ERRORS() check_errors(__FILE__, __FUNCTION__, __LINE__ , "No message")
#   define GL_CHECK_ERRORS_MSG(msg) check_errors(__FILE__, __FUNCTION__, __LINE__, msg)
#else
#	define GL_CHECK_ERRORS()
#   define GL_CHECK_ERRORS_MSG(msg)
#endif

#if DEBUG_LEVEL >= 3
#	define PERF_START(p)	dbgPassPerf.start(p)
#	define PERF_END(p)	    dbgPassPerf.end(p)
#	define PERF_INIT()		PassPerf dbgPassPerf
#	define PERF_PRINT()		dbgPassPerf.printLast()
#else
#	define PERF_START(p)
#	define PERF_END(p)
#	define PERF_INIT()
#	define PERF_PRINT()
#endif

/* Ensures that GL hasn't generated any errors */
bool check_errors(const string file, const string function, int line);
bool check_errors(const string file, const string function, int line, string msg);

class PassPerf {
public:
	enum Pass {
		FULL_PASS = 0,
		GEOMETRY_PASS,
		FULL_LIGHT_PASS,
		BLOOM_PASS,
		RESOLVE_PASS,
		QUAD_PASS,
		SWAP_PASS,
		NUMBER_PASSES
	};

	PassPerf() {
		glGenQueries(NUMBER_PASSES-1, queries);
		GL_CHECK_ERRORS();
	}


	void start(Pass p) {
		glBeginQuery(GL_TIME_ELAPSED, queries[p-1]);
	}

	void end(Pass p) {
		glEndQuery(GL_TIME_ELAPSED);
	}

	void printLast() {
		GLint available = 0;
		GLuint64 timeElapsed = 0;
		while (!available) {
			glGetQueryObjectiv(queries[NUMBER_PASSES - 2], GL_QUERY_RESULT_AVAILABLE, &available);
		}

		long double total = 0;

		for (int i = 0; i < NUMBER_PASSES-1; ++i) {
			glGetQueryObjectui64v(queries[i], GL_QUERY_RESULT, &timeElapsed);
			long double d = (long double)timeElapsed / 1e6;
			total += d;
			hist[i+1] = d;
		}
		hist[0] = total;

		if (header++ > 20) {
			cout << "Full      Geo     Lght+Shdow  Bloom    Resolve    Quad    Swap     FPS" << endl;
			header = 0;
		}


		streamsize old = cout.precision();
		cout.precision(4);
		cout << fixed;
		for (int i = 0; i < NUMBER_PASSES; ++i) {
			cout << hist[i] << "ms ";
		}
		cout << (double long)1000 / (hist[0]) << defaultfloat << endl;
		cout.precision(old);
	}

private:
	unsigned short header = 0;
	GLuint queries[NUMBER_PASSES];
	long double hist[NUMBER_PASSES];
};