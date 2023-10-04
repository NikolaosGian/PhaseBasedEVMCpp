
# Phase Based EVM C++
<p align="left">
  <a href="https://github.com/NikolaosGian/PhaseBasedEVMCpp/blob/main/LICENSE"><img src="https://img.shields.io/badge/License-MIT-brightgreen.svg" alt="License"></a>
</p>

## NOT WORKING 
## About:
This repository containing the Phase Method Eulerian Motion Magnification (EVM) implementation with C++

This implementation is based on the works done at MIT CSAIL.
For futher details visit https://people.csail.mit.edu/nwadhwa/phase-video/


Library Used: OpenCV, Boost
## Compiling and Running the code:
Required packages: g++, CMake, OpenCV, Boost
### Compiling
	$ cd <PROJ_DIR>
	$ cmake .
	$ make
### Running the program with test params
	$ cd <PROJ_DIR>
	$ ./bin/phaseEVM test/test_baby.param
 

 ## Adaptations:
This project has been adapted as reference to build Phase EVM by:

1. [https://github.com/kgram007/Eulerian-Motion-Magnification](https://github.com/kgram007/Eulerian-Motion-Magnification)
