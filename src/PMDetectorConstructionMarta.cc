#include "PMDetectorConstructionMarta.hh"

#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Sphere.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4SDManager.hh"

#include "PMSensitiveDetector.hh"
#include "PMHeedModel.hh"

using namespace RPCGeometry;

PMDetectorConstructionMarta::PMDetectorConstructionMarta(PMMainParameters *params) {
    this->params = params;
}

PMDetectorConstructionMarta::~PMDetectorConstructionMarta() {
}

G4VPhysicalVolume *PMDetectorConstructionMarta::Construct() {
    G4bool checkOverlaps = true;
    G4NistManager *nist  = G4NistManager::Instance();

    G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
    G4double rWorld = 1.5 * m;
    G4Sphere* solidWorld = new G4Sphere("World", 0., rWorld, 0., 2 * CLHEP::pi, 0., CLHEP::pi);

    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "physWorld", 0, false, 0, checkOverlaps);
    G4VisAttributes* worldVis = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
    logicWorld->SetVisAttributes(worldVis);

    G4Material* padMat = nist->FindOrBuildMaterial("G4_Cu");
    G4Material* borderMat = nist->FindOrBuildMaterial("G4_Cu");

    G4double xPad = 14.0 * cm;
    G4double yPad = 0.5 * cm;
    G4double zPad = 18.0 * cm;
    G4double borderThickness = 1.0 * cm;
    G4double cornerSize = 1.0 * cm;

    G4Box* solidPad = new G4Box("Pad", 0.5 * xPad, 0.5 * yPad, 0.5 * zPad);
    logicPad = new G4LogicalVolume(solidPad, padMat, "logicPad");
    G4VisAttributes* PadVis = new G4VisAttributes(G4Colour(1.0, 0.5, 0.0));
    logicPad->SetVisAttributes(PadVis);
    G4VisAttributes* BorderVis = new G4VisAttributes(G4Colour(0, 1, 0));

    G4Box* solidBorderX = new G4Box("BorderX", 0.5 * borderThickness, 0.5 * yPad, 0.5 * zPad);
    G4LogicalVolume* logicBorderX = new G4LogicalVolume(solidBorderX, borderMat, "logicBorderX");
    logicBorderX->SetVisAttributes(BorderVis);

    G4Box* solidBorderZ = new G4Box("BorderY", 0.5 * xPad, 0.5 * borderThickness, 0.5 * zPad);
    G4LogicalVolume* logicBorderZ = new G4LogicalVolume(solidBorderZ, borderMat, "logicBorderZ");
    logicBorderZ->SetVisAttributes(BorderVis);

    G4Box* solidCorner = new G4Box("Corner", 0.5 * cornerSize, 0.5 * cornerSize, 0.5 * zPad);
    G4LogicalVolume* logicCorner = new G4LogicalVolume(solidCorner, borderMat, "logicCorner");
    logicCorner->SetVisAttributes(BorderVis);

    G4double offsetX = -3.5 * (xPad + borderThickness);
    G4double offsetZ = -3.5 * (zPad + borderThickness);

    new G4PVPlacement(0, G4ThreeVector(offsetX, 0, offsetZ), logicPad, "physPad", logicWorld, false, 1, checkOverlaps);
    new G4PVPlacement(0, G4ThreeVector(offsetX - 0.5 * (xPad + borderThickness), 0, offsetZ), logicBorderX, "physBorderLeft", logicWorld, false, 100, checkOverlaps);
    new G4PVPlacement(0, G4ThreeVector(offsetX + 0.5 * (xPad + borderThickness), 0, offsetZ), logicBorderX, "physBorderRight", logicWorld, false, 101, checkOverlaps);
    new G4PVPlacement(0, G4ThreeVector(offsetX, 0, offsetZ - 0.5 * (zPad + borderThickness)), logicBorderZ, "physBorderBottom", logicWorld, false, 102, checkOverlaps);
    new G4PVPlacement(0, G4ThreeVector(offsetX, 0, offsetZ + 0.5 * (zPad + borderThickness)), logicBorderZ, "physBorderTop", logicWorld, false, 103, checkOverlaps);

    new G4PVPlacement(0, G4ThreeVector(offsetX - 0.5 * (xPad + borderThickness), 0, offsetZ - 0.5 * (zPad + borderThickness)), logicCorner, "physCornerBL", logicWorld, false, 104, checkOverlaps);
    new G4PVPlacement(0, G4ThreeVector(offsetX + 0.5 * (xPad + borderThickness), 0, offsetZ - 0.5 * (zPad + borderThickness)), logicCorner, "physCornerBR", logicWorld, false, 105, checkOverlaps);
    new G4PVPlacement(0, G4ThreeVector(offsetX - 0.5 * (xPad + borderThickness), 0, offsetZ + 0.5 * (zPad + borderThickness)), logicCorner, "physCornerTL", logicWorld, false, 106, checkOverlaps);
    new G4PVPlacement(0, G4ThreeVector(offsetX + 0.5 * (xPad + borderThickness), 0, offsetZ + 0.5 * (zPad + borderThickness)), logicCorner, "physCornerTR", logicWorld, false, 107, checkOverlaps);

    for (int i = 1; i < 8; i++)
    {
        G4double posZ = offsetZ + 0.5 * ((2*i) * zPad + (2*i) * borderThickness);
        new G4PVPlacement(0, G4ThreeVector(offsetX, 0, posZ), logicPad, "physPad", logicWorld, false, i+1, checkOverlaps);
        new G4PVPlacement(0, G4ThreeVector(offsetX - 0.5 * (xPad + borderThickness), 0, posZ + 0.5 * (zPad + borderThickness)), logicCorner, "physCornerTL", logicWorld, false, 6, checkOverlaps);
        new G4PVPlacement(0, G4ThreeVector(offsetX + 0.5 * (xPad + borderThickness), 0, posZ + 0.5 * (zPad + borderThickness)), logicCorner, "physCornerTR", logicWorld, false, 7, checkOverlaps);
        new G4PVPlacement(0, G4ThreeVector(offsetX - 0.5 * (xPad + borderThickness), 0, posZ), logicBorderX, "physBorderLeft", logicWorld, false, 0, checkOverlaps);
        new G4PVPlacement(0, G4ThreeVector(offsetX + 0.5 * (xPad + borderThickness), 0, posZ), logicBorderX, "physBorderRight", logicWorld, false, 1, checkOverlaps);
        new G4PVPlacement(0, G4ThreeVector(offsetX, 0, posZ + 0.5 * (zPad + borderThickness)), logicBorderZ, "physBorderTop", logicWorld, false, 3, checkOverlaps);
    }

    for (int j = 1; j < 8; j++)
    {
        G4double posX = offsetX + 0.5 * ((2*j) * xPad + (2*j) * borderThickness);
        new G4PVPlacement(0, G4ThreeVector(posX, 0, offsetZ), logicPad, "physPad", logicWorld, false, 8*j + 1, checkOverlaps);
        new G4PVPlacement(0, G4ThreeVector(posX, 0, offsetZ + 0.5 * (zPad + borderThickness)), logicBorderZ, "physBorderTop", logicWorld, false, 3, checkOverlaps);
        new G4PVPlacement(0, G4ThreeVector(posX + 0.5 * (xPad + borderThickness), 0, offsetZ), logicBorderX, "physBorderLeft", logicWorld, false, 0, checkOverlaps);
        new G4PVPlacement(0, G4ThreeVector(posX, 0,  offsetZ - 0.5 * (yPad + borderThickness)), logicBorderZ, "physBorderBottom", logicWorld, false, 2, checkOverlaps);
        new G4PVPlacement(0, G4ThreeVector(posX + 0.5 * (xPad + borderThickness), 0, offsetZ - 0.5 * (zPad + borderThickness)), logicCorner, "physCornerBR", logicWorld, false, 5, checkOverlaps);
        new G4PVPlacement(0, G4ThreeVector(posX + 0.5 * (xPad + borderThickness), 0, offsetZ + 0.5 * (zPad + borderThickness)), logicCorner, "physCornerBR", logicWorld, false, 5, checkOverlaps);

        for (int i = 0; i < 7; i++)
        {
            G4double posZ = offsetZ + 0.5 * ((2*i + 2) * zPad + (2*i + 2) * borderThickness);
            new G4PVPlacement(0, G4ThreeVector(posX, 0, posZ), logicPad, "physPad", logicWorld, false, 8*j + 2 + i, checkOverlaps);
            new G4PVPlacement(0, G4ThreeVector(posX, 0, posZ + 0.5 * (zPad + borderThickness)), logicBorderZ, "physBorderTop", logicWorld, false, 3, checkOverlaps);
            new G4PVPlacement(0, G4ThreeVector(posX + 0.5 * (xPad + borderThickness), 0, posZ), logicBorderX, "physBorderLeft", logicWorld, false, 0, checkOverlaps);
            new G4PVPlacement(0, G4ThreeVector(posX + 0.5 * (xPad + borderThickness), 0, posZ + 0.5 * (zPad + borderThickness)), logicCorner, "physCornerBR", logicWorld, false, 5, checkOverlaps);
        }
    }

    // Alumínio
    G4Material* aluminium = nist->FindOrBuildMaterial("G4_Al");
    G4double aluSizeX = 128.5 * cm / 2;
    G4double aluSizeY = 2.5 * cm / 2;
    G4double aluSizeZ = 165.0 * cm / 2;

    G4Box* solidLayer = new G4Box("AlLayer", aluSizeX, aluSizeY, aluSizeZ);
    G4LogicalVolume* logicLayer = new G4LogicalVolume(solidLayer, aluminium, "AlLayerLV");
    logicLayer->SetVisAttributes(new G4VisAttributes(G4Colour(0.7, 0.7, 0.7))); // Cinza claro

    G4ThreeVector aluPos = G4ThreeVector(0, 2.5 * cm, 0);
    new G4PVPlacement(nullptr, aluPos, logicLayer, "AlLayerPV", logicWorld, false, 0, true);

    // Acrílico
    G4Material* acrylic = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
    G4double acrylicSizeX = 125.0 * cm / 2;
    G4double acrylicSizeY = 1.0  * cm / 2;
    G4double acrylicSizeZ = 155.0 * cm / 2;

    G4Box* solidAcrylicBox = new G4Box("AcrylicBox", acrylicSizeX, acrylicSizeY, acrylicSizeZ);
    G4LogicalVolume* logicAcrylicBox = new G4LogicalVolume(solidAcrylicBox, acrylic, "AcrylicBoxLV");
    logicAcrylicBox->SetVisAttributes(new G4VisAttributes(G4Colour(0.2, 0.6, 0.9))); // Azul claro

    G4ThreeVector acrylicPos = G4ThreeVector(0, -5 * cm, 0);
    new G4PVPlacement(nullptr, acrylicPos, logicAcrylicBox, "AcrylicBoxPV", logicWorld, false, 0, true);

    // Grafite
    G4Material* graphite = nist->FindOrBuildMaterial("G4_GRAPHITE");
    G4double graphiteSizeX = 119.0 * cm / 2;
    G4double graphiteSizeY = 1.0   * cm / 2;
    G4double graphiteSizeZ = 148.5 * cm / 2;

    G4Box* solidGraphiteBox = new G4Box("GraphiteBox", graphiteSizeX, graphiteSizeY, graphiteSizeZ);
    G4LogicalVolume* logicGraphiteBox = new G4LogicalVolume(solidGraphiteBox, graphite, "GraphiteBoxLV");
    logicGraphiteBox->SetVisAttributes(new G4VisAttributes(G4Colour(0.3, 0.3, 0.3))); // Grafite escuro

    G4ThreeVector graphitePos = G4ThreeVector(0, -10 * cm, 0);
    new G4PVPlacement(nullptr, graphitePos, logicGraphiteBox, "GraphiteBoxPV", logicWorld, false, 0, true);

    // Vidro
    G4Material* glass = nist->FindOrBuildMaterial("G4_GLASS_PLATE");
    G4double glassSizeX = 120.0 * cm / 2;
    G4double glassSizeY = 0.2   * cm / 2;
    G4double glassSizeZ = 152.0 * cm / 2;

    G4Box* solidGlassBox = new G4Box("GlassBox", glassSizeX, glassSizeY, glassSizeZ);
    G4LogicalVolume* logicGlassBox = new G4LogicalVolume(solidGlassBox, glass, "GlassBoxLV");
    logicGlassBox->SetVisAttributes(new G4VisAttributes(G4Colour(0.8, 1.0, 1.0))); // Ciano bem claro

    G4ThreeVector glassPos = G4ThreeVector(0, -15 * cm, 0);
    new G4PVPlacement(nullptr, glassPos, logicGlassBox, "GlassBoxPV", logicWorld, false, 0, true);

    G4ThreeVector glassPos2 = G4ThreeVector(0, -20 * cm, 0);
    new G4PVPlacement(nullptr, glassPos2, logicGlassBox, "GlassBoxPV2", logicWorld, false, 0, true);

    G4ThreeVector glassPos3 = G4ThreeVector(0, -25 * cm, 0);
    new G4PVPlacement(nullptr, glassPos3, logicGlassBox, "GlassBoxPV3", logicWorld, false, 0, true);

    // Segunda camada de grafite
    G4ThreeVector graphitePos2 = G4ThreeVector(0, -30 * cm, 0);
    new G4PVPlacement(nullptr, graphitePos2, logicGraphiteBox, "GraphiteBoxPV2", logicWorld, false, 0, true);

    // Segunda camada de acrílico
    G4ThreeVector acrylicPos2 = G4ThreeVector(0, -35 * cm, 0);
    new G4PVPlacement(nullptr, acrylicPos2, logicAcrylicBox, "AcrylicBoxPV2", logicWorld, false, 0, true);

    // Segunda camada de aluminio
    G4ThreeVector aluPos2 = G4ThreeVector(0, -40 * cm, 0);
    new G4PVPlacement(nullptr, aluPos2, logicLayer, "AlLayerPV", logicWorld, false, 0, true);

    auto garfieldBox = new G4Box("GarfieldBox",
        0.5 * GARFIELD_VOLUME_X_MARTA, 0.5 * GARFIELD_VOLUME_Y_MARTA, 0.5 * GARFIELD_VOLUME_Z_MARTA
    );
    garfieldVolume = new G4LogicalVolume(garfieldBox, worldMat, "GarfieldBoxLV");
    garfieldEnvelope = new G4Region("GasRegion");
    garfieldEnvelope->AddRootLogicalVolume(garfieldVolume);

    new G4PVPlacement(nullptr, G4ThreeVector(0.0, 10.0 * cm, 0.0),
        garfieldVolume, "GarfieldBoxPV",
        logicWorld, false, 0, true);

    return physWorld;
}

void PMDetectorConstructionMarta::ConstructSDandField() {
    auto *sensDet = new PMSensitiveDetector("SensitiveDetector");
    logicPad->SetSensitiveDetector(sensDet);
    G4SDManager::GetSDMpointer()->AddNewDetector(sensDet);

    // Inicializando nosso modelo do Garfield++.
    auto* heedModel = new GarfieldInterface::PMHeedModel(garfieldEnvelope);
    heedModel->InitialiseGarfieldMarta(
        GARFIELD_VOLUME_X_MARTA / 2.0, GARFIELD_VOLUME_Y_MARTA / 2.0, GARFIELD_VOLUME_Z_MARTA / 2.0,
        params->GetGasVoltage(), params->GetGasFile(),
        PAD_X_MARTA, PAD_Z_MARTA
    );
}

