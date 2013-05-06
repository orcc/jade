
file(GLOB_RECURSE XCFSerialize_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/XCFSerialize/*.h")

add_library (XCFSerialize
  XCFParser.cpp
  XCFConstant.cpp
  XCFConstant.h
  ${XCFSerialize_HDRS}
)

