#ifndef STEPPINGACTION_HPP
#define STEPPINGACTION_HPP

#include "G4UserSteppingAction.hh"
#include "G4ThreeVector.hh"

namespace RPC {
    class SteppingAction : public G4UserSteppingAction {
    public:
        SteppingAction() = default;
        void UserSteppingAction(const G4Step *) override;
    };
}

#endif // STEPPINGACTION_HPP
