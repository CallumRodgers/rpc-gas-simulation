#include "PhysicsList.hpp"

#include "HeedModel.hpp"
#include "G4EmStandardPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4DecayPhysics.hh"

using namespace RPC;

PhysicsList::PhysicsList()
{
    // EM Physics
    RegisterPhysics(new G4EmStandardPhysics());

    // Radioactive decay physics
    RegisterPhysics(new G4RadioactiveDecayPhysics());

    // Decay physics
    RegisterPhysics(new G4DecayPhysics());
}

PhysicsList::~PhysicsList()
{
    delete fastSimPhysics;
}

void PhysicsList::RegisterParametrization()
{
    using namespace GarfieldInterface;

    // Registering Garfield++ physics
    fastSimPhysics = new G4FastSimulationPhysics();
    auto* modelParticles = USING_NEW_TRACK ? NEW_TRACK_PARTICLES : DELTA_ELECTRON_PARTICLES;
    for (const G4String& particleName : NEW_TRACK_PARTICLES) {
        G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
        fastSimPhysics->ActivateFastSimulation(particle->GetParticleName());
    }
    RegisterPhysics(fastSimPhysics);
}
