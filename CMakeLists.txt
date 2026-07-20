cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Nome do projeto para o CMake.
project(RPC_Simulation)

# Encontrando ROOT.
find_package(ROOT)

set(CMAKE_PREFIX_PATH ~/geant4/install:~/garfieldpp/install)

# Encontrando e includindo Geant4.
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

add_subdirectory(garfield/rpc_simulation-Callum)

# Encontrando Garfield++.
if(NOT TARGET Garfield::Garfield)
    find_package(Garfield REQUIRED)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

# Agrupando o código fonte e macros.
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

file(GLOB MACRO_FILES "macros/*.mac")
file(GLOB GAS_FILES "gas-files/*.gas")

file(COPY ${MACRO_FILES} DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${GAS_FILES} DESTINATION ${CMAKE_BINARY_DIR})

# Gerando executáveis.
add_executable(sim sim.cpp ${sources})
add_executable(gtgen GasTableGenerator.cc)
add_executable(test test.cc)
add_executable(analysis analysis.cc)
target_link_libraries(sim ${Geant4_LIBRARIES} Garfield::Garfield ${ROOT_LIBRARIES})
target_link_libraries(gtgen Garfield::Garfield)
target_link_libraries(test Garfield::Garfield)
target_link_libraries(analysis ${ROOT_LIBRARIES})

# Compilação do projeto inteiro depende da compilação
# do executável "sim" e do executável "gtgen".
add_custom_target(RPC_Simulation DEPENDS sim gtgen analysis)
