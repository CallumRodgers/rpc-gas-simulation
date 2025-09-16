#ifndef DetectorConstruction_h
#define DetectorConstruction_h 

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4Region; 
class GarfieldG4FastSimulationModel;
class GarfieldMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();
    virtual G4VPhysicalVolume *Construct() override;
    virtual void ConstructSDandField() override;

private:
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
    
    G4LogicalVolume* logicPad = nullptr;
    G4Material* fPadMaterial = nullptr;
    G4Material* fBorderMaterial = nullptr;
    G4Material* fGasMaterial = nullptr; 
    G4bool fCheckOverlaps = true; 
    
    
    G4Region* fGasRegion = nullptr;
    GarfieldG4FastSimulationModel* fGarfieldG4FastSimulationModel = nullptr;
    GarfieldMessenger* fGarfieldMessenger = nullptr;
};

#endif