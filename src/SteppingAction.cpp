#include "SteppingAction.hpp"

#include <G4Step.hh>
#include <G4VSolid.hh>

#include "G4ios.hh"

using namespace RPC;

void SteppingAction::UserSteppingAction(const G4Step *step) {
    //if (step->GetTrack()->GetVolume()->GetName() == "GarfieldBoxPV") {
    //    G4cout << "INSIDE" << G4endl;
    //    G4cout << "First step? " << step->IsFirstStepInVolume() << G4endl;
    //    G4cout << "Last step in volume? " << step->IsLastStepInVolume() << G4endl;
    //}
}


