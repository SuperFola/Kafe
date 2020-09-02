cmake_minimum_required(VERSION 3.8)

project(KafeTests CXX)

include_directories(
    ${PROJECT_SOURCE_DIR}/../kafe/include
    ${PROJECT_SOURCE_DIR}/../kafe/thirdparty
)

set(
    SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/main.hpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE Kafe)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)
