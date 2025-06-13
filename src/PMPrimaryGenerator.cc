#include "PMPrimaryGenerator.hh"
#include "Randomize.hh"  

PMPrimaryGenerator::PMPrimaryGenerator()
{
    fParticleGun = new G4ParticleGun(1);
}

PMPrimaryGenerator::~PMPrimaryGenerator()
{
    delete fParticleGun;
}

void PMPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{
G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
fParticleGun->SetParticleDefinition(particle);
fParticleGun->SetParticleEnergy(5. * GeV);

G4double rWorld = 1.5 * m;

G4double theta_pos = G4RandFlat::shoot(0., 0.5 * CLHEP::pi); // [0, pi/2] para Z positivo
G4double phi_pos = G4RandFlat::shoot(0., 2. * CLHEP::pi);    // [0, 2pi]

G4double x_pos = rWorld * std::sin(theta_pos) * std::cos(phi_pos);
G4double y_pos = rWorld * std::sin(theta_pos) * std::sin(phi_pos);
G4double z_pos = rWorld * std::cos(theta_pos);
G4ThreeVector pos(x_pos, y_pos, z_pos);
fParticleGun->SetParticlePosition(pos);

G4double theta_mom = std::acos(G4RandFlat::shoot(-1.0, 1.0)); // [0, pi]
G4double phi_mom = G4RandFlat::shoot(0., 2. * CLHEP::pi);     // [0, 2pi]

G4double mom_x = std::sin(theta_mom) * std::cos(phi_mom);
G4double mom_y = std::sin(theta_mom) * std::sin(phi_mom);
G4double mom_z = std::cos(theta_mom);
G4ThreeVector mom_direction(mom_x, mom_y, mom_z);
fParticleGun->SetParticleMomentumDirection(mom_direction.unit());

fParticleGun->GeneratePrimaryVertex(anEvent);
}
