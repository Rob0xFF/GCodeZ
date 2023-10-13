#ifndef gcode_h
#define gcode_h

#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdint>
#include <sstream>
#include <regex>

#include "STL_file.h"
#include "distance_calculator.h"


class gCodeFileTranslator
{
	public:

		gCodeFileTranslator(std::string inputFileName, std::string outputFileName, STL_file * stlFile, DistanceCalculator * calc);

		~gCodeFileTranslator();

		uint8_t setMinStep(float minstep);

		size_t length = 0;

		uint8_t read();

		uint8_t addZ();

	private:

		std::string myGCodeFile;

		std::string myOutputFile;

		STL_file * mySTL_file;

		DistanceCalculator * myCalc;

		std::ifstream ifs;

		char * buffer;

		float myMinStep = 0.5f;
};

#endif