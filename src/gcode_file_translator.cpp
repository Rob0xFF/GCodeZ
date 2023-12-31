#include "gcode_file_translator.h"

using namespace std;

gCodeFileTranslator::gCodeFileTranslator(string inputFileName, string outputFileName, STL_file * stlFile, DistanceCalculator * calc)
{
	myGCodeFile = inputFileName;
	myOutputFile = outputFileName;
	mySTL_file = stlFile;
	myCalc = calc;
}

gCodeFileTranslator::~gCodeFileTranslator()
{
}

uint8_t gCodeFileTranslator::setMinStep(float minstep)
{
	myMinStep = minstep;

	if(myMinStep <= 0.1f) {
		myMinStep = 0.1f;
	}

	return 0;
}

uint8_t gCodeFileTranslator::read()
{
	ifs.open(myGCodeFile);

	if(!ifs) {
		cerr << "[Error]: File does not exist: " << myGCodeFile << endl;
		return 1;
	}

	ifs.seekg(0, ios::end);
	length = ifs.tellg();
	buffer = new char[length];
	ifs.seekg(0, ios::beg);
	ifs.read(buffer, length);
	ifs.close();
	return 0;
}

uint8_t gCodeFileTranslator::addZ()
{
	string tmpstr(buffer, length);
	stringstream myStream(tmpstr);
	string thisline;
	size_t lines = 0;

	while(getline(myStream, thisline)) {
		lines++;
	}

	cout << "[Info]: " << lines << " lines of GCode to process." << endl;
	myStream.clear();
	myStream.seekg(0, ios::beg);
	regex range("([-]?[0-9]*\\.?[0-9]+)\\s*\\.*\\s*([-]?[0-9]*\\.?[0-9]+)");
	regex lineNumber("N\\d+\\s*");
	regex coordX("X([-]?[0-9]*\\.?[0-9]+)");
	regex coordY("Y([-]?[0-9]*\\.?[0-9]+)");
	regex coordZ("Z([-]?[0-9]*\\.?[0-9]+)\\s+");
	smatch matchRange, matchLineNumber, matchX, matchY;
	float thisX, thisY, lastX = 0.0f, lastY = 0.0f;
	ofstream out(myOutputFile);
	size_t line = 0;
	size_t lineOut = 1;
	uint8_t addLineNumber = 0;
	uint8_t passed[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t index = 0;
	float xRange[2] = {0.0f, 0.0f};
	float yRange[2] = {0.0f, 0.0f};
	cout << "[Info]: Processing .. 0%" << endl;

	while(getline(myStream, thisline)) {
		line++;
		string replacementX, replacementY, outStr;
		
		if(regex_search(thisline, matchLineNumber, lineNumber)) {
			if(matchLineNumber[0].matched) {
				thisline = regex_replace(thisline, lineNumber, "");
				addLineNumber = 1;
			}
		}

		if(thisline.contains("X range:")) {
			if(regex_search(thisline, matchRange, range)) {
				xRange[0] = stof(matchRange[1].str());
				xRange[1] = stof(matchRange[2].str());
				mySTL_file->setXRange(xRange);
			}
		}

		if(thisline.contains("Y range:")) {
			if(regex_search(thisline, matchRange, range)) {
				yRange[0] = stof(matchRange[1].str());
				yRange[1] = stof(matchRange[2].str());
				mySTL_file->setYRange(yRange);
			}
		}

		if((float) line / (float) lines >= ((float) index + 1.0f) / 10.0f && passed[index] == 0) {
			cout << "[Info]: Processing .. " << 10.0f * ((float) index + 1.0) << "%" << endl;
			passed[index] = 1;
			index++;
		}

		if(thisline.contains("G00 ") || thisline.contains("G01 ") || thisline.contains("G0 ") || thisline.contains("G1 ")) {
			string xValue = "";
			string yValue = "";

			uint8_t newCoord = 0;

			if(regex_search(thisline, matchX, coordX)) {
				xValue = matchX[1].str();
				thisX = stof(xValue);
				newCoord = 1;
			}

			if(regex_search(thisline, matchY, coordY)) {
				yValue = matchY[1].str();
				thisY = stof(yValue);
				newCoord = 1;
			}

			if(newCoord) {
				// we have to approach towards the next point with stepwidth
				float distFromLastPoint = sqrt(pow(thisX - lastX, 2) + pow(thisY - lastY, 2));

				while(distFromLastPoint > myMinStep) {
					float norm = 1 / sqrt(pow(thisX - lastX, 2) + pow(thisY - lastY, 2));
					lastX = lastX + myMinStep * (thisX - lastX) * norm;
					lastY = lastY + myMinStep * (thisY - lastY) * norm;
					float orig[3] = {lastX, lastY, myCalc->maxZ};
					distFromLastPoint = sqrt(pow(thisX - lastX, 2) + pow(thisY - lastY, 2));
					myCalc->findNearestIntersection(orig);
					replacementX = "X" + to_string(lastX);
					replacementY = "Y" + to_string(lastY) + " Z" + to_string(myCalc->calculateLaserDistance());
					outStr = "";
					if(addLineNumber) {
						outStr = "N" + to_string(lineOut) + " ";
						lineOut++;
					}
					outStr += thisline;
					outStr = regex_replace(outStr, coordZ, "");
					outStr = regex_replace(outStr, coordX, replacementX);
					outStr = regex_replace(outStr, coordY, replacementY);
					out << outStr << endl;
				}

				float orig[3] = {thisX, thisY, myCalc->maxZ};
				myCalc->findNearestIntersection(orig);
				replacementX = "X" + to_string(thisX);
				replacementY = "Y" + to_string(thisY) + " Z" + to_string(myCalc->calculateLaserDistance());
				outStr = "";
				if(addLineNumber) {
					outStr = "N" + to_string(lineOut) + " ";
					lineOut++;
				}
				outStr += thisline;
				outStr = regex_replace(outStr, coordZ, "");
				outStr = regex_replace(outStr, coordX, replacementX);
				outStr = regex_replace(outStr, coordY, replacementY);
				out << outStr << endl;
				lastX = thisX;
				lastY = thisY;
				newCoord = 0;
			} else {
				outStr = "";
				if(addLineNumber) {
					outStr = "N" + to_string(lineOut) + " ";
					lineOut++;
				}
				outStr += thisline;
				out << outStr << endl;
			}
		} else {
			outStr = "";
			if(addLineNumber) {
				outStr = "N" + to_string(lineOut) + " ";
				lineOut++;
			}
			outStr += thisline;
			out << outStr << endl;
		}
	}

	out.close();

	if(myCalc->pointsOutside) {
		cout << "[Warning]: Found " << myCalc->pointsOutside << " points with no intersection with the surface. Please verify." << endl;
	}

	if(myCalc->pointsTooLow) {
		cout << "[Warning]: Found " << myCalc->pointsTooLow << " points below z-axis range. Please verify." << endl;
	}

	cout << "[Info]: Written output file " << myOutputFile << endl;
	return 0;
}