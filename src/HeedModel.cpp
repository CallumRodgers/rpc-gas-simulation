#include "HeedModel.hpp"

// Partículas a serem simuladas no Garfield++.
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

// Outros includes.
#include <G4AnalysisManager.hh>
#include <G4Box.hh>
#include <TApplication.h>

#include "Analysis.hpp"
#include "DetectorConstructionIRPC.hpp"
#include "DetectorConstructionMarta.hpp"
#include "TCanvas.h"

using namespace RPC::GarfieldInterface;

struct EnergyRange {
    G4double min;
    G4double max;
};

std::map<const G4ParticleDefinition*, EnergyRange> energyRangeMap;

// Construidor.
HeedModel::HeedModel(G4Envelope* gasRegion, const DetectorType type, const DetectorConfig& config)
    : G4VFastSimulationModel("Heed Model", gasRegion),
      mInterface(type, config)
{
    // Criando ranges de energia para os quais o Garfield irá ser chamado
    // para uma dada partícula.
    EnergyRange electronRange{}, muonRange{};
    electronRange.min = 0.0 * eV;
    electronRange.max = 1.0 * keV;
    muonRange.min = 1.0 * MeV;
    muonRange.max = 10.0 * TeV;

    #define PAIR std::pair<const G4ParticleDefinition*, EnergyRange>
    energyRangeMap.insert(PAIR(G4Electron::Definition(), electronRange));
    energyRangeMap.insert(PAIR(G4Positron::Definition(), electronRange));
    energyRangeMap.insert(PAIR(G4MuonMinus::Definition(), muonRange));
    energyRangeMap.insert(PAIR(G4MuonPlus::Definition(), muonRange));
    #undef PAIR

    mInterface.prepareDetectors();
}

/**
 * Este método é chamado a cada step da simulação, e retorna true
 * caso os processos físicos devam ser simulados pela parametrização (Garfield++).
 * O object G4FastTrack contém informações sobre a partícula atual, e é aqui
 * que devemos chegar se ela contém a cinemática e características necessárias
 * para a simulação.
 * @return true se a partícula deve ser simulada no Garfield++.
 */
G4bool HeedModel::ModelTrigger(const G4FastTrack& track) {
    G4cout << "ModelTrigger" << G4endl;
    int m;
    double epsM;
    if (true) { // Por algum motivo os índices começam do 1 no getLayer().
        G4double e_kin = track.GetPrimaryTrack()->GetKineticEnergy();
        EnergyRange range = energyRangeMap.at(track.GetPrimaryTrack()->GetParticleDefinition());
        return range.min <= e_kin&&e_kin <= range.max;
    }
    G4cout << "Wrong layer.";
    // A camada que a partícula está é uma placa resistiva
    // (vidro/bakelite), devemos esperar até que ela esteja
    // no gás.
    return false;
}

/**
 * Aqui devemos dizer ao Geant4 as partículas que o Garfield irá simular.
 * Ao retornar false, Garfield não será chamado para uma dada partículas
 * @return true se devemos passar a partícula ao Garfield.
 */
G4bool HeedModel::IsApplicable(const G4ParticleDefinition& partDef) {
    G4cout << "IsApplicable" << G4endl;
    for (const G4String& particleName : INTERFACE_PARTICLES) {
        if (particleName == partDef.GetParticleName()) {
            return true;
        }
    }
    return false;
}

/**
 * Aqui é onde devemos de fato chamar o Garfield++ para realizar a simulação.
 * os objetos G4FastTrack e G4FastStep nos dão informações sobre a partícula
 * sendo transportada. Este método só é chamado com a "aval" do IsApplicable()
 * e ModelTrigger().
 * @param track
 * @param step
 */
void HeedModel::DoIt(const G4FastTrack& track, G4FastStep& step) {
    G4cout << "Entering Garfield Interface..." << G4endl;

    // ---------------------------------------
    // Recebendo dinâmica e dados da partícula
    // ---------------------------------------

    G4cout << "Receiving particle information and dynamics..." << G4endl;

    const G4Track* partTrack = track.GetPrimaryTrack();
    const G4ParticleDefinition* partDef = partTrack->GetParticleDefinition();
    G4ThreeVector pos       = track.GetPrimaryTrackLocalPosition();
    G4ThreeVector dir    = partTrack->GetMomentumDirection();
    G4double globalTime     = partTrack->GetGlobalTime();
    G4double e_kin          = partTrack->GetKineticEnergy() / eV;

    G4cout << "Particle Type: " << partDef->GetParticleName() << G4endl;
    G4cout << "Particle Position: " << pos / cm << G4endl;
    G4cout << "Particle Direction: " << dir << G4endl;
    G4cout << "Particle Time: " << globalTime << G4endl;
    G4cout << "Particle Energy: " << e_kin << G4endl;

    auto type = ParticleType::MUON_MINUS; // default.
    if (partDef == G4Electron::Definition()) {
        type = ParticleType::ELECTRON;
    } else if (partDef == G4Positron::Definition()) {
        type = ParticleType::POSITRON;
    } else if (partDef == G4MuonMinus::Definition()) {
        type = ParticleType::MUON_MINUS;
    } else if (partDef == G4MuonPlus::Definition()) {
        type = ParticleType::MUON_PLUS;
    }

    mInterface.transmit({
        .type = type,
        .position = vec3(pos.x() / cm, pos.y() / cm, pos.z() / cm),
        .momentum = vec3(dir.x(), dir.y(), dir.z()),
        .time = globalTime,
        .energy = e_kin
    });

    G4cout << "Transporting particle to after the volume" << G4endl;

    G4ThreeVector min, max;
    track.GetEnvelopeSolid()->BoundingLimits(min, max);

    step.ProposePrimaryTrackFinalPosition(G4ThreeVector(pos.x(), min.y(), pos.z()));

    G4cout << "Exiting Garfield Interface..." << G4endl;
}
