 TEMPLATE   = app
 QMAKE_CXXFLAGS += -Wall
 CONFIG += c++2b static console
 CONFIG -= app_bundle
 SRCDIR = ./src
 HEADERS   += $${SRCDIR}/distance_calculator.h $${SRCDIR}/gcode_file_translator.h $${SRCDIR}/STL_file.h
 SOURCES   += $${SRCDIR}/distance_calculator.cpp $${SRCDIR}/gcode_file_translator.cpp $${SRCDIR}/STL_file.cpp $${SRCDIR}/main.cpp