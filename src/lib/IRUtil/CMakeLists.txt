include_directories(${LLVM_INCLUDE_DIRS} ${JADE_MAIN_INCLUDE_DIR})

add_definitions(${LLVM_CPP_FLAGS} ${JADE_CPP_FLAGS})

file(GLOB_RECURSE IRUtil_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IRUtil/*.h")

add_library (IRUtil
	FunctionMng.cpp
	PackageMng.cpp
	OptionMng.cpp
	CompressionMng.cpp
	TraceMng.cpp
	${IRUtil_HDRS}
 )
 
if(UNIX)
  set_target_properties(IRUtil PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)
