#include "PMMainParameters.hh"

#include "G4SystemOfUnits.hh"

PMMainParameters::PMMainParameters() {
    messenger = new PMMainMessenger(this);

    // Inicializando com valores padrões
    rpcType = 0;
    gasFile = "test.gas";
    gasHV = 9000.0;
    beamEnergy = 100.0 * MeV;
}

PMMainParameters::~PMMainParameters() {
    delete messenger;
}


