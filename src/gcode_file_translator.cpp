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

	if (myMinStep <= 0.1f) {
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
	uint32_t lines = 0;

	while (getline(myStream, thisline)) {
		lines++;
	}

	cout << "[Info]: " << lines << " lines of GCode to process." << endl;
	myStream.clear();
	myStream.seekg(0, ios::beg);
	regex gCode("(G[0]?[0,1])\\s");
	regex range("([-]?[0-9]*\\.?[0-9]+)\\s*\\.*\\s*([-]?[0-9]*\\.?[0-9]+)");
	regex coordX("X([-]?[0-9]*\\.?[0-9]+)");
	regex coordY("Y([-]?[0-9]*\\.?[0-9]+)");
	regex feedRate("F[-]?[0-9]*\\.?[0-9]+");
	smatch matchG, matchRange, matchX, matchY, matchF;
	float thisX, thisY, lastX = 0.0f, lastY = 0.0f;
	ofstream out(myOutputFile);
	uint32_t line = 0;
	uint8_t passed[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t index = 0;
	float xRange[2] = {0.0f, 0.0f};
	float yRange[2] = {0.0f, 0.0f};
	cout << "[Info]: Processing .. 0%" << endl;

	while (getline(myStream, thisline)) {
		line++;

		if (thisline.contains("X range:")) {
			if(regex_search(thisline, matchRange, range)) {
				xRange[0] = stof(matchRange[1].str());
				xRange[1] = stof(matchRange[2].str());
				mySTL_file->setXRange(xRange);
			}
		}

		if (thisline.contains("Y range:")) {
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

		if (thisline.contains("G00 ") || thisline.contains("G01 ") || thisline.contains("G0 ") || thisline.contains("G1 ")) {
			string gCodeValue = "";
			string feedRateValue = "";
			string xValue = "";
			string yValue = "";

			if(regex_search(thisline, matchG, gCode)) {
				gCodeValue = matchG[1].str();
				//cout << gCodeValue << endl;
			}

			if(regex_search(thisline, matchF, feedRate)) {
				feedRateValue = matchF[0].str();
				//cout << feedRateValue << endl;
			}

			uint8_t newCoord = 0;

			if (regex_search(thisline, matchX, coordX)) {
				xValue = matchX[1].str();
				thisX = stof(xValue);
				newCoord = 1;
			}

			if (regex_search(thisline, matchY, coordY)) {
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
					out << gCodeValue << " X" << lastX << " Y" << lastY << " Z" << myCalc->calculateLaserDistance() << " " << feedRateValue << endl;
				}

				float orig[3] = {thisX, thisY, myCalc->maxZ};
				myCalc->findNearestIntersection(orig);
				out << gCodeValue << " X" << thisX << " Y" << thisY << " Z" << myCalc->calculateLaserDistance() << " " << feedRateValue << endl;
				lastX = thisX;
				lastY = thisY;
				newCoord = 0;
			} else {
				out << thisline << endl;
			}
		} else {
			out << thisline << endl;
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