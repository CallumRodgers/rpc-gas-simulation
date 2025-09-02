#ifndef PMPHYSICSLISTS_HH
#define PMPHYSICSLISTS_HH

#include "G4VModularPhysicsList.hh"
#include "G4FastSimulationPhysics.hh"

class PMPhysicsList : public G4VModularPhysicsList
{
public:
    PMPhysicsList();
    ~PMPhysicsList();

    void RegisterParametrization();

private:
    G4FastSimulationPhysics* fastSimPhysics;
};

#endif