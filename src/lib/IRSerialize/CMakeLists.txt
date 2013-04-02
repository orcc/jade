include_directories(${JADE_MAIN_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})

add_definitions(${LLVM_CPP_FLAGS} ${JADE_CPP_FLAGS})

file(GLOB_RECURSE IRSerialize_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IRSerialize/*.h")

add_library (IRSerialize
  IRConstant.h
  IRLinker.cpp
  IRParser.cpp
  IRWriter.cpp
  IRUnwriter.cpp
  ${IRSerialize_HDRS}
)
 
if(UNIX)
  set_target_properties(IRSerialize PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)
