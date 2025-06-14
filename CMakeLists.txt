cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Nome do projeto para o CMake.
project(RPC_Simulation)

# Encontrando e includindo Geant4.
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# Encontrando o ROOT (necessário para o Garfield).

# Encontrando Garfield++.
set(CMAKE_PREFIX_PATH ~/garfieldpp/install)
if(NOT TARGET Garfield::Garfield)
    find_package(Garfield REQUIRED)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

set(CMAKE_BINARY_DIR bin)

# Agrupando o código fonte e macros.
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB MACRO_FILES "macros/*.mac")
file(COPY ${MACRO_FILES} DESTINATION ${CMAKE_BINARY_DIR}/)

# Gerando executáveis.
add_executable(sim sim.cc ${sources})
add_executable(gtgen GasTableGenerator.cc)
target_link_libraries(sim ${Geant4_LIBRARIES} Garfield::Garfield)
target_link_libraries(gtgen Garfield::Garfield)

# Compilação do projeto inteiro depende da compilação
# do executável "sim" e do executável "gas_table".
add_custom_target(RPC_Simulation DEPENDS sim gtgen)
