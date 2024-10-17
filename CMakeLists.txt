cmake_minimum_required(VERSION 3.18.4)

# or use: /opt/devkitpro/portlibs/switch/bin/aarch64-none-elf-cmake
include(/opt/devkitpro/cmake/Switch.cmake)

project(deko3d_examples VERSION 0.0.1 LANGUAGES C CXX)
set(PROJECT_AUTHOR "SamoZ256")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

function(ADD_EXAMPLE EXAMPLE_NAME)
    add_executable(${EXAMPLE_NAME} src/${EXAMPLE_NAME}/main.cpp)

    target_compile_features(${EXAMPLE_NAME} PUBLIC cxx_std_17)

    target_link_libraries(${EXAMPLE_NAME} PUBLIC $<IF:$<CONFIG:Debug>,deko3dd,deko3d>)
    target_include_directories(${EXAMPLE_NAME} PUBLIC
    ${DEVKITPRO}/portlibs/switch/include)

    # TODO: shaders
    #nx_add_shader_program(imgui_vsh src/imgui_vsh.glsl vert)
    #nx_add_shader_program(imgui_fsh src/imgui_fsh.glsl frag)

    dkp_add_asset_target(${EXAMPLE_NAME}_romfs ${CMAKE_CURRENT_BINARY_DIR}/romfs)
    #dkp_install_assets(${EXAMPLE_NAME}_romfs DESTINATION shaders TARGETS imgui_vsh imgui_fsh)

    nx_generate_nacp(${EXAMPLE_NAME}.nacp NAME ${EXAMPLE_NAME} AUTHOR ${PROJECT_AUTHOR})
    nx_create_nro(${EXAMPLE_NAME} NACP ${EXAMPLE_NAME}.nacp ROMFS ${EXAMPLE_NAME}_romfs)
endfunction()

ADD_EXAMPLE(hello_world)
