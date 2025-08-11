#include "PMHeedModel.hh"

// Partículas a serem simuladas no Garfield++.
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

// Outros includes.
#include "PMDetectorConstruction.hh"
#include "Garfield/SolidBox.hh"

using namespace GarfieldInterface;

struct EnergyRange {
    G4double min;
    G4double max;
};

std::map<const G4ParticleDefinition*, EnergyRange> energyRangeMap;

// Construidor.
PMHeedModel::PMHeedModel(G4Envelope* gasRegion): G4VFastSimulationModel("Heed Model", gasRegion) {
    // Criando ranges de energia para os quais o Garfield irá ser chamado
    // para uma dada partícula.
    EnergyRange electronRange{};
    electronRange.min = 0.0 * eV;
    electronRange.max = 1.0 * keV;
    energyRangeMap.insert(std::pair<const G4ParticleDefinition*, EnergyRange>(G4Electron::Definition(), electronRange));
}

// Destruidor.
PMHeedModel::~PMHeedModel() {
    delete gGasMedium;
    delete gGeometry;
}

/**
 * Este método é chamado a cada step da simulação, e retorna true
 * caso os processos físicos devam ser simulados pela parametrização (Garfield++).
 * O object G4FastTrack contém informações sobre a partícula atual, e é aqui
 * que devemos chegar se ela contém a cinemática e características necessárias
 * para a simulação.
 * @return true se a partícula deve ser simulada no Garfield++.
 */
G4bool PMHeedModel::ModelTrigger(const G4FastTrack& track) {
    G4double e_kin = track.GetPrimaryTrack()->GetKineticEnergy();
    EnergyRange range = energyRangeMap.at(track.GetPrimaryTrack()->GetParticleDefinition());
    return range.min <= e_kin&&e_kin <= range.max;
}

/**
 * Aqui devemos dizer ao Geant4 as partículas que o Garfield irá simular.
 * Ao retornar false, Garfield não será chamado para uma dada partículas
 * @return true se devemos passar a partícula ao Garfield.
 */
G4bool PMHeedModel::IsApplicable(const G4ParticleDefinition& partDef) {
    for (const G4ParticleDefinition* particle : INTERFACE_PARTICLES) {
        if (particle == &partDef) {
            return true;
        }
    }
    return false;
}

/**
 * Aqui é onde devemos de fato chamar o Garfield++ para realizar a simulação.
 * os objectos G4FastTrack e G4FastStep nos dão informações sobre a partícula
 * sendo transportada. Este método só é chamado com a "aval" do IsApplicable()
 * e ModelTrigger().
 * @param track
 * @param step
 */
void PMHeedModel::DoIt(const G4FastTrack& track, G4FastStep& step) {
    // ---------------------------------------
    // Recebendo dinâmica e dados da partícula
    // ---------------------------------------
    const G4Track* partTrack = track.GetPrimaryTrack();
    const G4ParticleDefinition* partDef = partTrack->GetParticleDefinition();
    G4ThreeVector pos    = partTrack->GetPosition();
    G4ThreeVector dir    = partTrack->GetMomentumDirection();
    G4double globalTime     = partTrack->GetGlobalTime();
    G4double e_kin          = partTrack->GetKineticEnergy();

    Garfield::TrackHeed::Cluster deltaElectronCluster;

    // -------------------------
    // Executando Heed
    // -------------------------
    if (partDef->GetParticleName() == G4Electron::Definition()->GetParticleName()) {
        gTrackHeed->SetParticle("e-");
        deltaElectronCluster = gTrackHeed->TransportDeltaElectron(
            pos.x(), pos.y(), pos.z(),
            globalTime, e_kin,
            dir.x(), dir.y(), dir.z()
        );
    }

    // -----------------------------------
    // Processando resultados da simulação
    // -----------------------------------

    // Processando elétrons
    for (const auto& electron : deltaElectronCluster.electrons) {

    }

    // ---------------------
    // Finalizando interface
    // ---------------------

    // "Matando" partícula. Podemos fazer isso pois ela já perdeu sua energia ao ser transportada
    // pelo Garfield.
    step.KillPrimaryTrack();
    step.ProposePrimaryTrackPathLength(0.0);
    step.ProposeTotalEnergyDeposited(e_kin);
}

/**
 * Carregando arquivos de gás, inicializando campo elétrico, geometria, etc.
 */
void PMHeedModel::InitialiseGarfield(PMMainParameters* params) {
    // Carregando o meio (gás) sendo simulado pelo Garfield.
    gGasMedium = new Garfield::MediumMagboltz();
    gGasMedium->LoadGasFile(gGasFile);
    
    // Criando geometria.
    gGeometry = new Garfield::GeometrySimple();
    G4double lenX, lenY, lenZ;

    G4double hvSeparation; // em metros.

    switch (params->GetRPCType()) {
        default:
        case 0:
            lenX = RPCGeometry::GAS_GAP_X_MARTA / 2.0;
            lenY = RPCGeometry::GAS_GAP_Y_MARTA / 2.0;
            lenZ = RPCGeometry::GAS_GAP_Z_MARTA / 2.0;
            hvSeparation = 0.01;
            break;
        case 1:
            lenX = RPCGeometry::GAS_GAP_X_IRPC / 2.0;
            lenY = RPCGeometry::GAS_GAP_Y_IRPC / 2.0;
            lenZ = RPCGeometry::GAS_GAP_Z_IRPC / 2.0;
            hvSeparation = 0.01;
            break;
    }

    auto box = new Garfield::SolidBox(
        0., 0., 0., // Centralizando a caixa na origem.
        lenX, lenY, lenZ
    );
    gGeometry->AddSolid(box, gGasMedium);

    // Criando campo elétrico.
    gComp = new Garfield::ComponentConstant();
    gComp->SetGeometry(gGeometry);

    // Usando um campo uniforme por enquanto.
    // O campo produzido por uma dada ddp deve depender
    // da geometria em questão.
    G4double hvValue = params->GetGasVoltage();
    gComp->SetElectricField(0., 0., hvValue / hvSeparation);

    // Criando o Sensor.
    gSensor = new Garfield::Sensor(gComp);

    // Criando o Track.
    gTrackHeed = new Garfield::TrackHeed(gSensor);
    gTrackHeed->EnableDeltaElectronTransport();
    gTrackHeed->DisableMagneticField();
}





