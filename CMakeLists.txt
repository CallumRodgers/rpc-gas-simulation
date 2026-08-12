cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Nome do projeto para o CMake.
project(RPC_Simulation)

# Encontrando ROOT.
find_package(ROOT)

set(CMAKE_PREFIX_PATH ~/geant4/install:~/garfieldpp/install)

# Encontrando e incluindo o Geant4.
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# Encontrando Garfield++.
find_package(Garfield REQUIRED)

# =========================================
# CÓDIGO DA INTERFACE
# =========================================
add_library(Interface SHARED)
target_sources(Interface
    PUBLIC
        ${PROJECT_SOURCE_DIR}/interface/include/GarfieldInterface.hpp

    PRIVATE
        ${PROJECT_SOURCE_DIR}/interface/src/GarfieldInterface.cpp

        ${PROJECT_SOURCE_DIR}/interface/src/MartaSimulation.hpp
        ${PROJECT_SOURCE_DIR}/interface/src/MartaSimulation.cpp
        ${PROJECT_SOURCE_DIR}/interface/src/iRPCSimulation.hpp
        ${PROJECT_SOURCE_DIR}/interface/src/iRPCSimulation.cpp
)
target_include_directories(Interface PUBLIC ${PROJECT_SOURCE_DIR}/interface/include)
target_link_libraries(Interface PRIVATE Garfield::Garfield)

# =========================================
# CÓDIGO DA SIMULAÇÃO (GEANT4)
# =========================================

# Agrupando o código fonte e macros.
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.hpp)

file(GLOB MACRO_FILES "macros/*.mac")
file(GLOB GAS_FILES "gas-files/*.gas")

file(COPY ${MACRO_FILES} DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${GAS_FILES} DESTINATION ${CMAKE_BINARY_DIR})

# Gerando executáveis.
add_executable(sim sim.cpp ${sources})
add_executable(gtgen GasTableGenerator.cc)
add_executable(analysis analysis.cc)
target_link_libraries(sim Interface ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
target_link_libraries(gtgen Garfield::Garfield)
target_link_libraries(analysis ${ROOT_LIBRARIES})

# Compilação do projeto inteiro depende da compilação
# do executável "sim" e do executável "gtgen".
add_custom_target(RPC_Simulation DEPENDS sim gtgen analysis)
