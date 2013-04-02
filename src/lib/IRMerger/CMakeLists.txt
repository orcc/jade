include_directories(${JADE_MAIN_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})

add_definitions(${LLVM_CPP_FLAGS} ${JADE_CPP_FLAGS})

file(GLOB_RECURSE IRMerger_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IRMerger/*.h")

add_library (IRMerger
	CheckPinoRules.cpp
	CheckPinoRules.h
	Merger.cpp
	Rational.cpp
	Rational.h
	SuperInstance.cpp
	${IRMerger_HDRS}
)
 
if(UNIX)
  set_target_properties(IRMerger PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)
