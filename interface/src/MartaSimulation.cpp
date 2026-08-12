#include "MartaSimulation.hpp"

#include <Garfield/SolidBox.hh>

using namespace RPC::GarfieldInterface;

void MartaSimulation::initGarfield() {
    setupGas();
    setupGeometry();

    // -------------------
    // RPC Component Setup
    // -------------------

    mRPC = std::make_unique<Garfield::ComponentParallelPlate>();

    double epGas = 1.0;
    double epGlass = 1.0;
    // As espessuras estão em [cm], conforme o Garfield espera.
    double dGap = 0.2;
    double dGlass = 0.2;
    std::vector eps             = {epGlass, epGas, epGlass, epGas, epGlass};
    std::vector thickness       = {dGlass, dGap, dGlass, dGap, dGlass};
    //std::vector resistiveLayers = {0, 2, 4}; // Índices das placas resistivas.

    const unsigned int N = thickness.size();
    assert(eps.size() == N);
    mRPC->Setup(N, eps, thickness, mDetectorConfig.voltage);
    mRPC->SetGeometry(mGeometry.get());

    // -------------------
    // Sensor Setup
    // -------------------

    // Criando o Sensor.
    mSensor = std::make_unique<Garfield::Sensor>(mRPC.get());
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
    mRPC->AddPlane("Plane");
    mSensor->AddElectrode(mRPC.get(), "Plane");

    mSensor->SetTimeWindow(
        0.0,
        SIGNAL_WINDOW_NS / SIGNAL_WINDOW_BINS,
        SIGNAL_WINDOW_BINS
    );
    mViewSignal = std::make_unique<Garfield::ViewSignal>(mSensor.get());

    setupAvalanches();
    setupHeed();
}

void MartaSimulation::setupGas() {
    // Carregando o meio (gás) sendo simulado pelo Garfield.
    std::cout << "Initialising Gas Medium..." << std::endl;
    mGasMedium = std::make_unique<Garfield::MediumMagboltz>();
    mGasMedium->LoadGasFile(mDetectorConfig.gasFile);
    mGasMedium->Initialise(false);
}

void MartaSimulation::setupGeometry() {
    // Criando geometria.
    mGeometry = std::make_unique<Garfield::GeometrySimple>();

    auto box = new Garfield::SolidBox(
        0., 0., 0., // Centralizando a caixa na origem.
        0.5 * GARFIELD_VOLUME_X, 0.5 * GARFIELD_VOLUME_Y, 0.5 * GARFIELD_VOLUME_Z
    );
    mGeometry->AddSolid(box, mGasMedium.get());
}

void MartaSimulation::setupAvalanches() {

}

void MartaSimulation::setupHeed() {

}

void MartaSimulation::processEvent(EventData data) {

}
