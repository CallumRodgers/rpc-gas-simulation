
#ifndef DETECTORCONSTRUCTION_H
#define DETECTORCONSTRUCTION_H

#include "G4VUserDetectorConstruction.hh"

class DetectorConstruction : public G4VUserDetectorConstruction {
    public:
        static constexpr int TYPE_GLASS = 0;
        static constexpr int TYPE_iRPC_PROTOTYPE = 1;

        explicit DetectorConstruction(int type);
        ~DetectorConstruction() override = default;
        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;
        G4LogicalVolume* logicPad;
        G4LogicalVolume* logicGasBox;

    private:
        G4VPhysicalVolume* ConstructGlassRPC();
        G4VPhysicalVolume* ConstructiRPCPrototype();
};

#endif //DETECTORCONSTRUCTION_H
