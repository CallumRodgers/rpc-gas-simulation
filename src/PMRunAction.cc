#include "PMRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"

PMRunAction::PMRunAction()
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    // Histogramas
    analysisManager->CreateH1("Edep", "Total energy deposit per event", 100, 0., 1.1 * MeV);
    analysisManager->CreateH1("HitsPerPad", "Number of hits per pad", 64, 0., 64.);
    analysisManager->CreateH1("EdepPerPad", "Energy deposit per pad", 64, 0., 64.);
}

PMRunAction::~PMRunAction()
{
}

void PMRunAction::BeginOfRunAction(const G4Run* run)
{
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->OpenFile("output.root");
    G4cout << "Starting run " << run->GetRunID() << G4endl;
}

void PMRunAction::EndOfRunAction(const G4Run* run)
{
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
    G4cout << "Finished run " << run->GetRunID() << G4endl;
}
