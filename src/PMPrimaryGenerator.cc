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
    // Define a partícula (mu-)
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(5. * GeV); 

    G4double r = 1.5 * m;
    G4double theta_pos = std::acos(G4RandFlat::shoot(-1.0, 1.0));  // [0, pi]
    G4double phi_pos = G4RandFlat::shoot(0., 2. * CLHEP::pi);      // [0, 2pi]

    G4double x = r * std::sin(theta_pos) * std::cos(phi_pos);
    G4double y = r * std::sin(theta_pos) * std::sin(phi_pos);
    G4double z = r * std::cos(theta_pos);
    G4ThreeVector pos(x, y, z);
    fParticleGun->SetParticlePosition(pos);

    G4double theta_mom = std::acos(G4RandFlat::shoot(-1.0, 1.0));  // [0, pi]
    G4double phi_mom = G4RandFlat::shoot(0., 2. * CLHEP::pi);      // [0, 2pi]

    G4double mom_x = G4RandFlat::shoot(-10, 10 *GeV);  
    G4double mom_y = G4RandFlat::shoot(-10, 10 *GeV);
    G4double mom_z = G4RandFlat::shoot(-10, 10 *GeV);
    G4ThreeVector mom(mom_x, mom_y, mom_z);
    fParticleGun->SetParticleMomentumDirection(mom);

    // --- Gera o evento ---
    fParticleGun->GeneratePrimaryVertex(anEvent);

}
