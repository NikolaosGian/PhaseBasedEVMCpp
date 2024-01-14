
# Phase Based EVM C++
<p align="left">
  <a href="https://github.com/NikolaosGian/PhaseBasedEVMCpp/blob/main/LICENSE"><img src="https://img.shields.io/badge/License-MIT-brightgreen.svg" alt="License"></a>
</p>
 
## About:
This repository containing the Phase Method Eulerian Motion Magnification (EVM) implementation with C++

This implementation is based on the works done at MIT CSAIL.
For futher details visit https://people.csail.mit.edu/nwadhwa/phase-video/

Ιmplementation works only with:
Attempt | #1 | #2 | #3 | #4 | #5 | #6 | #7 | #8 | #9 | #10 | #11
--- | --- | --- | --- |--- |--- |--- |--- |--- |--- |--- |---
Seconds | 301 | 283 | 290 | 286 | 289 | 285 | 287 | 287 | 272 | 276 | 269

## Compiling and Running the code:
Required packages: C++, CMake, FFMPEG
### Compiling
	$ cd <PROJ_DIR>
	$ cmake .
	$ make
### Running the program with test params
	$ cd <PROJ_DIR>
	$ ./bin/phase_Eulerian
 

 ## Adaptations:
This project has been adapted as reference to build Phase EVM by:

1. [https://github.com/kgram007/Eulerian-Motion-Magnification](https://github.com/kgram007/Eulerian-Motion-Magnification)
