#include "G4RunManager.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "PMAbortionHandler.hh"

#include "PMPhysicsList.hh"
#include "PMDetectorConstructionMarta.hh"
#include "PMActionInitialization.hh"
#include "PMDetectorConstructionIRPC.hh"
#include "PMMainParameters.hh"

int main(int argc, char **argv) {
    G4UIExecutive *ui = nullptr;

    auto runManager = new G4MTRunManager();
    //#ifdef G4MULTITHREADED
    //    G4MTRunManager *runManager = new G4MTRunManager;
    //#else
    //    G4RunManager *runManager = new G4RunManager;
    //#endif

    auto* params = new PMMainParameters();

    // Physics list
    auto physicsList = new PMPhysicsList();
    runManager->SetUserInitialization(physicsList);

    // Detector construction
    runManager->SetUserInitialization(new RPCGeometry::PMDetectorConstructionIRPC(params));

    // Action initialization
    runManager->SetUserInitialization(new PMActionInitialization());

    // Precisa ser chamado após a PhysicsList ser adicionada ao runManager.
    // Caso contrário um erro será gerado.
    physicsList->RegisterParametrization();

    // Salva os histogramas no caso de uma exception.
    auto abortionHandler = new PMAbortionHandler();

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

    delete abortionHandler;
    delete runManager;
}
