#ifndef PHYSICSLISTS_HH
#define PHYSICSLISTS_HH

#include "G4VModularPhysicsList.hh"
#include "G4FastSimulationPhysics.hh"

namespace RPC {
    class PhysicsList : public G4VModularPhysicsList {
    public:
        PhysicsList();
        ~PhysicsList();

        void RegisterParametrization();

    private:
        G4FastSimulationPhysics* fastSimPhysics;
    };
}

#endif