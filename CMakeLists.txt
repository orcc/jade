# CMake top level project file
cmake_minimum_required (VERSION 2.8)

project (Jade)

include(resources/JadeMacros.cmake)

# Allow users to compile Jade with a non-default LLVM version
# Must be defined before find_package(LLVM) call
set(LLVM_INSTDIR_DEFAULT_VALUE "/path/to/llvm/folder")
set(FORCE_LLVM_INSTALL_DIR ${LLVM_INSTDIR_DEFAULT_VALUE} CACHE PATH "Force LLVM version to compile against by providing directory where it is installed.")

if(NOT "${FORCE_LLVM_INSTALL_DIR}" STREQUAL "${LLVM_INSTDIR_DEFAULT_VALUE}")
	if(IS_DIRECTORY "${FORCE_LLVM_INSTALL_DIR}" AND IS_DIRECTORY "${FORCE_LLVM_INSTALL_DIR}/share/llvm/cmake")
        set(LLVM_DIR "${FORCE_LLVM_INSTALL_DIR}/share/llvm/cmake")
	endif()
endif()

# Search for required third party libraries
find_package(LLVM REQUIRED)
find_package(SDL REQUIRED)
find_package(Threads REQUIRED)

# Generate documentation option
option(GENERATE_DOCS "Generate documentation using doxygen.")
# Generate documentation option
option(BUILD_GPAC_RVC_DECODER "Generate RVCDecodcr library needed to compile Gpac rvc_decoder module")

if(LLVM_FOUND)

	message(STATUS "Found LLVM ${LLVM_VERSION}")

	# Force Cmake to search for a new llvm-config program if LLVM version has been changed since last build
	if(DEFINED LLVM_CONFIG_TOOL)
		execute_process(COMMAND ${LLVM_CONFIG_TOOL} --version OUTPUT_VARIABLE LLVM_CFG_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
		if(NOT "${LLVM_CFG_VERSION}" STREQUAL "${LLVM_VERSION}")
			unset(LLVM_CONFIG_TOOL CACHE)
        endif()
    endif()

	# Try to find llvm-config tool, to configure all LLVM related options for Jade build
	find_file(LLVM_CONFIG_TOOL
		NAMES "llvm-config${CMAKE_EXECUTABLE_SUFFIX}"
		PATHS ${LLVM_INSTALL_PREFIX}/bin ${LLVM_TOOLS_BINARY_DIR}
		NO_DEFAULT_PATH
		DOC "Path to llvm-config executable"
	)

	# List LLVM components used in Jade
	set(COMPONENTS archive jit asmparser instrumentation interpreter nativecodegen bitreader selectiondag linker bitwriter ipo irreader engine)

	message(STATUS "Use ${LLVM_CONFIG_TOOL} to configure linker")

	execute_process(COMMAND ${LLVM_CONFIG_TOOL} --libs ${COMPONENTS}	OUTPUT_VARIABLE LLVM_LIBRARIES		OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${LLVM_CONFIG_TOOL} --includedir 			OUTPUT_VARIABLE LLVM_INCLUDE_DIRS	OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${LLVM_CONFIG_TOOL} --cppflags 				OUTPUT_VARIABLE LLVM_CPP_FLAGS		OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${LLVM_CONFIG_TOOL} --ldflags 				OUTPUT_VARIABLE LLVM_LD_FLAGS		OUTPUT_STRIP_TRAILING_WHITESPACE)

endif(LLVM_FOUND)

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CPP_FLAGS} -fno-rtti -fPIC")
include_directories(${JADE_MAIN_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})

if(GENERATE_DOCS)
	add_subdirectory(doc)
endif(GENERATE_DOCS)
add_subdirectory(runtime)
add_subdirectory(src)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/JadeUninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/JadeUninstall.cmake"
    IMMEDIATE @ONLY
)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/JadeUninstall.cmake)
