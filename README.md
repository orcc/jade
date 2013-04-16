# Jade

## About
The Just-In-Time Adaptive Decoder Engine (Jade) project is a new dynamic decoder based on the Low Level Virtual Machine (LLVM), fully compliant with MPEG RVC paradigm. The Jade executable can dynamically instantiate several RVC decoder descriptions produced by the Jade back-end of Orcc.

Input stimuli of decoders are available at: https://sourceforge.net/projects/orcc/files/Sequences/

## Project structure

The Jade project is structured as follow :

- src: source files of the project
- include: shared headers for Jade libraries
- doc: project Doxygen will be generated here
- runtime: external (orcc/orcc) library used to build Jade

## Compile

To compile, Jade depends on:

- CMake (at least 2.6): http://www.cmake.org
- SDL (at least 1.2.14): http://www.libsdl.org
- LLVM (at least 2.9): http://llvm.org

To compile Jade, use cmake to generate build files corresponding to your environment (OS and IDE, if any)

```sh
mkdir build
cd build
cmake ..
make
```

Once Jade is built, you will find the resulting binary under bin/<BUILD_TYPE> directory.

## Use Jade

Jade provides different interfaces for managing the execution of XDF networks. 
The console or scenario modes give the ability to test its reconfiguration ability by changing on-the-fly the network execution. 
Jade encompasses several options to tunes the performances depending on the executing platform.

Add the options --help to Jade to find a full lists of options managed by Jade. The three differents modes of Jade are explain below :
Command line : Simply start Jade with an XDF network (option -xdf), an input stimulus (option -i) and the VTL folder (option -L).
Console mode : Start Jade with the option -console and a VTL folder (-L). Type help to obtain a full list of actions available.
Scenario mode : Start Jade with a scenario (option -scenario) and a VTL folder (-L). The manager of Jade will execute sequentially the given scenario file. 

There is a little explanation of all commands available in Scenario or console mode.
Start : Start a given decoder with a given input stimulus.  
Pause : Pause a given decoder.  
Load : Load and store a given decoder.  
Set : Reconfigure an existing decoder with a new decoder.  
Wait : Put Jade in wait mode during a given period (in second).  
Print : Print a decoder representation in a given file.  
Remove : Remove a given decoder.  
Verify : Verify the coherence of a given decoder.  


An example of a scenario is given below, which start the proprietary MPEG-4 Simple Profile of Ericsson and then reconfigure the decoder with the MPEG-C normalized MPEG-4 Simple Profile.
```xml
<?xml version="1.0" encoding="UTF-8"?>
<JSC>
	<Load id=1 xdf="D:\projet\orcc\trunk\projects\Jade\VTL\Top_E___.xdf"/>
	<Start input="E:\Sequences\Mpeg4\SIMPLE\P-VOP\hit001.m4v" id=1 threaded=1/>
	<Pause/>
	<Stop id=1/>
	<Set xdf="D:\projet\orcc\trunk\projects\Jade\VTL\Top_RVC.xdf" id=1/>
	<Start input="E:\Sequences\Mpeg4\SIMPLE\P-VOP\hit001.m4v" id=1 threaded=1/>
	<Pause/>
	<Stop id=1/>
</JSC>
```