#include "PMPhysicsList.hh"

#include "PMHeedModel.hh"
#include "G4EmStandardPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4DecayPhysics.hh"

PMPhysicsList::PMPhysicsList()
{
    // EM Physics
    RegisterPhysics(new G4EmStandardPhysics());

    // Radioactive decay physics
    RegisterPhysics(new G4RadioactiveDecayPhysics());

    // Decay physics
    RegisterPhysics(new G4DecayPhysics());
}

PMPhysicsList::~PMPhysicsList()
{
    delete fastSimPhysics;
}

void PMPhysicsList::RegisterParametrization()
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
