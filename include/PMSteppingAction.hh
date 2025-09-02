#ifndef PMEVENTACTION_HH
#define PMEVENTACTION_HH

#include "G4UserSteppingAction.hh"
#include "G4ThreeVector.hh"

class PMSteppingAction : public G4UserSteppingAction {
public:
    PMSteppingAction() = default;
    void UserSteppingAction(const G4Step *) override;
};

#endif //PMEVENTACTION_HH
