#ifndef DETECTORCONSTRUCTIONIRPC_HH
#define DETECTORCONSTRUCTIONIRPC_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "MainParameters.hpp"
#include "G4Region.hh"

namespace RPC::Geometry {
    // Constantes para a geometria da iRPC do CERN
    constexpr G4double GARFIELD_VOLUME_X_IRPC = 0.5 * m;
    constexpr G4double GARFIELD_VOLUME_Y_IRPC = 6 * 1.4 * mm;
    constexpr G4double GARFIELD_VOLUME_Z_IRPC = 0.5 * m;

    constexpr G4int nStrips = 22;

    class DetectorConstructionIRPC : public G4VUserDetectorConstruction {
    public:
        explicit DetectorConstructionIRPC(MainParameters *params);
        ~DetectorConstructionIRPC() override;

        G4VPhysicalVolume *Construct() override;
        void ConstructSDandField() override;

        G4Region *garfieldEnvelope;

    private:
        MainParameters *params;
    };
}

#endif // DETECTORCONSTRUCTIONIRPC_HH
