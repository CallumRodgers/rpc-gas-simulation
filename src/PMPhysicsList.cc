#include "G4FastSimulationPhysics.hh"
#include "PMPhysicsList.hh"

#include "PMHeedModel.hh"

PMPhysicsList::PMPhysicsList()
{
    // EM Physics
    RegisterPhysics(new G4EmStandardPhysics());

    // Radioactive decay physics
    RegisterPhysics(new G4RadioactiveDecayPhysics());

    // Decay physics
    RegisterPhysics(new G4DecayPhysics());

    // Registering Garfield++ physics
    auto* fastSimPhysics = new G4FastSimulationPhysics();
    RegisterPhysics(fastSimPhysics);

    for (const G4ParticleDefinition* particle : GarfieldInterface::INTERFACE_PARTICLES) {
        fastSimPhysics->ActivateFastSimulation(particle->GetParticleName());
    }
}

PMPhysicsList::~PMPhysicsList()
{
}