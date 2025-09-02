#include "PMRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"
#include "PMAnalysis.hh"

PMRunAction::PMRunAction()
{
    PMAnalysis::CreateHistogramsMarta();
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
