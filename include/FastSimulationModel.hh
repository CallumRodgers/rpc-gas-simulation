#ifndef FastSimulationModel_h
#define FastSimulationModel_h 

#include "G4VFastSimulationModel.hh"
#include "Physics.hh"

class G4VPhysicalVolume;

class FastSimulationModel : public G4VFastSimulationModel {
 public:
  FastSimulationModel(G4String, G4Region*);
  FastSimulationModel(G4String);
  ~FastSimulationModel();

  void SetPhysics(GarfieldPhysics* fGarfieldPhysics);
  void WriteGeometryToGDML(G4VPhysicalVolume* physicalVolume);

  virtual G4bool IsApplicable(const G4ParticleDefinition&);
  virtual G4bool ModelTrigger(const G4FastTrack&);
  virtual void DoIt(const G4FastTrack&, G4FastStep&);

 private:
  GarfieldPhysics* fGarfieldPhysics;
};

#endif
