#include "G4RunManager.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"

#include "PMPhysicsList.hh"
#include "PMDetectorConstruction.hh"
#include "PMActionInitialization.hh"

int main(int argc, char** argv)
{
    G4UIExecutive *ui = nullptr;

    #ifdef G4MULTITHREADED
        G4MTRunManager *runManager = new G4MTRunManager;
    #else
        G4RunManager *runManager = new G4RunManager;
    #endif

    // Physics list
    runManager->SetUserInitialization(new PMPhysicsList());

    // Detector construction
    runManager->SetUserInitialization(new PMDetectorConstruction());

    // Action initialization
    runManager->SetUserInitialization(new PMActionInitialization());

    // Cria UI executive apenas se não houver macro (modo interativo)
    if (argc == 1)
    {
        ui = new G4UIExecutive(argc, argv);
    }

    // Inicializa visualização somente se estiver no modo interativo
    G4VisManager *visManager = nullptr;
    if (ui)
    {
        visManager = new G4VisExecutive();
        visManager->Initialize();
    }

    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    if (ui)
    {
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();

        delete visManager;
        delete ui;
    }
    else
    {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }

    delete runManager;

    return 0;
}
