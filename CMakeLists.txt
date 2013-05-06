# CMake top level project file
cmake_minimum_required (VERSION 2.6)

project (Jade)

find_program(LLVM_CONFIG llvm-config ${LLVM_INSTDIR}/bin DOC "Path to llvm-config executable")
find_package(SDL REQUIRED)
find_package(Threads REQUIRED)

# PThread dependencies
IF(NOT CMAKE_HAVE_PTHREAD_H)
	set(PTHREAD_DIR CACHE STRING "pthread directory")
	set(PTHREAD_INCLUDE_DIRS ${PTHREAD_DIR}/include)
	set(CMAKE_THREAD_LIBS_INIT ${PTHREAD_DIR}/lib/pthreadVCE2.lib)

	include_directories(${PTHREAD_INCLUDE_DIRS})
ENDIF(NOT CMAKE_HAVE_PTHREAD_H)

# LLVM dependencies
IF(LLVM_CONFIG STREQUAL "LLVM_CONFIG-NOTFOUND")
	set(LLVM_DIR /where/llvm/lives CACHE STRING "llvm source directory")
	set(LLVM_INST_DIR /where/llvm/is/installed CACHE STRING "llvm installation directory")

else(LLVM_CONFIG STREQUAL "LLVM_CONFIG-NOTFOUND")

    string(REPLACE "/bin/llvm-config" "" LLVM_DIR ${LLVM_CONFIG})
    set(LLVM_INSTDIR ${LLVM_DIR} CACHE PATH "LLVM installation directory" FORCE)

	EXECUTE_PROCESS(COMMAND ${LLVM_CONFIG} --libs archive jit asmparser instrumentation interpreter nativecodegen bitreader selectiondag linker bitwriter ipo irreader engine OUTPUT_VARIABLE LLVM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
	EXECUTE_PROCESS(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
	EXECUTE_PROCESS(COMMAND ${LLVM_CONFIG} --cppflags OUTPUT_VARIABLE LLVM_CPP_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
	EXECUTE_PROCESS(COMMAND ${LLVM_CONFIG} --ldflags OUTPUT_VARIABLE LLVM_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

endif(LLVM_CONFIG STREQUAL "LLVM_CONFIG-NOTFOUND")

# Set Jade flags
set(JADE_MAIN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(JADE_DOC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doc)
set(JADE_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Generate documentation option
option(GENERATE_DOCS "Generate documentation using doxygen.")
if(GENERATE_DOCS)
	add_subdirectory(doc)
endif()

# Set output path depending on build type set in environment
if("" STREQUAL "${CMAKE_BUILD_TYPE}")
	set(EXECUTABLE_OUTPUT_PATH "${JADE_BIN_DIR}/Default")
else()
	set(EXECUTABLE_OUTPUT_PATH "${JADE_BIN_DIR}/${CMAKE_BUILD_TYPE}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
add_definitions(${LLVM_CPP_FLAGS})
include_directories(${JADE_MAIN_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})

add_subdirectory(runtime)
add_subdirectory(src)
