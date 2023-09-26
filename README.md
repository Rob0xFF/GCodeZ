# GCodeZ
 3D projection of 2D laser-gcode onto a surface derived from *.stl
# Required Command Line Options: 
 **--stl=stl_file** STL File, could be binary or ASCII
 
 **--gcode=gcode_file** 2D GCode file

 **--output=output_file** Output file, which will contain the pseudo-3D Code

 **--laser=laser_diameter** Diameter of the laser spot im mm, must be the same which was used for 2D GCode generation

# Optional Command Line Options:
 **--stepwith=stepwith** Minimal distance between two spatial points in mm, defaults to 0.5mm

# Build
 uses std::string.contains(), therefore -std=c++2b compiler flag must be set to g++