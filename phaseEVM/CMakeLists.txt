#******************************************************************************
# Copyright 2016 Ramsundar K G. All Rights Reserved.
#
# This source code is licensed as defined by the LICENSE file found in the
# root directory of this source tree.
#
# Author: Nikolaos Giannopoulos (giannopoylos.2@gmail.com)
#
# This file is a part of C++ implementation of Phase Method Eulerian Motion Magnification
# with the help of the repository https://github.com/kgram007/Eulerian-Motion-Magnificationfor the development of Phase EVM
#
#******************************************************************************

# CMake version check
cmake_minimum_required(VERSION 2.8)

# Project Name
project(phaseEVM)

# Dependent packages
find_package(OpenCV REQUIRED core imgproc objdetect highgui)
find_package(Boost REQUIRED program_options)
find_package(OpenMP)

# Using CXX Flags: Optimization (-O3) and OpenMP 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fopenmp -std=c++14")

# Includes
include_directories("include")
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

# Executable file directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Set the build type to Debug
set(CMAKE_BUILD_TYPE Debug)

# Set the build type to Release
#set(CMAKE_BUILD_TYPE Release)

# Executable
add_executable(${PROJECT_NAME}
	src/main.cpp
	src/phase_evm.cpp

	include/phase_evm.h
	include/AmplitudeWeightedBlur.hpp
	include/fftshift.hpp     
	include/buildLevel.hpp                
	include/filters.hpp      
	include/reconLevel.hpp
	include/differenceOfButterworths.hpp  
	include/FIRWindowBP.hpp
	include/differenceOfIIR.hpp           
	include/maxSCFpyrHt.hpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
	${OpenCV_LIBS}
	${OpenMP_LIBS}
	${Boost_LIBRARIES}
)
