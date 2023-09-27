# GCodeZ
A pseudo-3D projection of 2D laser G-code onto a surface derived from an *.stl file.
## Required Command Line Options: 
 **--stl=stl_file** The STL file can be either binary or ASCII format.
 
 **--gcode=gcode_file** The 2D GCode file.

 **--output=output_file** The output file will contain the pseudo-3D code.

 **--laser=laser_diameter** The laser spot diameter is specified in millimeters and must match the diameter used for 2D GCode generation.

## Optional Command Line Options:
 **--stepwith=stepwith** The minimum spatial point-to-point distance in the output is expressed in millimeters, with a default value of 0.5 millimeters. Reducing this value will result in greater precision in laser path tracking along the STL surface but will necessitate a longer computation time.
 
 **--simplify** Consider only facets whose normal vector contains a component in the positive z-direction. Additionally, if a proper range is given in the header of the GCode file, STL facet search is restricted to the area (0, 0) .. (xMax + 0.5, yMax + 0.5). This can lead to increased execution speed in certain models. Ensure that the STL file includes valid normals for each facet. The option does not require any parameters.

## Build
It utilizes std::string.contains(), hence the -std=c++2b compiler flag needs to be provided to g++.
 
## GUI
A basic GUI for Python is accessible at /src/gcodeZ_GUI.py.
![GUI](https://github.com/Rob0xFF/GCodeZ/blob/main/gui.png?raw=true)