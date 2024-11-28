#include <iostream>

// Geant4 includes.
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"

// User includes.
#include "geometry/DetectorConstruction.h"
#include "physics/PhysicsList.h"
#include "physics/ActionInitialization.h"

int main(int argc, char** argv) {

    // ###############################
    // Parsing command line arguments.
    // ###############################

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <RPC_TYPE>" << std::endl;
        std::cout << "RPC_TYPE:\n- 0 for glass RPC\n- 1 for CERN iRPC" << std::endl;
        return 1;
    }
    int rpcType = atoi(argv[1]); // Converting char array to int.

    // #####################
    // Starting application.
    // #####################

    // Creating our run manager.
    auto runManager = G4RunManagerFactory::CreateRunManager();

    std::cout << "RunManager initialized." << std::endl;

    // Setting up required information for kernel initialization.
    runManager->SetUserInitialization(new DetectorConstruction(rpcType));
    runManager->SetUserInitialization(new PhysicsList());
    runManager->SetUserInitialization(new ActionInitialization());

    // Initialize G4 kernel.
    runManager->Initialize();

    // Get pointer to the UI manager (created during RunManager initialization) and set verbosities.
    G4UImanager* UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/run/verbose 1");
    UI->ApplyCommand("/event/verbose 1");
    UI->ApplyCommand("/tracking/verbose 1");

    // Start run.
    G4int numberOfEvents = 3;
    runManager->BeamOn(numberOfEvents);

    // Terminating application.
    delete runManager;
    return 0;
}