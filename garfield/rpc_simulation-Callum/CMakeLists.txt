# ########################################################################################
# Use the code in this section for standalone projects
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(RPC)
if(NOT TARGET Garfield::Garfield)
  find_package(Garfield REQUIRED)
endif()
# ########################################################################################

# ---Build executable------------------------------------------------------------
add_executable(teste teste.C)
target_link_libraries(teste Garfield::Garfield)

add_executable(teste_doublegap teste_doublegap.C)
target_link_libraries(teste_doublegap Garfield::Garfield)

# ---Copy all files locally to the build directory-------------------------------
foreach(_file cms_rpc_95.2_4.5_0.3_25-40kV.gas)
  configure_file(${_file} ${_file} COPYONLY)
endforeach()
