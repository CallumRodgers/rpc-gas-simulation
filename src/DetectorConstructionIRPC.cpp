#include "DetectorConstructionIRPC.hpp"

#include <G4PVPlacement.hh>

#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4Sphere.hh"

#include "HeedModel.hpp"

using namespace RPC::Geometry;
using CLHEP::pi;

G4ThreeVector addY(const G4ThreeVector& vec, const G4double y) {
    return {vec.x(), vec.y() + y, vec.z()};
}

DetectorConstructionIRPC::DetectorConstructionIRPC(MainParameters *params) {
    this->params = params;
}

DetectorConstructionIRPC::~DetectorConstructionIRPC() {
}

G4VPhysicalVolume* DetectorConstructionIRPC::Construct() {
    // ------------------------------------------------------------------------
    // Materials
    G4NistManager *nist = G4NistManager::Instance();

    auto *matWorld = nist->FindOrBuildMaterial("G4_AIR");
    auto *matAluminum = nist->FindOrBuildMaterial("G4_Al");
    auto *matCopper = nist->FindOrBuildMaterial("G4_Cu");
    auto *matMylar = nist->FindOrBuildMaterial("G4_MYLAR");
    auto *matBakelite = nist->FindOrBuildMaterial("G4_BAKELITE");
    auto *matPVC = nist->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE");

    // ------------------------------------------------------------------------
    // Dimensions
    G4double worldRadius = 1.5 * m;
    G4double gasGap = 1.4 * mm;

    G4double bottomFrameX = 0.58 * m;
    G4double bottomFrameY = 5.0 * mm;
    G4double bottomFrameZ = 0.58 * m;

    G4double mylarFoilX = 0.51 * m;
    G4double mylarFoilY = 125 * um;
    G4double mylarFoilZ = 0.51 * m;

    G4double bottomCopperFoilX = 0.66 * m;
    G4double bottomCopperFoilY = 60.0 * um;
    G4double bottomCopperFoilZ = 0.69 * m;

    G4double bakeliteX = 0.5 * m;
    G4double bakeliteY = 1.4 * mm;
    G4double bakeliteZ = 0.5 * m;

    G4double pvcFoilX = 0.5 * m;
    G4double pvcFoilY = 1.0 * mm; // Estimativa!
    G4double pvcFoilZ = 0.54 * m;

    G4double topCopperFoilX = 0.5 * m;
    G4double topCopperFoilY = 60.0 * um;
    G4double topCopperFoilZ = 0.53 * m;

    G4double topFrameX = 0.58 * m;
    G4double topFrameY = 5.0 * mm;
    G4double topFrameZ = 0.54 * m;

    // ------------------------------------------------------------------------
    // Solids and Logical volumes

    // World
    G4Sphere *worldSphere = new G4Sphere("WorldSphere", 0.0, worldRadius, 0.0, 2.0 * pi, 0.0, pi);
    G4LogicalVolume *worldLog = new G4LogicalVolume(worldSphere, matWorld, "WorldLog");

    // RPC
    G4Box *bottomFrameBox = new G4Box("BottomFrameBox", bottomFrameX / 2.0, bottomFrameY / 2.0, bottomFrameZ / 2.0);
    G4Box *mylarFoilBox = new G4Box("MylarFoilBox", mylarFoilX / 2.0, mylarFoilY / 2.0, mylarFoilZ / 2.0);
    G4Box *bottomCopperBox = new G4Box("BottomCopperBox", bottomCopperFoilX / 2.0, bottomCopperFoilY / 2.0, bottomCopperFoilZ / 2.0);
    G4Box *bakeliteBox = new G4Box("BakeliteBox", bakeliteX / 2.0, bakeliteY / 2.0, bakeliteZ / 2.0);
    G4Box *pvcFoilBox = new G4Box("PVCBox", pvcFoilX / 2.0, pvcFoilY / 2.0, pvcFoilZ / 2.0);
    G4Box *topCopperBox = new G4Box("TopCopperBox", topCopperFoilX / 2.0, topCopperFoilY / 2.0, topCopperFoilZ / 2.0);
    G4Box *topFrameBox = new G4Box("TopFrameBox", topFrameX / 2.0, topFrameY / 2.0, topFrameZ / 2.0);

    G4LogicalVolume *bottomFrameLog = new G4LogicalVolume(bottomFrameBox, matAluminum, "BottomFrameLog");
    G4LogicalVolume *mylarFoilLog = new G4LogicalVolume(mylarFoilBox, matMylar, "MylarFoilLog");
    G4LogicalVolume *bottomCopperLog = new G4LogicalVolume(bottomCopperBox, matCopper, "BottomCopperLog");
    G4LogicalVolume *bakeliteLog = new G4LogicalVolume(bakeliteBox, matBakelite, "BakeliteLog");
    G4LogicalVolume *pvcFoilLog = new G4LogicalVolume(pvcFoilBox, matPVC, "PVCLog");
    G4LogicalVolume *topCopperLog = new G4LogicalVolume(topCopperBox, matCopper, "TopCopperLog");
    G4LogicalVolume *topFrameLog = new G4LogicalVolume(topFrameBox, matAluminum, "TopFrameLog");

    // ------------------------------------------------------------------------
    // Placements

    // World

    G4VPhysicalVolume *world = new G4PVPlacement(nullptr, G4ThreeVector(),
        worldLog, "World", nullptr,
        false, 0
    );

    // RPC

    G4ThreeVector detectorOrigin = G4ThreeVector(0, 0, 0);

    G4double bottomFramePos = 0.0 * mm;
    G4double mylar1Pos = bottomFramePos + bottomFrameY / 2.0 + mylarFoilY / 2.0;
    G4double bottomCopperPos = mylar1Pos + mylarFoilY / 2.0 + bottomCopperFoilY / 2.0;
    G4double mylar2Pos = bottomCopperPos + bottomCopperFoilY / 2.0 + mylarFoilY / 2.0;
    G4double bottomGapBakelite1Pos = mylar2Pos + mylarFoilY / 2.0 + bakeliteY / 2.0;
    G4double bottomGapBakelite2Pos = bottomGapBakelite1Pos + 2 * gasGap;
    G4double pvcFoilPos = bottomGapBakelite2Pos + bakeliteY / 2.0 + pvcFoilY / 2.0;
    G4double topGapBakelite1Pos = pvcFoilPos + pvcFoilY / 2.0 + bakeliteY / 2.0;
    G4double topGapBakelite2Pos = topGapBakelite1Pos + 2 * gasGap;
    G4double mylar3Pos = topGapBakelite2Pos + bakeliteY / 2.0 + mylarFoilY / 2.0;
    G4double topCopperPos = mylar3Pos + mylarFoilY / 2.0 + topCopperFoilY / 2.0;
    G4double topFramePos = topCopperPos + topCopperFoilY / 2.0 + topFrameY / 2.0;

    new G4PVPlacement(nullptr, addY(detectorOrigin, bottomFramePos),
        bottomFrameLog, "BottomFrame", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, mylar1Pos),
        mylarFoilLog, "Mylar1", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, bottomCopperPos),
        bottomCopperLog, "BottomCopperFoil", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, mylar2Pos),
        mylarFoilLog, "Mylar2", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, bottomGapBakelite1Pos),
        bakeliteLog, "BottomGapBakelite1", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, bottomGapBakelite2Pos),
        bakeliteLog, "BottomGapBakelite2", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, pvcFoilPos),
        pvcFoilLog, "PVCFoil", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, topGapBakelite1Pos),
        bakeliteLog, "TopGapBakelite1", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, topGapBakelite2Pos),
        bakeliteLog, "TopGapBakelite2", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, mylar3Pos),
        mylarFoilLog, "Mylar3", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, topCopperPos),
        topCopperLog, "TopCopperFoil", worldLog,
        false, 0
    );
    new G4PVPlacement(nullptr, addY(detectorOrigin, topFramePos),
        topFrameLog, "TopFrame", worldLog,
        false, 0
    );

    // ------------------------------------------------------------------------
    // Garfield Volume

    G4double garfieldPos = mylar2Pos + GARFIELD_VOLUME_Y_IRPC / 2.0;

    auto garfieldBox = new G4Box("GarfieldBox",
        0.5 * GARFIELD_VOLUME_X_IRPC, 0.5 * GARFIELD_VOLUME_Y_IRPC, 0.5 * GARFIELD_VOLUME_Z_IRPC
    );
    auto *garfieldVolume = new G4LogicalVolume(garfieldBox, matWorld, "GarfieldBoxLV");
    garfieldEnvelope = new G4Region("GasRegion");
    garfieldEnvelope->AddRootLogicalVolume(garfieldVolume);

    new G4PVPlacement(nullptr, addY(detectorOrigin, garfieldPos),
        garfieldVolume, "GarfieldRegion", worldLog,
        false, 0
    );

    return world;
}

void DetectorConstructionIRPC::ConstructSDandField() {
    // Inicializando nosso modelo do Garfield++.
    new GarfieldInterface::HeedModel(
        garfieldEnvelope,
        GarfieldInterface::DetectorType::iRPC,
        {
            .gasFile = params->GetGasFile(),
            .voltage = params->GetGasVoltage()
    });
}
