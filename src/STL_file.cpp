/*
	Parts of the code (C) Jerry Greenough, used and published with friendly permission.
	https://jgxsoft.com/examples/STL%20Reader/STL%20Reader.html
*/

#include "STL_file.h"

using namespace std;

STL_file::STL_file(string filename)
{
    myFile = filename;
}

STL_file::~STL_file()
{

}

uint8_t STL_file::read()
{
    ifs.open(myFile);

    if(!ifs) {
        cerr << "[Error]: File does not exist: " << myFile << endl;
        exit(1);
    }

    pbuf = ifs.rdbuf();
    size = pbuf -> pubseekoff(0, ifs.end);
    pbuf -> pubseekpos(0);
    buffer = new char[(size_t) size];
    pbuf -> sgetn(buffer, size);

    if(!isBinary()) {
        cout << "[Info]: STL file seems to be ASCII: " << myFile << endl;
        is_binary = 0;
        //exit(2);
    } else {
        cout << "[Info]: STL file seems to be binary: " << myFile << endl;
        is_binary = 1;
    }

    uint32_t nFacet = 0;

    if(is_binary) {

        bufptr = buffer;
        bufptr += 80;
        numberOfFacets = * (uint32_t * )(bufptr);
        cout << "[Info]: " << numberOfFacets << " facets to read from file." << endl;
        bufptr += 4;

        vertex normal;
        vertex v1, v2, v3;

        surface = new facet[numberOfFacets];

        while (bufptr < buffer + size) {

            normal.x = * (float * )(bufptr);
            normal.y = * (float * )(bufptr + 4);
            normal.z = * (float * )(bufptr + 8);
            bufptr += 12;

            v1.x = * (float * )(bufptr);
            v1.y = * (float * )(bufptr + 4);
            v1.z = * (float * )(bufptr + 8);
            bufptr += 12;

            v2.x = * (float * )(bufptr);
            v2.y = * (float * )(bufptr + 4);
            v2.z = * (float * )(bufptr + 8);
            bufptr += 12;

            v3.x = * (float * )(bufptr);
            v3.y = * (float * )(bufptr + 4);
            v3.z = * (float * )(bufptr + 8);
            bufptr += 12;

            //const float eps = (float) 1.0e-9;

            /*if (abs(normal.x) < eps && abs(normal.y) < eps && abs(normal.z) < eps) {
            	vertex u, v;
            	u = v2 - v1;
            	v = v3 - v1;
            	normal = u ^ v;
            	unit(normal);
            }*/

            surface[nFacet] = {
                v1,
                v2,
                v3
            };

            nFacet++;

            bufptr += 2;
        }
    } else {
        string tmpstr(buffer, size);
        stringstream myStream(tmpstr);
        string thisline;

        vertex normal;

        regex find_xyz("([-]?[0-9]*\\.?[0-9]+)\\s*([-]?[0-9]*\\.?[0-9]+)\\s*([-]?[0-9]*\\.?[0-9]+)");
        smatch match_xyz;

        uint32_t lines = 0;
        while (getline(myStream, thisline)) {
            lines++;
        }
        cout << "[Info]: " << lines << " lines in STL to process." << endl;
        myStream.clear();
        myStream.seekg(0,ios::beg);

        while (getline(myStream, thisline)) {
            if(thisline.contains("facet normal")) {
                if(regex_search(thisline, match_xyz, find_xyz)) {
                    normal.x = stof(match_xyz[1].str());
                    normal.y = stof(match_xyz[2].str());
                    normal.z = stof(match_xyz[3].str());
                }
                numberOfFacets++;
            }
        }

        myStream.clear();
        myStream.seekg(0,ios::beg);

        vertex v1, v2, v3;
        surface = new facet[numberOfFacets];

        char nextFacet = '0';

        while (getline(myStream, thisline)) {
            if(thisline.contains("outer loop")) {
                nextFacet = '1';
            }
            if(thisline.contains("vertex")) {
                if(nextFacet =='1') {
                    if(regex_search(thisline, match_xyz, find_xyz)) {
                        v1.x = stof(match_xyz[1].str());
                        v1.y = stof(match_xyz[2].str());
                        v1.z = stof(match_xyz[3].str());
                    }
                    nextFacet = '2';
                } else if(nextFacet =='2') {
                    if(regex_search(thisline, match_xyz, find_xyz)) {
                        v2.x = stof(match_xyz[1].str());
                        v2.y = stof(match_xyz[2].str());
                        v2.z = stof(match_xyz[3].str());
                    }
                    nextFacet = '3';
                } else if(nextFacet =='3') {
                    if(regex_search(thisline, match_xyz, find_xyz)) {
                        v3.x = stof(match_xyz[1].str());
                        v3.y = stof(match_xyz[2].str());
                        v3.z = stof(match_xyz[3].str());
                    }
                }
            }
            if(thisline.contains("endloop")) {
                surface[nFacet] = {
                    v1,
                    v2,
                    v3
                };

                nFacet++;
                nextFacet = '0';
            }
        }
    }

    ifs.close();

    delete[] buffer;

    cout << "[Info]: Read " << nFacet << " facets from file." << endl;

    return 0;
}

uint8_t STL_file::isBinary()
{
    uint8_t bbinary = 1;
    size_t spnsz, spnsz0;
    spnsz = strspn(buffer, " ");
    char ctstr[6];
    strncpy_s(ctstr, & buffer[spnsz], 5);
    ctstr[5] = '\0';
    char csolid[] = "solid\0";

    if (!strcmp(ctstr, csolid)) {
        spnsz0 = 5 + spnsz;
        char * pch = strchr( & buffer[spnsz0], '\n');
        if (pch) {
            pch++;
            spnsz = strspn(pch, " ");
            spnsz0 = spnsz;
            spnsz = strcspn(pch + spnsz0, " ");
            if (spnsz == 5) {
                strncpy_s(ctstr, pch + spnsz0, 5);
                ctstr[5] = '\0';
                char cfacet[] = "facet\0";
                if (!strcmp(ctstr, cfacet)) {
                    bbinary = 0;
                }
            }
        }
    }
    return (bbinary);
}
