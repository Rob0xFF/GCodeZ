# GCodeZ
A pseudo-3D projection of 2D laser G-code onto a surface derived from an *.stl file. It will add z-coordinates to each linear movement command (G00 and G01). If the distance between two points within the xy-plane exceeds the value given by stepwith, the linear path will be split up to allow precise laser path tracking along the surface. 

The 2D-GCode might come from [Flatcam](https://bitbucket.org/jpcgt/flatcam/downloads/) in GRBL style.
## Coordinates
The **machine (laser) coodinates** are right-handed, where allowed values of x and y are positive and z is negative. When z = 0, the laser is at its highest possible point. The maximum z value is calculated using the laser diameter and the calibration line, representing an exposure of the machine bed with the specified diameter. 

The **workpiece coordinates** (from STL) are right handed, where x, y and z are positive. If the workpiece at any point given in the 2D GCode is higher than the maximum z value, the conversion stops in order to prevent a collision of the laser with the workpiece. If parts of the STL workpiece model are below the machine bed, a warning is emitted and z magnitude is clipped to max z. If parts of the 2D GCode protrude the workpiece, a warning is emitted and z magnitude is clipped to max z, again.

![coord](https://github.com/Rob0xFF/GCodeZ/blob/main/coordinates.png?raw=true)
## Laser Calibration
The laser must be calibrated with respect to the spot size on the machine bed, depending on the z-height. The magnitude of the z-height is expressed by the following linear equation **|z| = cal1 * diameter + cal2**, where **z** and **diameter** are given in mm. Please specify the two parameters in the "laserCalibration.cfg" configuration file in the format 
```
# This line will be ignored
cal1=float
cal2=float
```
where **float** must be replaced by valid floating-point numbers. Lines beginning with # at the start will be ignored. If no parameters can be read, a warning will be issued, and the default values will be used for calculations. 

## Required Command Line Options: 
 **--stl=stl_file** The STL file can be either binary or ASCII format.
 
 **--gcode=gcode_file** The 2D GCode file.

 **--output=output_file** The output file will contain the pseudo-3D code.

 **--laser=laser_diameter** The laser spot diameter is specified in millimeters and must match the diameter used for 2D GCode generation. Parameter must be be valid floating point number.

## Optional Command Line Options:
 **--stepwidth=stepwidth** The minimum spatial point-to-point distance in the output is expressed in millimeters, with a default value of 0.5 millimeters. Reducing this value will result in greater precision in laser path tracking along the STL surface but will necessitate a longer computation time. Parameter must be be valid floating point number.
 
 **--simplify** Consider only facets whose normal vector contains a component in the positive z-direction. Additionally, if a proper range is given in the header of the GCode file, STL facet search is restricted to the area (0, 0) .. (xMax + 0.5, yMax + 0.5). This can lead to increased execution speed in certain models. Ensure that the STL file includes valid normals for each facet. The option does not require any parameters.

## Build
It utilizes std::string.contains(), hence the -std=c++2b compiler flag needs to be provided to g++.
```
g++ -std=c++2b gcode_file_translator.cpp distance_calculator.cpp STL_file.cpp main.cpp -o gcodeZ.exe
```
You can also use **qmake**:
```
qmake && make && make clean
```
 
## GUI
A basic GUI for Python is accessible at /src/gcodeZ_GUI.py.

![GUI](https://github.com/Rob0xFF/GCodeZ/blob/main/gui.png?raw=true)

## Credits
Parts of the code in STL_file.cpp (C) Jerry Greenough, used and published with friendly permission. [Source](https://jgxsoft.com/examples/STL%20Reader/STL%20Reader.html)
 
The intersection algorithm was adapted from *Tomas Möller & Ben Trumbore (1997) Fast, Minimum Storage Ray-Triangle Intersection, Journal of Graphics Tools, 2:1, 21-28* [DOI: 10.1080/10867651.1997.10487468](https://doi.org/10.1080/10867651.1997.10487468)
