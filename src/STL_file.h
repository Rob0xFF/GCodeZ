#ifndef STL_file_h
#define STL_file_h

#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdint>
#include <regex>

#define PI 3.14159265

class STL_file
{
	public:

		STL_file(std::string filename, uint8_t simplify = 0);

		~STL_file();

		uint8_t read();

		uint32_t numberOfFacets = 0;

		struct vertex {
			float x;
			float y;
			float z;
		};

		struct facet {
			vertex vertex1;
			vertex vertex2;
			vertex vertex3;
		};

		facet * surface;

		vertex * surface_normals;

		uint8_t * useFacet;

	private:

		uint8_t isBinary();

		uint8_t is_binary;

		std::string myFile;

		std::ifstream ifs;

		std::filebuf * pbuf;

		uint32_t size;

		char * buffer;

		char * bufptr;

		uint8_t doSimplify;

};

#endif