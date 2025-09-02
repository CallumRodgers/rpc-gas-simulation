#include "PMDetectorConstructionIRPC.hh"

#include "PMHeedModel.hh"

using namespace RPCGeometry;

PMDetectorConstructionIRPC::PMDetectorConstructionIRPC(PMMainParameters *params) {
    this->params = params;
}

PMDetectorConstructionIRPC::~PMDetectorConstructionIRPC() {
}

G4VPhysicalVolume* PMDetectorConstructionIRPC::Construct() {

}

void PMDetectorConstructionIRPC::ConstructSDandField() {

    // Inicializando nosso modelo do Garfield++.
    auto* heedModel = new GarfieldInterface::PMHeedModel(gasEnvelope);
    heedModel->InitialiseGarfieldIRPC(
        GARFIELD_VOLUME_X_IRPC / 2.0, GARFIELD_VOLUME_Y_IRPC / 2.0, GARFIELD_VOLUME_Z_IRPC / 2.0,
        params->GetGasVoltage(), params->GetGasFile(),
        nStrips
    );
}
