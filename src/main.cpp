#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdint>
#include <sstream>
#include <regex>
#include <getopt.h>

#include "STL_file.h"
#include "distance_calculator.h"
#include "gcode_file_translator.h"

using namespace std;

int main(int argc, char * argv[])
{
	string stl_file;
	string gcode_file;
	string output_file;
	float stepwidth_value = 0.0f;
	float laser_value = 0.0f;
	int simplify_flag = 0;
	int c;
	int digit_optind = 0;

	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"stl", required_argument, 0, 0},
			{"gcode", required_argument, 0, 0},
			{"output", required_argument, 0, 0},
			{"laser", required_argument, 0, 0},
			{"stepwidth", optional_argument, 0, 0},
			{"simplify", no_argument, &simplify_flag, 1},
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "", long_options, &option_index);

		if (c == -1)
			break;

		switch (c) {
			case 0:
				if (optarg) {
					if(strcmp(long_options[option_index].name, "stl") == 0) {
						stl_file = optarg;
					} else if(strcmp(long_options[option_index].name, "gcode") == 0) {
						gcode_file = optarg;
					} else if(strcmp(long_options[option_index].name, "output") == 0) {
						output_file = optarg;
					} else if(strcmp(long_options[option_index].name, "laser") == 0) {
						try {
							laser_value = stof(optarg);
						} catch (const invalid_argument & e) {
							cerr << "[Error]: Argument for laser is not a valid float. Use --laser=diameter, where diameter is a valid floating point number. Exit." << endl;
							exit(1);
						} catch (const out_of_range & e) {
							cerr << "[Error]: Argument for laser is not a valid float. Use --laser=diameter, where diameter is a valid floating point number. Exit." << endl;
							exit(1);
						}
					} else if(strcmp(long_options[option_index].name, "stepwidth") == 0) {
						try {
							stepwidth_value = stof(optarg);
						} catch (const invalid_argument & e) {
							cout << "[Warning]: Argument for stepwidth is not a valid float. Using default value." << endl;
						} catch (const out_of_range & e) {
							cout << "[Warning]: Argument for stepwidth is not a valid float. Using default value." << endl;
						}
					}
				}

				break;
		}
	}

	int exit_val = 0;

	if(stl_file.empty()) {
		cerr << "[Error]: No STL-file defined, define with --stl=stl_filename" << endl;
		exit_val++;
	}

	if(gcode_file.empty()) {
		cerr << "[Error]: No GCode-file defined, define with --gcode=gcode_filename" << endl;
		exit_val++;
	}

	if(output_file.empty()) {
		cerr << "[Error]: No Output-file defined, define with --output=output_filename" << endl;
		exit_val++;
	}

	if(laser_value < 0.05f) {
		cerr << "[Error]: No or invalid laser diameter defined, must be >= 0.05, define with --laser=laser_diameter" << endl;
		exit_val++;
	}

	if(exit_val > 0) {
		cerr << "[EXIT]: " << exit_val << " arguments missing." << endl;
		exit(exit_val);
	}

	STL_file mySTL = STL_file(stl_file, simplify_flag);
	mySTL.read();
	DistanceCalculator myDistanceCalculator = DistanceCalculator(&mySTL, laser_value);
	gCodeFileTranslator myGCode = gCodeFileTranslator(gcode_file, output_file, &mySTL, &myDistanceCalculator);

	if(stepwidth_value > 0.0f) {
		myGCode.setMinStep(stepwidth_value);
	}

	if(!myGCode.read()) {
		myGCode.addZ();
	} else {
		cerr << "[EXIT]: No GCodeFile found" << endl;
		exit(1);
	}

	cout << "[Info]: Success! Exiting." << endl;
	return 0;
}