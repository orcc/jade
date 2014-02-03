# CMake top level project file
cmake_minimum_required (VERSION 2.8)

project (Jade)

include(resources/JadeMacros.cmake)

# Search for required third party libraries
find_package(SDL REQUIRED)
find_package(Threads REQUIRED)

# Generate documentation option
option(GENERATE_DOCS "Generate documentation using doxygen.")
# Generate documentation option
option(BUILD_GPAC_RVC_DECODER "Generate RVCDecodcr library needed to compile Gpac rvc_decoder module")

# Try to find llvm-config tool, to configure all LLVM related options for Jade build
# We don't use find_package(LLVM) here because if users compiled LLVM with autotools (configure / make) instead
# of CMake (cmake / make) the share/llvm/cmake path is missing in install directory.
find_program(LLVM_CONFIG_TOOL llvm-config
	DOC "Path to llvm-config executable"
)

# Check if find_program didn't succeed to find llvm-config executable
if("${LLVM_CONFIG_TOOL}" STREQUAL "LLVM_CONFIG_TOOL-NOTFOUND")
	message(FATAL_ERROR "The llvm-config has not been found on your system. Please set manually LLVM_CONFIG_TOOL variable with the full path of llvm-config you want to use.\nYou also can fill CMAKE_MODULE_PATH with the path of an LLVM bin directory.")
endif()

# Check if LLVM_CONFIG_TOOL has a valid value (path of llvm-config)
execute_process(COMMAND ${LLVM_CONFIG_TOOL} --version OUTPUT_VARIABLE LLVM_CFG_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
if("" STREQUAL "${LLVM_CFG_VERSION}")
	message(FATAL_ERROR "The LLVM_CONFIG_TOOL variable have an invalid value. It must contains the full path of llvm-config executable.")
endif()

message(STATUS "Use ${LLVM_CONFIG_TOOL} to configure link information")

# List LLVM components used in Jade
set(COMPONENTS archive jit asmparser instrumentation interpreter nativecodegen bitreader selectiondag linker bitwriter ipo irreader engine)

execute_process(COMMAND ${LLVM_CONFIG_TOOL} --libs ${COMPONENTS}	OUTPUT_VARIABLE LLVM_LIBRARIES		OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_TOOL} --bindir				OUTPUT_VARIABLE LLVM_BIN_DIR		OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_TOOL} --includedir 			OUTPUT_VARIABLE LLVM_INCLUDE_DIRS	OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_TOOL} --cppflags 				OUTPUT_VARIABLE LLVM_CPP_FLAGS		OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_TOOL} --ldflags 				OUTPUT_VARIABLE LLVM_LD_FLAGS		OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "LLVM version ${LLVM_CFG_VERSION} found in folder ${LLVM_BIN_DIR}")

# Set Jade flags
set(JADE_MAIN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(JADE_DOC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doc)
set(JADE_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Set output path depending on build type set in environment
if("" STREQUAL "${CMAKE_BUILD_TYPE}")
	set(JADE_OUTPUT_PATH "${JADE_BIN_DIR}/Default")
else()
	set(JADE_OUTPUT_PATH "${JADE_BIN_DIR}/${CMAKE_BUILD_TYPE}")
endif()

# Configure build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CPP_FLAGS} -fno-rtti -fPIC")

include_directories(${JADE_MAIN_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})
message(STATUS "includes: ${LLVM_INCLUDE_DIRS}")

if(GENERATE_DOCS)
	add_subdirectory(doc)
endif()

add_subdirectory(runtime)
add_subdirectory(src)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/JadeUninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/JadeUninstall.cmake"
    IMMEDIATE @ONLY
)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/JadeUninstall.cmake)
