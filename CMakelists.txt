cmake_minimum_required(VERSION 3.12 FATAL_ERROR) # Updated to 3.12 as per example

project(RPC_Simulation)

# --- Find Garfield++ FIRST (as per their example) ---
# This assumes Garfield++ installed a proper CMake config file (GarfieldConfig.cmake)
# If this fails, we might need to revert to the explicit path finding.
find_package(Garfield REQUIRED) # This is the preferred way if available

# --- Find Geant4 package ---
# Keeping the option for UI/Vis as in the example
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 COMPONENTS ui_all vis_all)
else()
    find_package(Geant4)
endif()

if(NOT Geant4_FOUND)
    message(FATAL_ERROR "Geant4 is not found. Example cannot be built.")
endif()

# ---Setup Geant4 include directories and compile definitions--------------------
include(${Geant4_USE_FILE})

# ---Locate sources and headers for this project---------------------------------
# Adjusting to your existing structure
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh) # Assuming you have header files here

# ---Copy all scripts to the build directory-------------------------------------
# Keeping your existing macro copy, you can add more if needed
file(GLOB MACRO_FILES "macros/*.mac")
file(COPY ${MACRO_FILES} DESTINATION ${CMAKE_BINARY_DIR}/)

# If you have specific Garfield++ scripts or gas files, you can add them here
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ar_70_co2_30_1000mbar.gas ${CMAKE_CURRENT_BINARY_DIR}/ar_70_co2_30_1000mbar.gas COPYONLY)

# ---Add the executable, and link it to the Geant4 and Garfield libraries-----
add_executable(sim sim.cc ${sources} ${headers}) # Using your 'sim.cc' as main source

target_link_libraries(sim
    ${Geant4_LIBRARIES}
    Garfield::Garfield # This is how the example links Garfield++
)

target_include_directories(sim PRIVATE
    ${PROJECT_SOURCE_DIR}/include # Your project's includes
    # Garfield++'s include directories are usually added automatically by find_package(Garfield)
)

# ---Add custom target (your existing one)---------------------------------------
add_custom_target(RPC_Simulation DEPENDS sim)