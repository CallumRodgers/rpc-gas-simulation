cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(RPC)

# Encontra os pacotes necessários
find_package(Garfield REQUIRED)
find_package(Geant4 REQUIRED)

# Inclui as configurações padrão do Geant4
include(${Geant4_USE_FILE})

# Encontra automaticamente todos os arquivos fonte (.cc) na pasta src
file(GLOB sources "src/*.cc")

# Define o executável principal
add_executable(RPC RPC.cc ${sources})

# --- Configuração dos Includes e Bibliotecas ---

# Diz ao compilador onde procurar por arquivos de header (#include "...")
target_include_directories(RPC
    PRIVATE
    # Adicionamos a pasta 'src', caso algum header esteja lá
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    # ADICIONADO: Adicionamos a pasta 'include' que contém os headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Faz o link do executável com as bibliotecas do Geant4 e Garfield++
target_link_libraries(RPC
    PRIVATE
    ${Geant4_LIBRARIES}
    Garfield::Garfield
)

configure_file(vis.mac vis.mac COPYONLY)
configure_file(run.mac run.mac COPYONLY)
configure_file(rpc_gas_5_5_90.gas rpc_gas_5_5_90.gas COPYONLY)