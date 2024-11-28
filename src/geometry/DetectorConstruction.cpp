//
// Created by callum on 24/11/24.
//

#include "geometry/DetectorConstruction.h"

#include <stdexcept>

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4NistManager.hh"

using namespace CLHEP;

int selectedType = 0;

DetectorConstruction::DetectorConstruction(const int type) {
    if ((type != TYPE_GLASS) and (type != TYPE_iRPC_PROTOTYPE)) {
        throw std::invalid_argument("Invalid detector type.");
    }
    selectedType = type;
}

/**
 * Creates the detector geometry representing the glass RPC.
 * @return the physical volume containing the glass RPC.
 */
G4VPhysicalVolume* ConstructGlassRPC() {

    // #########################################################
    // Creating World box: where the detector will be contained.
    // #########################################################

    // Geant4 considers the length to go from the origin to the value specified,
    // so for example a 4.0m length in the x direction (centred at the origin) would give a 2.0m world_x.
    G4double world_x = 1.0 * m;
    G4double world_y = 1.0 * m;
    G4double world_z = 0.2 * m;

    auto worldBox = new G4Box("World", world_x, world_y, world_z);

    G4NistManager* man = G4NistManager::Instance();
    G4Material* air = man->FindOrBuildMaterial("G4_AIR");

    auto worldLog = new G4LogicalVolume(worldBox, air, "World");

    G4VPhysicalVolume* worldPhysical = new G4PVPlacement(nullptr,
        G4ThreeVector(0, 0, 0),
        worldLog,
        "World",
        nullptr,
        false,
        0);

    return worldPhysical;
}

/**
 * Creates the detector geometry of CERN's iRPC prototype.
 * @return the physical volume representing the whole iRPC.
 */
G4VPhysicalVolume* ConstructiRPCPrototype() {
    // TODO: Implement CERN iRPC geometry.
}

/**
 * This is where we construct the geometry of the detector.
 * @return the physical volume representing the RPC used. It may be either the glass RPC or the CERN iRPC prototype.
 */
G4VPhysicalVolume* DetectorConstruction::Construct() {
    switch (selectedType) {
        case TYPE_GLASS:
            return ConstructGlassRPC();
        case TYPE_iRPC_PROTOTYPE:
            return ConstructiRPCPrototype();
        default:
            throw std::invalid_argument("Invalid detector type.");
    }
}


