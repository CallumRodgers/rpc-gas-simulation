
#include "G4RunManager.hh"

#include "Math/IntegratorOptions.h"

#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "AbortionHandler.hpp"

#include "PhysicsList.hpp"
#include "DetectorConstructionMarta.hpp"
#include "ActionInitialization.hpp"
#include "DetectorConstructionIRPC.hpp"
#include "MainParameters.hpp"

int main(int argc, char **argv) {

    ROOT::Math::IntegratorOneDimOptions::SetDefaultIntegrator("AdaptiveSingular");

    G4UIExecutive* ui = nullptr;

    // Garfield é single-threaded, então não funciona com o G4MTRunManager().
    auto runManager = new G4RunManager();

    auto* params = new RPC::MainParameters();

    // Physics list
    auto physicsList = new RPC::PhysicsList();
    runManager->SetUserInitialization(physicsList);

    // Detector construction
    runManager->SetUserInitialization(new RPC::Geometry::DetectorConstructionIRPC(params));

    // Action initialization
    runManager->SetUserInitialization(new RPC::ActionInitialization());

    // Precisa ser chamado após a PhysicsList ser adicionada ao runManager.
    // Caso contrário um erro será gerado.
    physicsList->RegisterParametrization();

    // Salva os histogramas no caso de uma exception.
    auto abortionHandler = new RPC::AbortionHandler();

    // Cria UI executive apenas se não houver macro (modo interativo)
    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv);
    }

    // Inicializa visualização somente se estiver no modo interativo
    G4VisManager *visManager = nullptr;
    if (ui) {
        visManager = new G4VisExecutive();
        visManager->Initialize();
    }

    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    if (ui) {
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();

        delete ui;
        delete visManager;
    } else {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }
}
