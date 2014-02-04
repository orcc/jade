
file(GLOB_RECURSE RVCDecoder_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/RVCDecoder/*.h")

set(LIBRARY_OUTPUT_PATH ${JADE_OUTPUT_PATH})

add_library(RVCDecoder SHARED
	RVCDecoder.cpp
	source.c
	display.c
	fpsPrint.c
	compareyuv.c
	writer.c
	orcc_util.c
	orcc_util.h
	orcc_types.h
	${RVCDecoder_HDRS}
 )

# Library required
target_link_libraries(RVCDecoder
	RVCEngine
	ConfigurationEngine
	XDFSerialize
	IRMerger
	Scenario
	IROptimize
	IRSerialize
	RoundRobinScheduler
	IRJit
	IRActor
	IRUtil
	IRCore
	HDAGGraph
	TinyXml
	${LLVM_LIBRARIES}
	${LLVM_LD_FLAGS}
	${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS RVCDecoder
	LIBRARY DESTINATION lib
)
