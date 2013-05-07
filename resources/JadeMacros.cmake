
# Print all cmake variables
macro(print_all_variables)
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endmacro(print_all_variables)

# Print content of _inc_dir variable
macro(print_include_dirs)
    get_directory_property(_incs INCLUDE_DIRECTORIES)
    message(STATUS "Include directories for ${CMAKE_CURRENT_SOURCE_DIR} :")
    foreach (_inc_dir ${_incs})
        message(STATUS " * " ${_inc_dir})
    endforeach()
endmacro(print_include_dirs)