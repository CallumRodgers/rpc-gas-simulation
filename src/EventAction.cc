#include "EventAction.hh"
#include <iomanip>
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "Analysis.hh"
#include "Physics.hh"
#include "RunAction.hh"
#include "Randomize.hh"
#include "G4coutDestination.hh" 
#include "G4VisManager.hh"
#include "G4Polyline.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

EventAction::EventAction() 
: G4UserEventAction(),
  fEnergyAbs(0.),
  fEnergyGas(0.),
  fTrackLAbs(0.),
  fAvalancheSize(0),
  fGain(0.)
{}

EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event* /*event*/) {
  fEnergyAbs = 0.;
  fEnergyGas = 0.;
  fTrackLAbs = 0.;
  fAvalancheSize = 0.;
  fGain = 0.;

  GarfieldPhysics* garfieldPhysics = GarfieldPhysics::GetInstance();
  garfieldPhysics->Clear();
}

void EventAction::EndOfEventAction(const G4Event* event) {
  G4int eventID = event->GetEventID();
  G4cout << "\n[LOG] EventAction::EndOfEventAction -> End of Event " << eventID << G4endl;

  GarfieldPhysics* garfieldPhysics = GarfieldPhysics::GetInstance();
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  
  fEnergyGas = garfieldPhysics->GetEnergyDeposit_MeV();
  fAvalancheSize = garfieldPhysics->GetAvalancheSize();
  fGain = garfieldPhysics->GetGain();
  
  G4cout << "    -> Retrieved Avalanche Size: " << fAvalancheSize << G4endl;
  G4cout << "    -> Retrieved Gain: " << fGain << G4endl;

  analysisManager->FillH1(1, fEnergyAbs);
  analysisManager->FillH1(2, fTrackLAbs);
  analysisManager->FillH1(3, fEnergyGas);
  analysisManager->FillH1(4, fAvalancheSize);
  analysisManager->FillH1(5, fGain);

  analysisManager->FillNtupleDColumn(0, fEnergyAbs);
  analysisManager->FillNtupleDColumn(1, fTrackLAbs);
  analysisManager->FillNtupleDColumn(2, fEnergyGas);
  analysisManager->FillNtupleDColumn(3, fAvalancheSize);
  analysisManager->FillNtupleDColumn(4, fGain);
  analysisManager->AddNtupleRow();

  G4VVisManager* pVisManager = G4VVisManager::GetConcreteInstance();
  if (pVisManager) {
      GarfieldPhysics* garfieldPhysics = GarfieldPhysics::GetInstance();
      const auto& driftLines = garfieldPhysics->GetDriftLines();
      for (const auto& line : driftLines) {
          G4Polyline polyline;
          polyline.push_back(line.first);
          polyline.push_back(line.second);
          
          G4Colour colour(0.0, 1.0, 1.0); 
          G4VisAttributes attribs(colour);
          polyline.SetVisAttributes(attribs);
          
          pVisManager->Draw(polyline);
      }
  }

  G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ((printModulo > 0) && (eventID % printModulo == 0)) {
    G4cout << "---> End of event summary: " << eventID << G4endl;

    G4cout << "   Absorber: total energy: " << std::setw(7)
           << G4BestUnit(fEnergyAbs, "Energy")
           << "       total track length: " << std::setw(7)
           << G4BestUnit(fTrackLAbs, "Length") << G4endl;

    G4cout << "        Gas: total energy: " << std::setw(7)
           << G4BestUnit(fEnergyGas, "Energy")
           << "       avalanche size: " << fAvalancheSize
           << "       gain: " << fGain << G4endl;
  }
}