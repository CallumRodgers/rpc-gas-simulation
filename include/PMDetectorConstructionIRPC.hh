#ifndef PMDETECTORCONSTRUCTIONIRPC_HH
#define PMDETECTORCONSTRUCTIONIRPC_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "PMMainParameters.hh"
#include "G4Region.hh"

namespace RPCGeometry {
    // Constantes para a geometria da iRPC do CERN
    // TODO: Substituir com a geometria correta.
    constexpr G4double GARFIELD_VOLUME_X_IRPC = 20.0 * cm;
    constexpr G4double GARFIELD_VOLUME_Y_IRPC = 20.0 * cm;
    constexpr G4double GARFIELD_VOLUME_Z_IRPC = 1.4 * mm;

    constexpr G4int nStrips = 32;

    class PMDetectorConstructionIRPC : public G4VUserDetectorConstruction {
    public:
        explicit PMDetectorConstructionIRPC(PMMainParameters *params);
        ~PMDetectorConstructionIRPC() override;

        G4VPhysicalVolume *Construct() override;
        void ConstructSDandField() override;

        G4Region *gasEnvelope;

    private:
        PMMainParameters *params;
    };
}

#endif // PMDETECTORCONSTRUCTIONIRPC_HH
