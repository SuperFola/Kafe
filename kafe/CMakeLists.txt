cmake_minimum_required(VERSION 3.8)

project(
    Kafe
    LANGUAGES
        CXX
    VERSION
        0.1.0
    DESCRIPTION
        "Kafe lang VM and compiler"
)

# enable -fPIC because we are building a library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(KAFE_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/thirdparty
)

include_directories(${KAFE_INCLUDE_DIR})

file(GLOB_RECURSE SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/source/*.cpp
    ${PROJECT_SOURCE_DIR}/thirdparty/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)
