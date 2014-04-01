
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
