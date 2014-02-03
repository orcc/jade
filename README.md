# Jade

**This branch supports build of Jade against LLVM _3.4_ only.**

## About
The Just-In-Time Adaptive Decoder Engine (Jade) project is a new dynamic decoder based on the Low Level Virtual Machine (LLVM), fully compliant with MPEG RVC paradigm. The Jade executable can dynamically instantiate several RVC decoder descriptions produced by the Jade backend of Orcc (http://github.com/orcc/orcc).

Input stimuli of decoders are available at https://sourceforge.net/projects/orcc/files/Sequences/

## Project structure

The Jade project is structured as follow :

- **src**: source files of the project
- **include**: shared headers for Jade libraries
- **doc**: folder where Doxygen doc will be generated
- **runtime**: external libraries used to build Jade
- **resources**: some cmake files used to configure and debug build

## Compile

To compile, Jade depends on:
- CMake (at least 2.6): http://www.cmake.org
- SDL (at least 1.2.14): http://www.libsdl.org
- LLVM 3.4: http://llvm.org

To compile Jade, use cmake to generate build files corresponding to your environment. You must ensure SDL library and LLVM binary dir are reachable from command line (add it to your PATH environment variable) or use cmake-gui to manually set their location.

Here is an example for a build from Unix/Linux command line:
```sh
mkdir build && cd build
cmake ..
make
sudo make install
```

You can uninstall Jade with the following command:
```sh
cd build
sudo make uninstall
```

## Link against LLVM

When running CMake, [find_program()](http://www.cmake.org/cmake/help/v2.8.10/cmake.html#command:find_program "CMake doc on find_program()") command will be used to search for the binary llvm-config. If this binary is reachable from your $PATH or another environment variable, CMake will find it by itself. This executable is used to configure the whole build and the flags used to compile Jade against LLVM.

### CMake don't find llvm-config automatically
You can help CMake to find llvm-config by setting CMAKE_MODULE_PATH with the full path of a LLVM bin directory (ex: ```cmake -DCMAKE_MODULE_PATH=/opt/llvm/3.4/bin ..```), or you can set LLVM_CONFIG_TOOL variable (see below)

### I want to change the LLVM installation to use
You can force an LLVM version tu use by simply set LLVM_CONFIG_TOOL variable with the full path of a llvm-config binary (ex: ```cmake -DLLVM_CONFIG_TOOL=/opt/llvm/3.4/bin/llvm-config ..```)

## CMake options
You can configure some parts of the build by setting variables when calling CMake tool. To do this, use cmake-gui to set variables in a graphic window, or use the command line options: ```cmake <src_folder> -DVAR1=<value> -DVAR2=<value> ...```

Supported variables are:
 - **GENERATE_DOCS**: set to 1 to generate Jade documentation (Doxygen is required)
 - **BUILD_GPAC_RVC_DECODER**: set to 1 to build RVCDecoder dynamic library, to build Gpac module 'rvc_dec'
 - **CMAKE_PREFIX_PATH**: set the path you want to install Jade when using ```make install``` or equivalent target

## Use Jade

Jade provides different interfaces for managing the execution of XDF networks. The console or scenario modes give the ability to test its reconfiguration ability by changing on-the-fly the network execution.
Jade encompasses several options to tunes the performances depending on the executing platform. Use `--help` to get a full lists of options managed by Jade.

Jade can run on three differents modes.

### Command line

This is the default mode, Jade is launched with a specified VTL, an XDF network and an input stimulus. It execute the given design and stop.
Run Jade with the following options:
 - `-xdf <Top_network.xdf>`: Full path of your design's top network
 - `-L <VTL_folder>`: Full path of VTL folder your generated with Orcc
 - `-i <input_stimulus>`: Full path of a video to decode (see https://sourceforge.net/projects/orcc/files/Sequences/)

### Console mode

In this interactive mode, you can control Jade runtime by directly sending commands.
Run Jade with the following options:
 - `-console`: Start the console mode
 - `-L <VTL_folder>`: Full path of VTL folder your generated with Orcc

Once Jade run in console mode, you can type `help` to get a list of available commands. You can also see [below](#console--scenario-modes-commands)

### Scenario mode

Jade runs with a given list of actions to execute, and stop.
Run Jade with the following options:
 - `-scenario <scenario_file.jsc>`: Start the scenario mode. Full path of scenario file must be specified (see below for an example)
 - `-L <VTL_folder>`: Full path of VTL folder your generated with Orcc

An example of a scenario is given below, which start the proprietary MPEG-4 Simple Profile of Ericsson and then reconfigure the decoder with the MPEG-C normalized MPEG-4 Simple Profile.
```xml
<?xml version="1.0" encoding="UTF-8"?>
<JSC>
	<Load id="1" xdf="/path/to/Top_E___.xdf"/>
	<Start input="/path/to/sequences/MPEG4/SIMPLE/P-VOP/hit001.m4v" id="1" mapping="/path/to/Top_E___.xcf"/>
	<Pause/>
	<Stop id="1"/>
	<Set xdf="/path/to/Top_mpeg4_part2_SP_decoder.xdf" id="1"/>
	<Start input="/path/to/sequences/MPEG4/SIMPLE/P-VOP/hit001.m4v" id="1" mapping="/path/to/Top_mpeg4_part2_SP_decoder.xcf"/>
	<Pause/>
	<Stop id="1"/>
</JSC>
```

### Console / scenario modes commands

Here is a list of commands you can use in both console and scenario mode.
 - **Pause**: Pause a given decoder.
 - **Load**: Load and store a given decoder.
 - **Set**: Reconfigure an existing decoder with a new network.
 - **Wait**: Put Jade in wait mode during a given period (in second).
 - **Print**: Print a decoder representation in a given file.
 - **Remove**: Remove a given decoder.
 - **Verify**: Verify the coherence of a given decoder.
