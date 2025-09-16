#include <iostream>
#include "G4RunManagerFactory.hh" 
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "DetectorConstruction.hh" 
#include "PhysicsList.hh"
#include "ActionInitialization.hh" 

int main(int argc, char** argv)
{
    // Detecta o modo de sessão (gráfico ou batch)
    G4UIExecutive* ui = nullptr;
    if (argc == 1) { // Modo interativo
        ui = new G4UIExecutive(argc, argv);
    }

    // -> MUDANÇA: Cria o RunManager apropriado (MT ou sequencial) automaticamente
    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    // 1. Registra o DetectorConstruction (ESSENCIAL, estava faltando!)
    runManager->SetUserInitialization(new DetectorConstruction());

    // 2. Registra a PhysicsList
    runManager->SetUserInitialization(new PhysicsList()); // Use o nome da sua classe de física

    // 3. Registra a ActionInitialization (que cuida das outras ações)
    runManager->SetUserInitialization(new ActionInitialization());
    
    // Inicializa o gerenciador de visualização
    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    // Pega o ponteiro do UI Manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    if (ui) { // Modo Gráfico
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
        delete ui;
    } else { // Modo Batch
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }
    
    // Limpeza da memória
    delete visManager;
    delete runManager;

    return 0;
}