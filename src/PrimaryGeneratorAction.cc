#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(), fParticleGun(0) 
{
    G4int nofParticles = 1;
    fParticleGun = new G4ParticleGun(nofParticles);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(5. * GeV);

    G4double rWorld = 1.5 * m;

    G4double theta_pos = G4RandFlat::shoot(0., 0.5 * CLHEP::pi);
    G4double phi_pos = G4RandFlat::shoot(0., 2. * CLHEP::pi);

    G4double x_pos = rWorld * std::sin(theta_pos) * std::cos(phi_pos);
    G4double y_pos = rWorld * std::sin(theta_pos) * std::sin(phi_pos);
    G4double z_pos = rWorld * std::cos(theta_pos);
    
    G4ThreeVector pos(x_pos, y_pos, z_pos);
    fParticleGun->SetParticlePosition(pos);

    G4ThreeVector mom_direction = -pos;
    fParticleGun->SetParticleMomentumDirection(mom_direction.unit());
    
    fParticleGun->GeneratePrimaryVertex(anEvent);
}