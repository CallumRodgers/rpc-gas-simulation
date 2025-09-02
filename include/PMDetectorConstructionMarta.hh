#ifndef PMDETECTORCONSTRUCTIONMARTA_HH
#define PMDETECTORCONSTRUCTIONMARTA_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "PMMainParameters.hh"
#include "G4Region.hh"

namespace RPCGeometry {

    // Constantes para a geometria da RPC Marta.
    constexpr G4double GARFIELD_VOLUME_X_MARTA = 120.0 * cm;
    constexpr G4double GARFIELD_VOLUME_Y_MARTA = 5 * 2.0 * mm;
    constexpr G4double GARFIELD_VOLUME_Z_MARTA = 152.0 * cm;

    constexpr G4double PAD_X_MARTA = 14.0 * cm;
    constexpr G4double PAD_Z_MARTA = 18.0 * cm;

    class PMDetectorConstructionMarta : public G4VUserDetectorConstruction {
    public:
        explicit PMDetectorConstructionMarta(PMMainParameters* params);
        ~PMDetectorConstructionMarta() override;

        G4VPhysicalVolume *Construct() override;
        void ConstructSDandField() override;

        G4Region *garfieldEnvelope;
        G4LogicalVolume *garfieldVolume;

    private:
        PMMainParameters *params;
        G4LogicalVolume *logicPad;
    };
}

#endif // PMDETECTORCONSTRUCTIONMARTA_HH
