
file(GLOB_RECURSE IROptimize_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/IROptimize/*.h")

add_library (IROptimize
	InstanceInternalize.cpp
	FifoFnRemoval.cpp
	${IROptimize_HDRS}
)
