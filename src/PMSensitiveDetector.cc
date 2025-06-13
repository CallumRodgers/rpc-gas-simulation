#include "PMSensitiveDetector.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"


PMSensitiveDetector::PMSensitiveDetector(G4String name)
    : G4VSensitiveDetector(name)
{
    fTotalEnergyDeposited = 0.;
    fHitsPerPad.resize(65, 0);
    fEdepPerPad.resize(65, 0.);
}


PMSensitiveDetector::~PMSensitiveDetector() {}


void PMSensitiveDetector::Initialize(G4HCofThisEvent *)
{
    fTotalEnergyDeposited = 0.;
    std::fill(fHitsPerPad.begin(), fHitsPerPad.end(), 0);
    std::fill(fEdepPerPad.begin(), fEdepPerPad.end(), 0.);
}


G4bool PMSensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{
    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    G4double energyDeposited = aStep->GetTotalEnergyDeposit();

    if (energyDeposited > 0)
    {
        fTotalEnergyDeposited += energyDeposited;

        G4TouchableHandle touchable = preStepPoint->GetTouchableHandle();
        G4int padID = touchable->GetCopyNumber(); 

        if (padID > 0 && padID < 65)
        {
            fHitsPerPad[padID]++;
            fEdepPerPad[padID] += energyDeposited;
        }
        else
        {
            G4cerr << "Warning: padID fora do intervalo esperado (1 a 64): " << padID << G4endl;
        }
    }

    return true;
}

void PMSensitiveDetector::EndOfEvent(G4HCofThisEvent *)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    analysisManager->FillH1(0, fTotalEnergyDeposited);
    for (G4int padID = 1; padID <= 64; ++padID)
    {
        for (G4int hitCount = 0; hitCount < fHitsPerPad[padID]; ++hitCount)
        {
            analysisManager->FillH1(1, padID); 
        }
    }
        
    for (G4int i = 1; i <= 64; ++i)
    {
        if (fEdepPerPad[i] > 0) {
            analysisManager->FillH1(2, i, fEdepPerPad[i]); 
        }
    }
    G4cout << "Deposited energy (event): " << fTotalEnergyDeposited / MeV << " MeV" << G4endl;
}