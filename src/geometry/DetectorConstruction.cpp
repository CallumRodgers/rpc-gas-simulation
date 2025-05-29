
#include "geometry/DetectorConstruction.h"

#include <G4Sphere.hh>
#include <stdexcept>

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "geometry/SensitiveDetector.h"

using namespace CLHEP;

struct G4area {
    G4double x;
    G4double y;
};

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
G4VPhysicalVolume* DetectorConstruction::ConstructGlassRPC() {

    G4NistManager* man = G4NistManager::Instance();

    // In this function I adopted the standard of prefixing dimensions with "d",
    // elements with "el", materials with "m".

    // ######### //
    // MATERIALS //
    // ######### //
    G4String name;
    G4double z, a, density;
    G4Element* elH = man->FindOrBuildElement(1);
    G4Element* elC = man->FindOrBuildElement(6);
    G4Element* elO = man->FindOrBuildElement(8);
    G4Element* elF = man->FindOrBuildElement(9);
    G4double r134aDensity = 1.0 * kg/m3;

    G4Material* mAir = man->FindOrBuildMaterial("G4_AIR");
    G4Material* mR134a = new G4Material(name="R134a", r134aDensity, 3);
    mR134a->AddElement(elH, 2);
    mR134a->AddElement(elC, 2);
    mR134a->AddElement(elF, 4);

    G4Material *mAcrylic = new G4Material("Acrylic", 1.18 * g / cm3, 3);
    mAcrylic->AddElement(elH, 8);
    mAcrylic->AddElement(elC, 5);
    mAcrylic->AddElement(elO, 2);


    G4Material *mGlass = man->FindOrBuildMaterial("G4_GLASS_PLATE");
    G4Material *mNylon = man->FindOrBuildMaterial("G4_NYLON");
    G4Material *mGraphite = man->FindOrBuildMaterial("G4_GRAPHITE_POROUS");
    G4Material * mAluminum = new G4Material(name="Aluminum", z=13.0, a=26.98*g/mole, density=2.700*g/cm3);
    G4Material * mCopper = man->FindOrBuildMaterial("G4_Cu");
    //G4Material* mFibreGlass = new G4Material(name="FibreGlass", density=1.85 * g/cm3, 2);
    G4Material* mFibreGlass = man->FindOrBuildMaterial("G4_GLASS_LEAD");

    // ########## //
    // DIMENSIONS //
    // ########## //
    G4double dGap = 2.0 * mm;
    G4double dGlassThickness = 2.0 * mm;
    G4area dAcrylic = {155.0 * cm, 125.0 * cm};
    G4double dAcrylicThickness = 1.0 * cm;
    G4area dAluminum = {165.0 * cm, 128.5 * cm};
    G4double dAluminumThickness = 2.5 * cm;
    G4area dGraphite = {148.5 * cm, 119 * cm};
    G4area dPad = {18.5 * cm, 14.5 * cm};
    G4area dDetector = dGraphite;
    G4double dCopperThickness = 35.0 * um;
    G4double dGlassFiberThickness = 1.0 * mm;

    // #########################################################
    // Creating World box: where the detector will be contained.
    // #########################################################

    // Geant4 considers the length to go from the origin to the value specified,
    // so for example a 4.0m length in the x direction (centred at the origin) would give a 2.0m world_x.
    G4double dWorld_x = 2.0 * m;
    G4double dWorld_y = 2.0 * m;
    G4double dWorld_z = 4.0 * m;

    auto worldSphere = new G4Sphere("World", 0.0 * cm, 5.0 * m, 0.0, 2.0 * pi * rad, 0.0, pi * rad);
    auto worldLog = new G4LogicalVolume(worldSphere, mAir, "World");
    G4VPhysicalVolume* worldPhysical = new G4PVPlacement(nullptr,
        G4ThreeVector(0, 0, 0),
        worldLog,
        "World",
        nullptr,
        false,
        0);

    // ##################### //
    // ALUMINUM AND GAS BODY //
    // ##################### //
    auto aluminumBox = new G4Box("Aluminum Box", dAluminum.x / 2.0, dAluminum.y / 2.0, dAluminumThickness / 2.0);
    auto aluminumLog = new G4LogicalVolume(aluminumBox, mAluminum, "AluminumLog");

    auto outerGasBox = new G4Box("Gas Box (Outside)",
        dAluminum.x / 2.0 - 1.0 * cm,
        dAluminum.y / 2.0 - 1.0 * cm,
        dAluminumThickness / 2.0 - 2.0 * mm);
    auto outerGasLog = new G4LogicalVolume(outerGasBox, mR134a, "GasLog(Outside)");

    auto innerGasBox = new G4Box("Gas Box (Inside)",
        dAcrylic.x / 2.0 - 0.5 * cm,
        dAcrylic.y / 2.0 - 0.5 * cm,
        dAcrylicThickness / 2.0 - 2.0 * mm);
    auto innerGasLog = new G4LogicalVolume(innerGasBox, mR134a, "GasLog(Inside)");

    // ############ //
    // ACRYLIC BODY //
    // ############ //

    auto acrylicBox = new G4Box("Acrylic Box",
        dAcrylic.x / 2.0, dAcrylic.y / 2.0, dAcrylicThickness / 2.0);
    auto acrylicLog = new G4LogicalVolume(acrylicBox, mAcrylic, "AcrylicLog");

    // ############### //
    // COPPER DETECTOR //
    // ############### //

    auto fibreGlassBox = new G4Box("FibreGlass Box",
        dAcrylic.x / 2.0, dAcrylic.y / 2.0, dGlassFiberThickness / 2.0);
    auto fibreGlassLog = new G4LogicalVolume(fibreGlassBox, mFibreGlass, "FibreGlassLog");

    // Copper layer
    auto copperBox = new G4Box("Copper Box",
        dAcrylic.x / 2.0, dAcrylic.y / 2.0, dCopperThickness / 2.0);
    auto copperLog = new G4LogicalVolume(copperBox, mCopper, "CopperLog");

    auto padBox = new G4Box("Pad Box",
        dPad.x / 2.0, dPad.y / 2.0, dCopperThickness / 2.0);
    logicPad = new G4LogicalVolume(padBox, mCopper, "PadLog");

    G4double padZ = acrylicBox->GetZHalfLength() + fibreGlassBox->GetZHalfLength()*2.0 + copperBox->GetZHalfLength() * 2.5;
    G4double offset = 0.5 * cm;
    for (int i = 0; i < 8; i++) {
        G4double padX = -copperBox->GetXHalfLength() + offset + padBox->GetXHalfLength();
        padX += i * (dPad.x + offset);
        for (int j = 0; j < 8; j++) {
            G4double padY = -copperBox->GetYHalfLength() + offset + padBox->GetYHalfLength();
            padY += j * (dPad.y + offset);
            auto physicalPad = new G4PVPlacement(
            nullptr,
            G4ThreeVector(padX, padY, padZ),
            logicPad,
            "Pad" + std::to_string(i * 8 + j + 1),
            outerGasLog,
            false,
            0,
            true
        );
        }
    }

    // ############ //
    // GLASS BODIES //
    // ############ //

    auto glassBox = new G4Box("Glass Box",
        dDetector.x / 2.0, dDetector.y / 2.0, dGlassThickness / 2.0);
    auto glassLog = new G4LogicalVolume(glassBox, mGlass, "GlassLog");

    G4VPhysicalVolume* aluminumPhysical = new G4PVPlacement(
        nullptr,
            G4ThreeVector(0, 0, 0),
            aluminumLog,
            "Aluminum",
            worldLog,
            false,
            0
    );

    G4VPhysicalVolume* gasPhysical = new G4PVPlacement(
        nullptr,
            G4ThreeVector(0, 0, 0),
            outerGasLog,
            "R134a",
            aluminumLog,
            false,
            0
    );

    G4VPhysicalVolume* acrylicPhysical = new G4PVPlacement(nullptr,
        G4ThreeVector(0, 0, 0),
        acrylicLog,
        "Acrylic",
        outerGasLog,
        false,
        0
    );

    G4VPhysicalVolume* fibreGlassPhysical = new G4PVPlacement(nullptr,
        G4ThreeVector(0, 0, acrylicBox->GetZHalfLength() + fibreGlassBox->GetZHalfLength()),
        fibreGlassLog,
        "Fibreglass",
        outerGasLog,
        false,
        0
    );

    G4VPhysicalVolume* copperPhysical = new G4PVPlacement(nullptr,
        G4ThreeVector(0, 0, acrylicBox->GetZHalfLength() + fibreGlassBox->GetZHalfLength()*2.0 + copperBox->GetZHalfLength()),
        copperLog,
        "CopperLayer",
        outerGasLog,
        false,
        0
    );

    G4VPhysicalVolume* innerGasPhysical = new G4PVPlacement(nullptr,
        G4ThreeVector(0, 0, 0),
        innerGasLog,
        "Gas(Inside)",
        acrylicLog,
        false,
        0
    );


    return worldPhysical;
}

/**
 * Creates the detector geometry of CERN's iRPC prototype.
 * @return the physical volume representing the whole iRPC.
 */
G4VPhysicalVolume* DetectorConstruction::ConstructiRPCPrototype() {
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

/**
 * Constructing pads and field.
 */
void DetectorConstruction::ConstructSDandField() {
    SensitiveDetector *pad_detector = new SensitiveDetector("PadDetector");
    logicPad->SetSensitiveDetector(pad_detector);
    G4SDManager::GetSDMpointer()->AddNewDetector(pad_detector);
}


