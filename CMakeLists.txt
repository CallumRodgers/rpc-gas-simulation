cmake_minimum_required(VERSION 3.29)
project(rpc_gas_simulation)

set(CMAKE_CXX_STANDARD 20)

set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# Finding Geant4.
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# Grouping all sources and headers we have.
file(GLOB sources ${SRC_DIR}/*.cpp ${SRC_DIR}/geometry/*.cpp ${SRC_DIR}/physics/*.cpp)
file(GLOB headers ${INCLUDE_DIR}/*.h ${INCLUDE_DIR}/geometry/*.h ${SRC_DIR}/physics/*.h)

# Linking sources and headers to our executable, as well as libraries.
add_executable(rpc_gas_simulation ${sources} ${headers})
target_link_libraries(rpc_gas_simulation ${Geant4_LIBRARIES})
