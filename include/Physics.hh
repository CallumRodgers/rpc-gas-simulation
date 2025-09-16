#ifndef Physics_h
#define Physics_h

#include <iostream>
#include <map>
#include <vector>
#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/TrackHeed.hh"
#include "G4ThreeVector.hh" 

using EnergyRange_MeV = std::pair<double, double>;
using MapParticlesEnergy = std::map<std::string, EnergyRange_MeV>;

class GarfieldParticle {
 public:
  GarfieldParticle(std::string particleName, double ekin_eV, double time,
                   double x_cm, double y_cm, double z_cm, double dx, double dy,
                   double dz)
      : fParticleName(particleName),
        fEkin_MeV(ekin_eV / 1000000),
        fTime(time),
        fx_mm(10 * x_cm),
        fy_mm(10 * y_cm),
        fz_mm(10 * z_cm),
        fdx(dx),
        fdy(dy),
        fdz(dz) {}
  ~GarfieldParticle() {}

  std::string getParticleName() const { return fParticleName; }
  double getX_mm() const { return fx_mm; }
  double getY_mm() const { return fy_mm; }
  double getZ_mm() const { return fz_mm; }
  double getEkin_MeV() const { return fEkin_MeV; }
  double getTime() const { return fTime; }
  double getDX() const { return fdx; }
  double getDY() const { return fdy; }
  double getDZ() const { return fdz; }

 private:
  std::string fParticleName;
  double fEkin_MeV, fTime, fx_mm, fy_mm, fz_mm, fdx, fdy, fdz;
};

class GarfieldPhysics {
 public:
  static GarfieldPhysics* GetInstance();
  static void Dispose();
  double GetEnergyDeposit_MeV();
  void InitializePhysics();

  void DoIt(std::string particleName, double ekin_MeV, double time, double x_cm,
            double y_cm, double z_cm, double dx, double dy, double dz);

  void AddParticleName(const std::string particleName, double ekin_min_MeV,
                       double ekin_max_MeV, std::string program);
  bool FindParticleName(const std::string name,
                        std::string program = "garfield");
  bool FindParticleNameEnergy(std::string name, double ekin_MeV,
                              std::string program = "garfield");
  double GetMinEnergyMeVParticle(std::string name,
                                 std::string program = "garfield");
  double GetMaxEnergyMeVParticle(std::string name,
                                 std::string program = "garfield");
  void SetIonizationModel(std::string model, bool useDefaults = true);
  std::string GetIonizationModel();
  const std::vector<GarfieldParticle>& GetSecondaryParticles() const {
    return fSecondaryParticles;
  }
  void EnableCreateSecondariesInGeant4(bool flag) {
    createSecondariesInGeant4 = flag;
  }
  bool GetCreateSecondariesInGeant4() const { 
    return createSecondariesInGeant4; 
  }
  double GetEnergyDeposit_MeV() const { return fEnergyDeposit / 1000000; }
  double GetAvalancheSize() const { return fAvalancheSize; }
  double GetGain() const { return fGain; }
  void Clear() {
    fEnergyDeposit = 0;
    fAvalancheSize = 0;
    fGain = 0;
    nsum = 0;
  }
  const std::vector<std::pair<G4ThreeVector, G4ThreeVector>>& GetDriftLines() const { return fDriftLines; }

  static constexpr double kGap   = 0.2;   // [cm]
  static constexpr double kHalfX = 5.0;   // [cm]
  static constexpr double kHalfY = 5.0;   // [cm]

 private:
  GarfieldPhysics() = default;
  ~GarfieldPhysics();

  std::string fIonizationModel = "Heed";

  static GarfieldPhysics* fGarfieldPhysics;

  MapParticlesEnergy fMapParticlesEnergyGeant4;
  MapParticlesEnergy fMapParticlesEnergyGarfield;
  Garfield::MediumMagboltz* fMediumMagboltz = nullptr;
  Garfield::Sensor* fSensor = nullptr;
  Garfield::TrackHeed* fTrackHeed = nullptr;
  Garfield::ComponentAnalyticField* fComponentAnalyticField = nullptr;

  std::vector<GarfieldParticle> fSecondaryParticles;
  std::vector<std::pair<G4ThreeVector, G4ThreeVector>> fDriftLines;

  bool createSecondariesInGeant4 = false;
  double fEnergyDeposit = 0.;
  double fAvalancheSize = 0.;
  double fGain = 0.;
  int nsum = 0;
};
#endif