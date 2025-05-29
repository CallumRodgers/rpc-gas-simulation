#include <iostream>

// Geant4 includes.
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "QBBC.hh"

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
    auto ui = new G4UIExecutive(argc, argv);
    auto runManager = G4RunManagerFactory::CreateRunManager();

    std::cout << "RunManager initialized." << std::endl;

    // Setting up required information for kernel initialization.
    runManager->SetUserInitialization(new DetectorConstruction(rpcType));
    runManager->SetUserInitialization(new QBBC);
    runManager->SetUserInitialization(new ActionInitialization());

    // Initialize G4 kernel.
    runManager->Initialize();

    // Visualization
    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    // Get pointer to the UI manager (created during RunManager initialization) and set verbosities.
    G4UImanager* UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/control/execute ../macros/vis.mac");
    UI->ApplyCommand("/control/execute ../macros/run.mac");

    ui->SessionStart();

    // Terminating application.
    delete visManager;
    delete runManager;
    delete ui;
    return 0;
}