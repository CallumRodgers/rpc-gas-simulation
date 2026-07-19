#include "MainParameters.hpp"

#include "G4SystemOfUnits.hh"

using namespace RPC;

MainParameters::MainParameters() {
    messenger = new MainMessenger(this);

    // Inicializando com valores padrões
    rpcType = 0;
    gasFile = "test.gas";
    gasHV = -9000.0;
    beamEnergy = 100.0 * MeV;
}

MainParameters::~MainParameters() {
    delete messenger;
}


