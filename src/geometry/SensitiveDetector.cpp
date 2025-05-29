//
// Created by callum on 23/04/25.
//

#include "geometry/SensitiveDetector.h"
#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

SensitiveDetector::SensitiveDetector(G4String name) : G4VSensitiveDetector(name) {
    fTotalEnergyDeposited = 0.;
}

SensitiveDetector::~SensitiveDetector() {}

void SensitiveDetector::Initialize(G4HCofThisEvent *) {
    fTotalEnergyDeposited = 0.;
}

G4bool SensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist) {
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();

    G4double fGlobalTime = preStepPoint->GetGlobalTime();
    G4ThreeVector posPhoton = preStepPoint->GetPosition();
    G4ThreeVector momPhoton = preStepPoint->GetMomentum();

    G4double fMomPhotonMag = momPhoton.mag();

    G4double fWlen = (1.239841939 * eV / fMomPhotonMag) * 1E+03;

    analysisManager->FillNtupleIColumn(0, 0, eventID);
    analysisManager->FillNtupleDColumn(0, 1, posPhoton[0]);
    analysisManager->FillNtupleDColumn(0, 2, posPhoton[1]);
    analysisManager->FillNtupleDColumn(0, 3, posPhoton[2]);
    analysisManager->FillNtupleDColumn(0, 4, fGlobalTime);
    analysisManager->FillNtupleDColumn(0, 5, fWlen);
    analysisManager->AddNtupleRow(0);

    G4double energyDeposited = aStep->GetTotalEnergyDeposit();

    if (energyDeposited > 0) {
        fTotalEnergyDeposited += energyDeposited;
    }

    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent *) {
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    analysisManager->FillH1(0, fTotalEnergyDeposited);

    G4cout << "Deposited energy: " << fTotalEnergyDeposited << G4endl;
}