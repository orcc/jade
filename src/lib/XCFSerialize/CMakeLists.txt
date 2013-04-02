add_definitions(${LLVM_CPP_FLAGS} ${JADE_CPP_FLAGS})

include_directories(${LLVM_INCLUDE_DIRS} ${JADE_MAIN_INCLUDE_DIR})

file(GLOB_RECURSE XCFSerialize_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/XCFSerialize/*.h")

add_library (XCFSerialize
  XCFParser.cpp
  XCFConstant.cpp
  XCFConstant.h
  ${XCFSerialize_HDRS}
)

