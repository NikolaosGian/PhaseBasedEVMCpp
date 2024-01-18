
# Phase Based EVM C++
<p align="left">
  <a href="https://github.com/NikolaosGian/PhaseBasedEVMCpp/blob/main/LICENSE"><img src="https://img.shields.io/badge/License-MIT-brightgreen.svg" alt="License"></a>
</p>
 
## About:
This repository containing the Phase Method Eulerian Motion Magnification (EVM) implementation with C++

This implementation is based on the works done at MIT CSAIL.
For futher details visit https://people.csail.mit.edu/nwadhwa/phase-video/

Ιmplementation works only with:


| Fillters  | pyrType | attenuateOtherFreq |
| ------------- | ------------- | ---------|
| differenceOfIIR  | octave / halfOctave  |	:heavy_check_mark:|
| differenceOfButterworths | octave / halfOctave 	 |	:heavy_check_mark: |

## Compiling and Running the code:
Required packages: C++, CMake, FFMPEG, FFTW3
### Compiling
	$ cd <PROJ_DIR>
	$ cmake .
	$ make
### Running the program
	$ cd <PROJ_DIR>
	$ ./bin/phase_Eulerian
 ### Outputs
In ./data/ having the start video (before magnification) and the final output video extended with _phase.avi.
**Do not use large size video.**


 ## Adaptations:
This project has been adapted as reference to build Phase EVM by:

1. [https://github.com/kgram007/Eulerian-Motion-Magnification](https://github.com/kgram007/Eulerian-Motion-Magnification)
