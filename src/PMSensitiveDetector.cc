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
    fHitsPerPad.resize(64, 0);
    fEdepPerPad.resize(64, 0.);
}

PMSensitiveDetector::~PMSensitiveDetector() {}

void PMSensitiveDetector::Initialize(G4HCofThisEvent *)
{
    fPadsHitThisEvent.clear();
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
        G4TouchableHandle touchable = preStepPoint->GetTouchableHandle();
        G4int padID = touchable->GetCopyNumber();

        if (padID >= 0 && padID < 64)
        {
            auto insertResult = fPadsHitThisEvent.insert(padID);
            if (fPadsHitThisEvent.size() > 1) {
                return false;
            }

            if (insertResult.second) {
                fTotalEnergyDeposited += energyDeposited;
                fHitsPerPad[padID]++;
                fEdepPerPad[padID] += energyDeposited;

                G4cout << "Pad sensibilizado: ID = " << padID
                       << " | Energia depositada = " << energyDeposited / keV << " keV"
                       << G4endl;
            }
        }
        else
        {
            G4cerr << "Warning: padID fora do intervalo esperado: " << padID << G4endl;
        }
    }

    return true;
}

void PMSensitiveDetector::EndOfEvent(G4HCofThisEvent *)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    analysisManager->FillH1(0, fTotalEnergyDeposited);
    for (G4int i = 0; i < 64; ++i)
    {
        for (G4int j = 0; j < fHitsPerPad[i]; ++j)
        {
            analysisManager->FillH1(1, i);
            analysisManager->FillH1(2, i, fEdepPerPad[i]);
        }
    }
    G4cout << "Deposited energy (event): " << fTotalEnergyDeposited / MeV << " MeV" << G4endl;
}
