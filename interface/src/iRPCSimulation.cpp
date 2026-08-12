#include "iRPCSimulation.hpp"

#include <Garfield/SolidBox.hh>
#include <Garfield/AvalancheMicroscopic.hh>

using namespace RPC::GarfieldInterface;

void iRPCSimulation::initGarfield() {
    setupGas();
    setupGeometry();

    // -------------------
    // RPC Component Setup
    // -------------------
    mRPC = std::make_unique<Garfield::ComponentParallelPlate>();

    double epGas = 1.0;
    double epBakelite = 4.0;
    // As espessuras estão em [cm], conforme o Garfield espera.
    double dGap = 0.14;
    double dBakelite = 0.14;
    //std::vector eps = {epBakelite, epGas, epBakelite, epGas, epBakelite};
    std::vector eps = {epGas, epGas, epGas, epGas, epGas};
    std::vector thickness = {dBakelite, dGap, dBakelite, dGap, dBakelite};
    std::vector resistiveLayers = {0, 2, 4};

    const unsigned int N = thickness.size();
    assert(eps.size() == N);
    mRPC->SetMedium(mGasMedium.get());
    mRPC->Setup(N, eps, thickness, mDetectorConfig.voltage);
    mRPC->SetGeometry(mGeometry.get());
    mRPC->SetIntegrationPrecision(1e-9);

    // -----------------
    // Sensor Setup
    // -----------------

    // Criando o Sensor.
    mSensor = std::make_unique<Garfield::Sensor>(mRPC.get());

    mRPC->AddPlane("Plane");
    mSensor->AddElectrode(mRPC.get(), "Plane");

    //gRPC->EnableDebugging();
    //gSensor->EnableDebugging();

    mSensor->SetTimeWindow(
        0.0,
        SIGNAL_WINDOW_NS / SIGNAL_WINDOW_BINS,
        SIGNAL_WINDOW_BINS
    );

    setupAvalanches();
    setupHeed();
}

void iRPCSimulation::setupGeometry() {
    // Criando geometria.
    mGeometry = std::make_unique<Garfield::GeometrySimple>();

    auto box = new Garfield::SolidBox(
        0., 0., 0., // Centralizando a caixa na origem.
        0.5 * GARFIELD_VOLUME_X, 0.5 * GARFIELD_VOLUME_Y, 0.5 * GARFIELD_VOLUME_Z
    );
    mGeometry->AddSolid(box, mGasMedium.get());
}

void iRPCSimulation::setupGas() {
    // Carregando o meio (gás) sendo simulado pelo Garfield.
    std::cout << "Initialising Gas Medium..." << std::endl;
    mGasMedium = std::make_unique<Garfield::MediumMagboltz>();
    mGasMedium->LoadGasFile(mDetectorConfig.gasFile);
    mGasMedium->Initialise(false);
}

void iRPCSimulation::setupAvalanches() {
    // Criando objetos de avalanche.
    mAvalancheMicroscopic = std::make_unique<Garfield::AvalancheMicroscopic>(mSensor.get());
    mAvalancheGrid = std::make_unique<Garfield::AvalancheGrid>(mSensor.get());

    mAvalancheGrid->EnableDebugging();

    // Por algum motivo ele trava ao usar o potencial.
    mAvalancheMicroscopic->UseWeightingPotential();
    mAvalancheMicroscopic->EnableSignalCalculation();
    // Tempo em ns que usaremos a simulação microscópica antes de passar para a em grade.
    mAvalancheMicroscopic->SetTimeWindow(0., 0.20);
}

void iRPCSimulation::setupHeed() {
    // Criando o Track.
    mTrackHeed = std::make_unique<Garfield::TrackHeed>(mSensor.get());
    mTrackHeed->EnableDeltaElectronTransport();
    mTrackHeed->CrossInactiveMedia(true);
    mTrackHeed->DisableMagneticField();
}

void iRPCSimulation::processEvent(EventData data) {
    std::cout << "Processing event!" << std::endl;
}
