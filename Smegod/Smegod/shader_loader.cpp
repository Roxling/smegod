#include "shader_loader.h"
#include <fstream>
#include <streambuf>
#include <iostream>

using namespace std;

const string FOLDER = "Shaders/";

int load_and_compile(std::string file) {
	ifstream stream(FOLDER+file);
	string shader_string((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
	cout << shader_string << endl;
	return 0;
}