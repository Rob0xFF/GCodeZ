# GCodeZ
A pseudo-3D projection of 2D laser G-code onto a surface derived from an *.stl file. It will add z-coordinates to each linear movement command (G00 and G01). If the distance between two points within the xy-plane exceeds the value given by stepwith, the linear path will be split up to allow precise laser path tracking along the surface. 
# Coordinates
The machine coodinates are right-handed, where allowed values of x and y are positive and z is negative. When z = 0, the laser is at its highest possible point. The maximum z value is calculated using the laser diameter and the calibration line, representing an exposure of the machine bed with the specified diameter. The workpiece coordinates (from STL) are right handed, where x, y and z are positive. If the workpiece at any point given in the 2D GCode is higher than the maximum z value, the conversion stops in order to prevent a collision of the laser with the workpiece. If parts of the STL workpiece are below the machine bed, a warning is emitted and z magnitude is clipped to max z. If parts of the 2D GCode protude the workpiece, a warning is emitted and z magnitude is clipped to max z, again.

![coord](https://github.com/Rob0xFF/GCodeZ/blob/main/coordinates.png?raw=true)
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
```
g++ -std=c++2b gcode_file_translator.cpp distance_calculator.cpp STL_file.cpp main.cpp -o gcodeZ.exe
```
 
## GUI
A basic GUI for Python is accessible at /src/gcodeZ_GUI.py.

![GUI](https://github.com/Rob0xFF/GCodeZ/blob/main/gui.png?raw=true)
