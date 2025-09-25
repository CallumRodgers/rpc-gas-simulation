#include "PMHeedModel.hh"

// Partículas a serem simuladas no Garfield++.
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

// Outros includes.
#include "PMAnalysis.hh"
#include "PMDetectorConstructionIRPC.hh"
#include "PMDetectorConstructionMarta.hh"
#include "Garfield/SolidBox.hh"

#include "Garfield/ViewSignal.hh"
#include "TCanvas.h"

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
}

// Destruidor.
PMHeedModel::~PMHeedModel() {
    delete gGasMedium;
    delete gGeometry;
    delete gRPC;
    delete gSensor;
    delete gTrackHeed;
    delete gAvalancheGrid;
    delete gAvalancheMicroscopic;
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
    G4cout << "ModelTrigger" << G4endl;
    auto pos = track.GetPrimaryTrackLocalPosition() / cm;
    int m;
    double epsM;
    gRPC->getLayer(pos.y(), m, epsM);
    G4cout << gRPC->NumberOfLayers() << G4endl;
    G4cout << "Pos: " << pos << G4endl;
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
G4bool PMHeedModel::IsApplicable(const G4ParticleDefinition& partDef) {
    G4cout << "IsApplicable" << G4endl;
    auto modelParticles = USING_NEW_TRACK ? NEW_TRACK_PARTICLES : DELTA_ELECTRON_PARTICLES;
    for (const G4String& particleName : NEW_TRACK_PARTICLES) {
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
void PMHeedModel::DoIt(const G4FastTrack& track, G4FastStep& step) {
    if (USING_NEW_TRACK) {
        DoItNewTrack(track, step);
    } else {
        DoItDeltaElectron(track, step);
    }
}

void PMHeedModel::DoItDeltaElectron(const G4FastTrack& track, G4FastStep& step) const {
    G4cout << "Entering Garfield Interface..." << G4endl;

    // ---------------------------------------
    // Recebendo dinâmica e dados da partícula
    // ---------------------------------------

    G4cout << "Receiving particle information and dynamics..." << G4endl;

    const G4Track* partTrack = track.GetPrimaryTrack();
    const G4ParticleDefinition* partDef = partTrack->GetParticleDefinition();
    G4ThreeVector pos     = partTrack->GetPosition();
    G4ThreeVector dir    = partTrack->GetMomentumDirection();
    G4double globalTime     = partTrack->GetGlobalTime();
    G4double e_kin          = partTrack->GetKineticEnergy();

    G4cout << "Particle Position: " << pos << G4endl;
    G4cout << "Particle Direction: " << dir << G4endl;
    G4cout << "Particle Time: " << globalTime << G4endl;
    G4cout << "Particle Energy: " << e_kin / eV << G4endl;

    Garfield::TrackHeed::Cluster deltaElectronCluster;

    // -------------------------
    // Executando Heed
    // -------------------------

    G4cout << "Executing Heed..." << G4endl;

    if (partDef->GetParticleName() == G4Electron::Definition()->GetParticleName()) {

        auto posCm = pos / cm;

        G4cout << "Position (cm): " << posCm << G4endl;

        gTrackHeed->SetParticle("e-");
        deltaElectronCluster = gTrackHeed->TransportDeltaElectron(
            posCm.x(), posCm.y(), posCm.z(),
            globalTime / ns, e_kin / eV,
            dir.x(), dir.y(), dir.z()
        );
    }

    // -----------------------------------
    // Gerando avalanches a partir das ionizações secundárias
    // -----------------------------------

    G4cout << "Simulating microscopic avalanches..." << G4endl;

    // Processando elétrons
    for (const auto& electron : deltaElectronCluster.electrons) {
        gAvalancheMicroscopic->AvalancheElectron(
            electron.x, electron.y, electron.z, electron.t, 0.1
        );
        gAvalancheGrid->AddElectrons(gAvalancheMicroscopic);
    }

    G4cout << "Starting grid avalanche..." << G4endl;

    gAvalancheGrid->StartGridAvalanche();

    // ------------------------------------
    // Processando sinal induzido pela avalanche
    // ------------------------------------

    G4cout << "Processing results..." << G4endl;

    for (int i = 0; i < 64; i++) {
        G4String label = "Pad" + std::to_string(i);
        if (gSensor->GetTotalInducedCharge(label) > 0) {
            G4cout << "Exporting Signal..." << G4endl;
            gSensor->ExportSignal(label, "signal.csv");
        }
    }

    // ---------------------
    // Finalizando interface
    // ---------------------

    G4cout << "Cleaning up interface..." << G4endl;

    gSensor->ClearSignal();

    // "Matando" partícula. Podemos fazer isso pois ela já perdeu sua energia ao ser transportada
    // pelo Garfield.
    step.KillPrimaryTrack();
    step.ProposePrimaryTrackPathLength(0.0);
    step.ProposeTotalEnergyDeposited(e_kin);

    G4cout << "Garfield Interface finished." << G4endl;
}

/**
 * Executa o modelo NewTrack() do Heed.
 */
void PMHeedModel::DoItNewTrack(const G4FastTrack& track, G4FastStep& step) const {
    G4cout << "Entering Garfield Interface..." << G4endl;

    // ---------------------------------------
    // Recebendo dinâmica e dados da partícula
    // ---------------------------------------

    G4cout << "Receiving particle information and dynamics..." << G4endl;

    const G4Track* partTrack = track.GetPrimaryTrack();
    const G4ParticleDefinition* partDef = partTrack->GetParticleDefinition();
    G4ThreeVector pos    = track.GetPrimaryTrackLocalPosition();
    G4ThreeVector dir    = partTrack->GetMomentumDirection();
    G4double globalTime     = partTrack->GetGlobalTime();
    G4double e_kin          = partTrack->GetKineticEnergy() / eV;

    G4double depositedEnergy = 0.0;

    G4cout << "Particle Position: " << pos << G4endl;
    G4cout << "Particle Direction: " << dir << G4endl;
    G4cout << "Particle Time: " << globalTime << G4endl;
    G4cout << "Particle Energy: " << e_kin << G4endl;

    // -------------------------
    // Executando Heed
    // -------------------------

    G4cout << "Executing Heed..." << G4endl;

    auto posCm = pos / cm;

    posCm.setY(posCm.y() - 0.001);

    G4cout << gRPC->GetMedium(posCm.x(), posCm.y(), posCm.z()) << G4endl;

    gTrackHeed->SetParticle(partDef->GetParticleName());
    gTrackHeed->SetKineticEnergy(e_kin);
    gTrackHeed->NewTrack(
        posCm.x(), posCm.y(), posCm.z(),
        globalTime / ns,
        dir.x(), dir.y(), dir.z()
    );

    // -----------------------------------
    // Gerando avalanches a partir das ionizações secundárias
    // -----------------------------------

    G4cout << "Simulating microscopic avalanches..." << G4endl;

    // Processando elétrons
    unsigned int n = 0;
    for (const auto& cluster : gTrackHeed->GetClusters()) {
        for (const auto& electron : cluster.electrons) {
            gAvalancheMicroscopic->AvalancheElectron(
                electron.x, electron.y, electron.z, electron.t, electron.e
            );
            n++;
        }
        depositedEnergy += cluster.energy;
    }

    G4cout << "Deposited Energy: " << depositedEnergy * eV / keV << " keV" << G4endl;

    G4cout << "Microscopic Initial: " << n << G4endl;
    G4cout << "Microscopic Final: " << gAvalancheMicroscopic->GetNumberOfElectronEndpoints() << G4endl;

    // ------------------------------------
    // Processando sinal induzido pela avalanche
    // ------------------------------------

    G4cout << "Processing results..." << G4endl;

    gSensor->ExportSignal("Plane", "signal");

    // ---------------------
    // Finalizando interface
    // ---------------------

    G4cout << "Cleaning up interface..." << G4endl;

    gSensor->ClearSignal();

    step.KillPrimaryTrack();
    step.ProposeLocalEnergyDeposit(100.0 * MeV);

    G4cout << "Garfield Interface finished." << G4endl;
}

/**
 * Carregando arquivos de gás, inicializando campo elétrico, geometria, etc.
 * @param halfX Metade das dimensões da caixa do volume de gás. ATENÇÃO 1: O volume deve incluir todos os
 * gaps E as placas resistivas (e.g. vidro/bakelite). A separação dos gaps é realizada no Garfield.
 * ATENÇÃO 2: O Garfield espera que o eixo "para cima" seja o Y. Ou seja, as camadas da RPC devem planos XZ.
 * Converter se necessário a geometria do Geant4.
 * @param hv Voltagem em operação na RPC.
 * @param gasFile Arquivo de gás (gerado pelo Magboltz) a ser usado na simulação.
 * @param padX Dimensões das pads (sem contar as bordas).
 */
void PMHeedModel::InitialiseGarfieldMarta(
    G4double halfX, G4double halfY, G4double halfZ,
    G4double hv, const G4String& gasFile,
    G4double padX, G4double padZ
    ) {
    double halfXcm = halfX / cm;
    double halfYcm = halfY / cm;
    double halfZcm = halfZ / cm;

    dGasGap = 0.2 * cm;
    dResistivePlate = 0.2 * cm;

    MakeGas(gasFile);
    MakeGeometry(halfXcm, halfYcm, halfZcm);

    // -------------------
    // RPC Component Setup
    // -------------------

    gRPC = new Garfield::ComponentParallelPlate();

    G4double epGas = 1.0;
    G4double epGlass = 1.0;
    // As espessuras estão em [cm], conforme o Garfield espera.
    G4double dGap = dGasGap / cm;
    G4double dGlass = dResistivePlate / cm;
    std::vector eps             = {epGlass, epGas, epGlass, epGas, epGlass};
    std::vector thickness       = {dGlass, dGap, dGlass, dGap, dGlass};
    //std::vector resistiveLayers = {0, 2, 4}; // Índices das placas resistivas.

    const unsigned int N = thickness.size();
    assert(eps.size() == N);
    gRPC->Setup(N, eps, thickness, hv);
    gRPC->SetGeometry(gGeometry);

    // -------------------
    // Sensor Setup
    // -------------------

    // Criando o Sensor.
    gSensor = new Garfield::Sensor(gRPC);
    // Adicionando eletrodos.
    //int padsPerDim = 8;
    //double padBorderX = (2.0 * halfXcm - padsPerDim * padX) / (padsPerDim + 1);
    //double padBorderZ = (2.0 * halfZcm - padsPerDim * padZ) / (padsPerDim + 1);
    //for (int i = 0; i < padsPerDim; i++) {
    //    for (int j = 0; j < padsPerDim; j++) {
    //        const G4String label = "Pad" + std::to_string(i * 8 + j);
    //        double x = -halfXcm + padBorderX + padX * (i + 0.5);
    //        double z = -halfZcm + padBorderZ + padZ * (j + 0.5);
    //        gRPC->AddPixel(x, z, padX, padZ, label);
    //        gSensor->AddElectrode(gRPC, label);
    //    }
    //}
    gRPC->AddPlane("Plane");
    gSensor->AddElectrode(gRPC, "Plane");

    gSensor->SetTimeWindow(
        0.0,
        PMAnalysis::SIGNAL_WINDOW_NS / PMAnalysis::SIGNAL_WINDOW_BINS,
        PMAnalysis::SIGNAL_WINDOW_BINS
    );
    gViewSignal = new Garfield::ViewSignal(gSensor);

    SetupAvalanches(halfXcm, halfYcm, halfZcm);
    SetupHeed();
}

/**
 * Carregando arquivos de gás, inicializando campo elétrico, geometria, etc.
 * @param halfX Metade das dimensões da caixa do volume de gás. ATENÇÃO 1: O volume deve incluir todos os
 * gaps E as placas resistivas (e.g. vidro/bakelite). A separação dos gaps é realizada no Garfield.
 * ATENÇÃO 2: O Garfield espera que o eixo "para cima" seja o Y. Ou seja, as camadas da RPC devem planos XZ.
 * Converter se necessário a geometria do Geant4.
 * @param hv Voltagem em operação na RPC.
 * @param gasFile Arquivo de gás (gerado pelo Magboltz) a ser usado na simulação.
 * @param isMartaRPC Se a RPC sendo simulada é a Marta (true) ou a iRPC (false).
 */
void PMHeedModel::InitialiseGarfieldIRPC(
    G4double halfX, G4double halfY, G4double halfZ,
    G4double hv, const G4String& gasFile,
    G4int nStrips
    ) {
    double halfXcm = halfX / cm;
    double halfYcm = halfY / cm;
    double halfZcm = halfZ / cm;

    MakeGas(gasFile);
    MakeGeometry(halfXcm, halfYcm, halfZcm);

    // -------------------
    // RPC Component Setup
    // -------------------
    gRPC = new Garfield::ComponentParallelPlate();

    G4double epGas = 1.0;
    G4double epBakelite = 4.0;
    // As espessuras estão em [cm], conforme o Garfield espera.
    G4double dGap = 0.14;
    G4double dBakelite = 0.14;
    std::vector eps = {epBakelite, epGas, epBakelite, epGas, epBakelite};
    std::vector thickness = {dBakelite, dGap, dBakelite, dGap, dBakelite};
    std::vector resistiveLayers = {0, 2, 4};

    const unsigned int N = thickness.size();
    assert(eps.size() == N);
    gRPC->Setup(N, eps, thickness, hv, resistiveLayers);
    gRPC->SetGeometry(gGeometry);
    gRPC->SetMedium(gGasMedium);

    // -----------------
    // Sensor Setup
    // -----------------

    // Criando o Sensor.
    gSensor = new Garfield::Sensor(gRPC);
    // Adicionando eletrodos.
    for (int i = 0; i < 8; i++) {

    }

    SetupAvalanches(halfXcm, halfYcm, halfZcm);
    SetupHeed();
}

void PMHeedModel::MakeGas(const G4String &gasFile) {
    // Carregando o meio (gás) sendo simulado pelo Garfield.
    G4cout << "Initialising Gas Medium..." << G4endl;
    gGasMedium = new Garfield::MediumMagboltz();
    gGasMedium->LoadGasFile(gasFile);
    gGasMedium->Initialise(false);
}

void PMHeedModel::MakeGeometry(double halfXcm, double halfYcm, double halfZcm) {
    // Criando geometria.
    gGeometry = new Garfield::GeometrySimple();

    auto box = new Garfield::SolidBox(
        0., 0., 0., // Centralizando a caixa na origem.
        halfXcm, halfYcm, halfZcm
    );
    gGeometry->AddSolid(box, gGasMedium);
}

void ElectronAttachment(double x, double y, double z, double t, int type, int level, Garfield::Medium *m) {
    G4cout << "Electron Attached" << G4endl;
}


void PMHeedModel::SetupAvalanches(double halfXcm, double halfYcm, double halfZcm) {
    // Criando objetos de avalanche.
    gAvalancheMicroscopic = new Garfield::AvalancheMicroscopic(gSensor);
    gAvalancheGrid = new Garfield::AvalancheGrid(gSensor);

    // Criando grade.
    int stepsPerCm = 1e4;
    gAvalancheGrid->SetGrid(
        -halfXcm, halfXcm, static_cast<int>(2 * halfXcm * stepsPerCm), // X axis.
        -halfYcm, halfYcm, static_cast<int>(2 * halfYcm * stepsPerCm), // Y axis.
        -halfZcm, halfZcm, static_cast<int>(2 * halfZcm * stepsPerCm) // Z axis.
    );
    gAvalancheGrid->EnableDebugging();

    // Por algum motivo ele trava ao usar o potencial.
    gAvalancheMicroscopic->UseWeightingPotential();
    gAvalancheMicroscopic->EnableSignalCalculation();
    // Tempo em ns que usaremos a simulação microscópica antes de passar para a em grade.
}

void PMHeedModel::SetupHeed() {
    // Criando o Track.
    gTrackHeed = new Garfield::TrackHeed(gSensor);
    gTrackHeed->EnableDeltaElectronTransport();
    gTrackHeed->DisableMagneticField();
}



