#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  G4LogicalVolume* volume 
    = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  
  if (volume->GetName() != "GasVolumeLV") {
    G4double edep = step->GetTotalEnergyDeposit();
    G4double stepl = 0.;
    if (step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) {
      stepl = step->GetStepLength();
    }
    if (edep > 0. || stepl > 0.) {
        fEventAction->AddAbs(edep, stepl);
    }
  }
}