cmake_minimum_required(VERSION 3.18.4)

# or use: /opt/devkitpro/portlibs/switch/bin/aarch64-none-elf-cmake
include(/opt/devkitpro/cmake/Switch.cmake)

project(deko3d_examples VERSION 0.0.1 LANGUAGES C CXX)
set(PROJECT_AUTHOR "SamoZ256")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(SWITCH_INCLUDE_DIRS ${DEVKITPRO}/portlibs/switch/include)
set(SWITCH_LIBRARIES $<IF:$<CONFIG:Debug>,deko3dd,deko3d>)

add_library(example_base STATIC
    src/base/application.cpp
    src/base/deko3d_application.cpp
)

target_include_directories(example_base PUBLIC ${SWITCH_INCLUDE_DIRS})
target_link_libraries(example_base PUBLIC ${SWITCH_LIBRARIES})

function(ADD_EXAMPLE)
    set(options)
    set(oneValueArgs EXAMPLE_NAME)
    set(multiValueArgs SHADER_NAMES)
    cmake_parse_arguments(ADD_EXAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${ADD_EXAMPLE_EXAMPLE_NAME} src/examples/${ADD_EXAMPLE_EXAMPLE_NAME}/main.cpp)

    target_compile_features(${ADD_EXAMPLE_EXAMPLE_NAME} PUBLIC cxx_std_17)

    target_include_directories(${ADD_EXAMPLE_EXAMPLE_NAME} PUBLIC ${SWITCH_INCLUDE_DIRS} src)
    target_link_libraries(${ADD_EXAMPLE_EXAMPLE_NAME} PUBLIC ${SWITCH_LIBRARIES} example_base)

    set(ASSETS "")

    foreach(SHADER_NAME IN LISTS ADD_EXAMPLE_SHADER_NAMES)
        nx_add_shader_program(${ADD_EXAMPLE_EXAMPLE_NAME}_${SHADER_NAME}_vsh src/examples/${ADD_EXAMPLE_EXAMPLE_NAME}/shaders/${SHADER_NAME}_vsh.glsl vert)
        nx_add_shader_program(${ADD_EXAMPLE_EXAMPLE_NAME}_${SHADER_NAME}_fsh src/examples/${ADD_EXAMPLE_EXAMPLE_NAME}/shaders/${SHADER_NAME}_fsh.glsl frag)

        set(ASSETS ${ASSETS} ${ADD_EXAMPLE_EXAMPLE_NAME}_${SHADER_NAME}_vsh ${ADD_EXAMPLE_EXAMPLE_NAME}_${SHADER_NAME}_fsh)
    endforeach()

    dkp_add_asset_target(${ADD_EXAMPLE_EXAMPLE_NAME}_romfs ${CMAKE_CURRENT_BINARY_DIR}/romfs)
    if(NOT ASSETS STREQUAL "")
        dkp_install_assets(${ADD_EXAMPLE_EXAMPLE_NAME}_romfs DESTINATION shaders TARGETS ${ASSETS})
    endif()

    nx_generate_nacp(${ADD_EXAMPLE_EXAMPLE_NAME}.nacp NAME ${ADD_EXAMPLE_EXAMPLE_NAME} AUTHOR ${PROJECT_AUTHOR})
    nx_create_nro(${ADD_EXAMPLE_EXAMPLE_NAME} NACP ${ADD_EXAMPLE_EXAMPLE_NAME}.nacp ROMFS ${ADD_EXAMPLE_EXAMPLE_NAME}_romfs)
endfunction()

ADD_EXAMPLE(EXAMPLE_NAME hello_world)
ADD_EXAMPLE(EXAMPLE_NAME clear_color)
