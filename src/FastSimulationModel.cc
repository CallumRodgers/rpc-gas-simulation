#include "FastSimulationModel.hh"
#include <iostream>
#include "G4Electron.hh"
#include "G4GDMLParser.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"
#include "G4coutDestination.hh" 
#include "G4VSolid.hh" 

FastSimulationModel::FastSimulationModel(G4String modelName, G4Region* envelope) : G4VFastSimulationModel(modelName, envelope) {
  G4cout << "[LOG] FastSimulationModel -> Constructor called for model: " << modelName << G4endl;
  fGarfieldPhysics = GarfieldPhysics::GetInstance();
}

FastSimulationModel::FastSimulationModel(G4String modelName) : G4VFastSimulationModel(modelName) {
  G4cout << "[LOG] FastSimulationModel -> Constructor called for model: " << modelName << G4endl;
  fGarfieldPhysics = GarfieldPhysics::GetInstance();
}

FastSimulationModel::~FastSimulationModel() {}

G4bool FastSimulationModel::IsApplicable(const G4ParticleDefinition& particleType) {
  G4String particleName = particleType.GetParticleName();
  bool result = fGarfieldPhysics->FindParticleName(particleName, "garfield");
  G4cout << "[LOG] FastSimulationModel::IsApplicable -> Particle: " << particleName << " -> Applicable? " << (result ? "Yes" : "No") << G4endl;
  return result;
}

G4bool FastSimulationModel::ModelTrigger(const G4FastTrack& fastTrack) {
  double ekin_MeV = fastTrack.GetPrimaryTrack()->GetKineticEnergy() / MeV;
  G4String particleName =
      fastTrack.GetPrimaryTrack()->GetParticleDefinition()->GetParticleName();
  bool result = fGarfieldPhysics->FindParticleNameEnergy(particleName, ekin_MeV, "garfield");
  G4cout << "[LOG] FastSimulationModel::ModelTrigger -> Particle: " << particleName << ", E_kin: " << ekin_MeV << " MeV. Trigger? " << (result ? "Yes" : "No") << G4endl;
  return result;
}

void FastSimulationModel::DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep) {
    G4cout << "\n========================================================================" << G4endl;
    G4cout << "[LOG] FastSimulationModel::DoIt -> TRIGGERED! Handing track to GarfieldPhysics." << G4endl;

    // --- Obtenha informações da trajetória primária ---
    const G4Track* track = fastTrack.GetPrimaryTrack();
    G4ThreeVector localpos = fastTrack.GetPrimaryTrackLocalPosition();
    G4ThreeVector localdir = fastTrack.GetPrimaryTrackLocalDirection();
    double ekin_MeV = track->GetKineticEnergy() / MeV;
    double globalTime = track->GetGlobalTime();
    G4String particleName = track->GetParticleDefinition()->GetParticleName();

    G4cout << "    -> Particle: " << particleName << ", E_kin: " << ekin_MeV << " MeV" << G4endl;
    G4cout << "    -> Position (cm): (" << localpos.x() / CLHEP::cm << ", " << localpos.y() / CLHEP::cm << ", " << localpos.z() / CLHEP::cm << ")" << G4endl;
    G4cout << "========================================================================\n" << G4endl;
    
    // --- Calcule a distância de saída do volume ---
    G4VSolid* solid = track->GetVolume()->GetLogicalVolume()->GetSolid();
    G4double distance = solid->DistanceToOut(localpos, localdir);

    if (distance < 0.) distance = 0.;

    // --- Chame a simulação do Garfield++ ---
    if (particleName == "kaon+") {
        particleName = "K+";
    } else if (particleName == "kaon-") {
        particleName = "K-";
    } else if (particleName == "anti_proton") {
        particleName = "anti-proton";
    }
    
    fGarfieldPhysics->DoIt(
        particleName, ekin_MeV, globalTime, localpos.x() / CLHEP::cm,
        localpos.y() / CLHEP::cm, localpos.z() / CLHEP::cm,
        localdir.x(), localdir.y(), localdir.z());

    double edep_MeV = fGarfieldPhysics->GetEnergyDeposit_MeV();
    
    // --- Calcule o estado final da partícula primária ---
    G4double final_ekin_MeV = ekin_MeV - edep_MeV;
    if (final_ekin_MeV < 0.) final_ekin_MeV = 0.;
    G4ThreeVector exit_pos = localpos + distance * localdir;

    // --- Atualize o G4FastStep com o estado final ---
    fastStep.ProposePrimaryTrackPathLength(distance);
    fastStep.ProposeTotalEnergyDeposited(edep_MeV * MeV);
    
    // Define o estado da partícula na borda de saída do volume
    fastStep.ProposePrimaryTrackFinalPosition(exit_pos);
    fastStep.ProposePrimaryTrackFinalKineticEnergy(final_ekin_MeV * MeV);
    fastStep.ProposePrimaryTrackFinalMomentumDirection(localdir);
    fastStep.ProposePrimaryTrackFinalPolarization(track->GetPolarization());

    // --- Geração de secundários (se houver) ---
    if (!fGarfieldPhysics->GetCreateSecondariesInGeant4()) return;
    const auto& secondaryParticles = fGarfieldPhysics->GetSecondaryParticles();

    if (secondaryParticles.empty()) return;
    fastStep.SetNumberOfSecondaryTracks(secondaryParticles.size());

    G4double totalEnergySecondaries_MeV = 0.;

    for (const auto& sp : secondaryParticles) {
        G4double eKin_MeV = sp.getEkin_MeV();
        G4double time = sp.getTime();
        G4ThreeVector momentumDirection(sp.getDX(), sp.getDY(), sp.getDZ());
        G4ThreeVector position(sp.getX_mm(), sp.getY_mm(), sp.getZ_mm());
        if (sp.getParticleName() == "e-") {
            G4DynamicParticle particle(G4Electron::ElectronDefinition(),
                                     momentumDirection, eKin_MeV);
            fastStep.CreateSecondaryTrack(particle, position, time, true);
            totalEnergySecondaries_MeV += eKin_MeV;
        } else if (sp.getParticleName() == "gamma") {
           G4DynamicParticle particle(G4Gamma::GammaDefinition(),
                                      momentumDirection, eKin_MeV);
          fastStep.CreateSecondaryTrack(particle, position, time, true);
          totalEnergySecondaries_MeV += eKin_MeV;
        }
    }
}