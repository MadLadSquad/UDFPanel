cmake_minimum_required(VERSION 3.21)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Framework/cmake/")
set(APP_LIB_TARGET "udf-panelLib")
set(APP_TARGET "udf-panel")

if (NOT EMSCRIPTEN)
    project(UntitledImGuiFramework)
    if (NOT WIN32)
        project(${APP_LIB_TARGET})
    endif ()
endif ()
project(${APP_TARGET})

include(UImGuiSetupCMake)
