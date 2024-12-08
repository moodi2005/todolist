cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)

include_directories(/Users/m/Desktop/Project/cplus/libs/Crow/include)
include_directories(/Users/m/Desktop/Project/cplus/libs/json-develop/include)
include_directories(/opt/homebrew/opt/asio/include)

set(BOOST_ROOT "/opt/homebrew/opt/boost")
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})


add_executable(tasks ./src/main.cpp)
