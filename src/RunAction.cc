#include "RunAction.hh"
#include "Physics.hh"
#include "G4Run.hh"
#include "G4UserRunAction.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "Analysis.hh"

RunAction::RunAction() : G4UserRunAction() {

    G4RunManager::GetRunManager()->SetPrintProgress(1);
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    G4cout << "Using" << analysisManager->GetType() << G4endl;

    analysisManager->SetVerboseLevel(1);
    analysisManager->SetFirstHistoId(1);

    analysisManager->CreateH1("1", "Edep in absorber", 100, 0., 800 * MeV);
    analysisManager->CreateH1("2", "Track length in absorber", 100, 0., 1 * m);
    analysisManager->CreateH1("3", "Edep in gas", 1000, 0., 100 * keV);

    analysisManager->CreateH1("4", "Avalanche size in gas", 10000, 0, 10000);
    analysisManager->CreateH1("5", "Gain", 1000, 0., 100);
    analysisManager->CreateH3("1", "Track position", 200, -10 * cm, 10 * cm, 29,
                                -1.45 * cm, 1.45 * cm, 29, -1.45 * cm, 1.45 * cm);

    analysisManager->CreateNtuple("Garfield", "Edep and TrackL");
    analysisManager->CreateNtupleDColumn("Eabs");
    analysisManager->CreateNtupleDColumn("Labs");
    analysisManager->CreateNtupleDColumn("Egas");
    analysisManager->CreateNtupleDColumn("AvalancheSize");
    analysisManager->CreateNtupleDColumn("Gain");
    analysisManager->FinishNtuple();
}

RunAction::~RunAction(){
#if (G4VERSION_NUMBER < 1100)
  auto analysisManager = G4AnalysisManager::Instance();
  if (analysisManager) delete analysisManager;
#endif
}

void RunAction::BeginOfRunAction(const G4Run* run) {
    if (isMaster) {
        G4cout << "### RunAction::BeginOfRunAction (Master Thread) -> Inicializando GarfieldPhysics..." << G4endl;
        GarfieldPhysics::GetInstance()->InitializePhysics();
    }

    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    G4String fileName = "Garfield.root";
    analysisManager->OpenFile(fileName);
}

void RunAction::EndOfRunAction(const G4Run*){
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    if (analysisManager->GetH1(1)) {
        G4cout << G4endl << " ----> print histograms statistic ";
        if (isMaster) {
        G4cout << "for the entire run " << G4endl << G4endl;
        } else {
        G4cout << "for the local thread " << G4endl << G4endl;
        }

        G4cout << " EAbs : mean = "
            << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy")
            << " rms = "
            << G4BestUnit(analysisManager->GetH1(1)->rms(), "Energy") << G4endl;

        G4cout << " LAbs : mean = "
            << G4BestUnit(analysisManager->GetH1(2)->mean(), "Length")
            << " rms = "
            << G4BestUnit(analysisManager->GetH1(2)->rms(), "Length") << G4endl;

        G4cout << " EGas : mean = "
            << G4BestUnit(analysisManager->GetH1(3)->mean(), "Energy")
            << " rms = "
            << G4BestUnit(analysisManager->GetH1(3)->rms(), "Energy") << G4endl;

        G4cout << " Avalanche size : mean = " << analysisManager->GetH1(4)->mean()
            << " rms = " << analysisManager->GetH1(4)->rms() << G4endl;

        G4cout << " Gain : mean = " << analysisManager->GetH1(5)->mean()
            << " rms = " << analysisManager->GetH1(5)->rms() << G4endl;
    }

    analysisManager->Write();
    analysisManager->CloseFile();
    
}