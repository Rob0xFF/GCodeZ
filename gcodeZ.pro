 TEMPLATE   = app
 QMAKE_CXXFLAGS += -Wall -Wextra
 CONFIG += c++2b
 SRCDIR = ./src
 HEADERS   += $${SRCDIR}/distance_calculator.h $${SRCDIR}/gcode_file_translator.h $${SRCDIR}/STL_file.h
 SOURCES   += $${SRCDIR}/distance_calculator.cpp $${SRCDIR}/gcode_file_translator.cpp $${SRCDIR}/STL_file.cpp $${SRCDIR}/main.cpp