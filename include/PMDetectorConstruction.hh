#ifndef PMDETECTORCONSTRUCTION_HH
#define PMDETECTORCONSTRUCTION_HH

#include <G4FastTrack.hh>

#include "G4VUserDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4SDManager.hh"
#include "PMMainParameters.hh"

#include "PMSensitiveDetector.hh"

namespace RPCGeometry {

    // Constantes para a geometria da RPC Marta.
    constexpr G4double GAS_GAP_X_MARTA = 120.0 * cm;
    constexpr G4double GAS_GAP_Y_MARTA = 152.0 * cm;
    constexpr G4double GAS_GAP_Z_MARTA = 2.0 * mm;
    // Constantes para a geometria da iRPC do CERN
    // TODO: Substituir com a geometria correta.
    constexpr G4double GAS_GAP_X_IRPC = 20.0 * cm;
    constexpr G4double GAS_GAP_Y_IRPC = 20.0 * cm;
    constexpr G4double GAS_GAP_Z_IRPC = 1.4 * mm;

    class PMDetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        explicit PMDetectorConstruction(PMMainParameters* params);
        ~PMDetectorConstruction() override;

        G4VPhysicalVolume* ConstructGlassRPC();
        G4VPhysicalVolume* ConstructRPCPrototype();

        G4VPhysicalVolume *Construct() override;

        G4Envelope *gasEnvelope;

    private:
        PMMainParameters* params;

        G4LogicalVolume *logicPad;

        virtual void ConstructSDandField();
    };
}

#endif