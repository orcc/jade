include_directories(${JADE_MAIN_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})

add_definitions(${LLVM_CPP_FLAGS} ${JADE_CPP_FLAGS})

file(GLOB ConfigurationEngine_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/ConfigurationEngine/*.h")

add_library (ConfigurationEngine
	Connector.cpp
	Connector.h
	Initializer.cpp
	Initializer.h
	Instantiator.cpp
	Instantiator.h
	ConfigurationEngine.cpp
	Configuration.cpp
	Reconfiguration.cpp
	Reconfiguration.h
	${ConfigurationEngine_HDRS}
 )

if(UNIX)
  set_target_properties(ConfigurationEngine PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)
