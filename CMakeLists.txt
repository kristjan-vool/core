cmake_minimum_required(VERSION 3.20)
project(core
    VERSION 1.0
    DESCRIPTION "core"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(FetchContent)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.10.4
)

FetchContent_GetProperties(json)
if (NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(src/server)
add_library(core SHARED
    src/server/core/headers/request.hpp
    src/server/core/headers/request.cpp
    src/server/core/headers/response.hpp
    src/server/core/headers/response.cpp
    src/server/core/router/router.hpp
    src/server/core/router/router.cpp
    src/server/core/server/server.hpp
    src/server/core/server/server.cpp
    src/server/core/status/status.hpp
)

target_link_libraries(core PRIVATE nlohmann_json::nlohmann_json)