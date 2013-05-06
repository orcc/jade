
file(GLOB_RECURSE Scenario_HDRS "${JADE_MAIN_INCLUDE_DIR}/lib/Scenario/*.h")

add_library (Scenario
	Manager.cpp
	Scenario.cpp
	Scenario.h
	ScenarioParser.cpp
	ScenarioParser.h
	${Scenario_HDRS}
)
