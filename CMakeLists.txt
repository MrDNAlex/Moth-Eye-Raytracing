cmake_minimum_required(VERSION 3.14)
project(Moth-Eye-Raytracing)

# 1. Set C++ standard (nlohmann/json needs at least C++11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Fetch nlohmann/json automatically
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2  # You can specify a version here
)
FetchContent_MakeAvailable(json)

# 3. Identify your source files
# Replace main.cpp with your actual file names
add_executable(${PROJECT_NAME} main.cpp)

# 4. Link the library to your executable
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)