
file(GLOB_RECURSE XDFSerialize_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/XDFSerialize/*.h")

add_library (XDFSerialize
  BinOpSeqParser.cpp
  BinOpSeqParser.h
  ExprParser.cpp
  ExprParser.h
  NetworkParser.cpp
  NetworkParser.h
  ParseContinuation.h
  TypeParser.cpp
  TypeParser.h
  XDFConstant.cpp
  XDFConstant.h
  XDFParser.cpp
  XDFWriter.cpp
  ${XDFSerialize_HDRS}
)

if(UNIX)
  set_target_properties(XDFSerialize PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)
