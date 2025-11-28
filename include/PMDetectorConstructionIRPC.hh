#ifndef PMDETECTORCONSTRUCTIONIRPC_HH
#define PMDETECTORCONSTRUCTIONIRPC_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "PMMainParameters.hh"
#include "G4Region.hh"

namespace RPCGeometry {
    // Constantes para a geometria da iRPC do CERN
    constexpr G4double GARFIELD_VOLUME_X_IRPC = 0.5 * m;
    constexpr G4double GARFIELD_VOLUME_Y_IRPC = 2.0 * mm + 6 * 1.4 * mm;
    constexpr G4double GARFIELD_VOLUME_Z_IRPC = 0.5 * m;

    constexpr G4int nStrips = 22;

    class PMDetectorConstructionIRPC : public G4VUserDetectorConstruction {
    public:
        explicit PMDetectorConstructionIRPC(PMMainParameters *params);
        ~PMDetectorConstructionIRPC() override;

        G4VPhysicalVolume *Construct() override;
        void ConstructSDandField() override;

        G4Region *garfieldEnvelope;

    private:
        PMMainParameters *params;
    };
}

#endif // PMDETECTORCONSTRUCTIONIRPC_HH
