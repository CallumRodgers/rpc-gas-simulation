//
// Created by callum on 24/04/25.
//

#include "physics/RunAction.h"

RunAction::RunAction()
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    analysisManager->CreateH1("Edep", "Energy deposit", 100, 0., 1.1 * MeV);

    analysisManager->CreateNtuple("Photons", "Photons");
    analysisManager->CreateNtupleIColumn("iEvent");
    analysisManager->CreateNtupleDColumn("fX");
    analysisManager->CreateNtupleDColumn("fY");
    analysisManager->CreateNtupleDColumn("fZ");
    analysisManager->CreateNtupleDColumn("fGlobalTime");
    analysisManager->CreateNtupleDColumn("fWlen");
    analysisManager->FinishNtuple(0);
}

RunAction::~RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run *run)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    G4int runID = run->GetRunID();

    std::stringstream strRunID;
    strRunID << runID;

    analysisManager->OpenFile("output.root");
}

void RunAction::EndOfRunAction(const G4Run *run)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    analysisManager->Write();

    analysisManager->CloseFile();

    G4int runID = run->GetRunID();

    G4cout << "Finishing run " << runID << G4endl;
}