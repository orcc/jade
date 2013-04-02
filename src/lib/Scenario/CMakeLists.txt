include_directories(${JADE_MAIN_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS})

add_definitions(${LLVM_CPP_FLAGS} ${JADE_CPP_FLAGS})

file(GLOB_RECURSE Scenario_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/Scenario/*.h")

add_library (Scenario
	Manager.cpp
	Scenario.cpp
	Scenario.h
	ScenarioParser.cpp
	ScenarioParser.h
	${Scenario_HDRS}
)
 
if(UNIX)
  set_target_properties(Scenario PROPERTIES COMPILE_FLAGS "-fPIC")
endif(UNIX)
