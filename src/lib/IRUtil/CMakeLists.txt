
file(GLOB_RECURSE IRUtil_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IRUtil/*.h")

add_library (IRUtil
	FunctionMng.cpp
	PackageMng.cpp
	OptionMng.cpp
	CompressionMng.cpp
	TraceMng.cpp
	${IRUtil_HDRS}
 )
