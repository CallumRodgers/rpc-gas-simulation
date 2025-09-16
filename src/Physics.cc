#include "Physics.hh"
#include "Analysis.hh"
#include "Garfield/AvalancheMC.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "G4SystemOfUnits.hh" 

GarfieldPhysics* GarfieldPhysics::fGarfieldPhysics = nullptr;

namespace {
  constexpr double kGap   = 0.2; 
  constexpr double kHalfX = 128.5 / 2.0;
  constexpr double kHalfZ = 165.0 / 2.0;
  constexpr double kHV    = 6000.0;
  constexpr double kEy    = kHV / kGap;
}

GarfieldPhysics* GarfieldPhysics::GetInstance() {
  if (!fGarfieldPhysics) fGarfieldPhysics = new GarfieldPhysics();
  return fGarfieldPhysics;
}

void GarfieldPhysics::Dispose() {
  delete fGarfieldPhysics;
  fGarfieldPhysics = nullptr;
}

GarfieldPhysics::~GarfieldPhysics() {
  delete fMediumMagboltz;
  delete fSensor;
  delete fComponentAnalyticField;
  delete fTrackHeed;

  std::cout << "Deconstructor GarfieldPhysics" << std::endl;
}

std::string GarfieldPhysics::GetIonizationModel() { return fIonizationModel; }

void GarfieldPhysics::SetIonizationModel(std::string model, bool useDefaults) {
  if (model != "PAIPhot" && model != "PAI" && model != "Heed") {
    std::cout << "Unknown ionization model " << model << std::endl;
    std::cout << "Using PAIPhot as default model!" << std::endl;
    model = "Heed";
  }
  fIonizationModel = model;

  if (fIonizationModel == "PAIPhot" || fIonizationModel == "PAI") {
    if (useDefaults) {
      this->AddParticleName("gamma", 1e-6, 1e+8, "garfield");
      this->AddParticleName("mu-", 1e+1, 1e+8, "garfield"); 
      this->AddParticleName("e-", 0, 1e+8, "geant4");
      this->AddParticleName("e+", 0, 1e+8, "geant4");
      this->AddParticleName("mu+", 0, 1e+8, "geant4");
      this->AddParticleName("proton", 0, 1e+8, "geant4");
      this->AddParticleName("pi+", 0, 1e+8, "geant4");
      this->AddParticleName("pi-", 0, 1e+8, "geant4");
      this->AddParticleName("alpha", 0, 1e+8, "geant4");
      this->AddParticleName("He3", 0, 1e+8, "geant4");
      this->AddParticleName("GenericIon", 0, 1e+8, "geant4");
    }
  } else if (fIonizationModel == "Heed") {
    if (useDefaults) {
      this->AddParticleName("gamma", 1e-6, 1e+8, "garfield");
      this->AddParticleName("e-", 6e-2, 1e+7, "garfield");
      this->AddParticleName("e+", 6e-2, 1e+7, "garfield");
      this->AddParticleName("mu-", 1e+1, 1e+8, "garfield");
      this->AddParticleName("mu+", 1e+1, 1e+8, "garfield");
      this->AddParticleName("pi-", 2e+1, 1e+8, "garfield");
      this->AddParticleName("pi+", 2e+1, 1e+8, "garfield");
      this->AddParticleName("kaon-", 1e+1, 1e+8, "garfield");
      this->AddParticleName("kaon+", 1e+1, 1e+8, "garfield");
      this->AddParticleName("proton", 9.e+1, 1e+8, "garfield");
      this->AddParticleName("anti_proton", 9.e+1, 1e+8, "garfield");
      this->AddParticleName("deuteron", 2.e+2, 1e+8, "garfield");
      this->AddParticleName("alpha", 4.e+2, 1e+8, "garfield");
    }
  } else if (fIonizationModel == "Heed") {
    if (useDefaults) {
      this->AddParticleName("mu-", 1e+1, 1e+8, "garfield"); 
    }
  }
}

void GarfieldPhysics::AddParticleName(const std::string particleName,
                                      double ekin_min_MeV, double ekin_max_MeV,
                                      std::string program) {
  if (ekin_min_MeV >= ekin_max_MeV) {
    std::cout << "Ekin_min=" << ekin_min_MeV
              << " keV is larger than Ekin_max=" << ekin_max_MeV << " keV"
              << std::endl;
    return;
  }
  if (program == "garfield") {
    std::cout << "Garfield model (Heed) is applicable for G4Particle "
              << particleName << " between " << ekin_min_MeV << " MeV and "
              << ekin_max_MeV << " MeV" << std::endl;
    fMapParticlesEnergyGarfield.insert(std::make_pair(
        particleName, std::make_pair(ekin_min_MeV, ekin_max_MeV)));
  } else {
    std::cout << fIonizationModel << " is applicable for G4Particle "
              << particleName << " between " << ekin_min_MeV << " MeV and "
              << ekin_max_MeV << " MeV" << std::endl;
    fMapParticlesEnergyGeant4.insert(std::make_pair(
        particleName, std::make_pair(ekin_min_MeV, ekin_max_MeV)));
  }
}

bool GarfieldPhysics::FindParticleName(std::string name, std::string program) {
  if (program == "garfield") {
    auto it = fMapParticlesEnergyGarfield.find(name);
    if (it != fMapParticlesEnergyGarfield.end()) return true;
  } else {
    auto it = fMapParticlesEnergyGeant4.find(name);
    if (it != fMapParticlesEnergyGeant4.end()) return true;
  }
  return false;
}

bool GarfieldPhysics::FindParticleNameEnergy(std::string name, double ekin_MeV,
                                             std::string program) {
  if (program == "garfield") {
    auto it = fMapParticlesEnergyGarfield.find(name);
    if (it != fMapParticlesEnergyGarfield.end()) {
      EnergyRange_MeV range = it->second;
      if (range.first <= ekin_MeV && range.second >= ekin_MeV) {
        return true;
      }
    }
  } else {
    auto it = fMapParticlesEnergyGeant4.find(name);
    if (it != fMapParticlesEnergyGeant4.end()) {
      EnergyRange_MeV range = it->second;
      if (range.first <= ekin_MeV && range.second >= ekin_MeV) {
        return true;
      }
    }
  }
  return false;
}

double GarfieldPhysics::GetMinEnergyMeVParticle(std::string name,
                                                std::string program) {
  if (program == "garfield") {
    auto it = fMapParticlesEnergyGarfield.find(name);
    if (it != fMapParticlesEnergyGarfield.end()) {
      EnergyRange_MeV range = it->second;
      return range.first;
    }
  } else {
    auto it = fMapParticlesEnergyGeant4.find(name);
    if (it != fMapParticlesEnergyGeant4.end()) {
      EnergyRange_MeV range = it->second;
      return range.first;
    }
  }
  return -1;
}

double GarfieldPhysics::GetMaxEnergyMeVParticle(std::string name,
                                                std::string program) {
  if (program == "garfield") {
    auto it = fMapParticlesEnergyGarfield.find(name);
    if (it != fMapParticlesEnergyGarfield.end()) {
      EnergyRange_MeV range = it->second;
      return range.second;
    }
  } else {
    auto it = fMapParticlesEnergyGeant4.find(name);
    if (it != fMapParticlesEnergyGeant4.end()) {
      EnergyRange_MeV range = it->second;
      return range.second;
    }
  }
  return -1;
}


void GarfieldPhysics::InitializePhysics(){
    G4cout << "[LOG] GarfieldPhysics::InitializePhysics -> Initializing..." << G4endl;

    fMediumMagboltz = new Garfield::MediumMagboltz();
    fMediumMagboltz->SetComposition("ic4h10", 5., "sf6", 5., "c2h2f4", 90.);
    fMediumMagboltz->SetTemperature(293.15);
    fMediumMagboltz->SetPressure(760.);
    fMediumMagboltz->EnableDrift();
    fMediumMagboltz->Initialise(true);
    
    if (!fMediumMagboltz->LoadGasFile("rpc_gas_5_5_90.gas")) {
        G4cout << "[LOG] GarfieldPhysics::InitializePhysics -> ERRO: Arquivo .gas não encontrado!" << G4endl;
    } else {
        G4cout << "[LOG] GarfieldPhysics::InitializePhysics -> Arquivo .gas carregado com sucesso." << G4endl;
    }


    fComponentAnalyticField = new Garfield::ComponentAnalyticField();
    fComponentAnalyticField->SetMedium(fMediumMagboltz);
    fComponentAnalyticField->AddPlaneY(-0.5 * kGap,    0., "anode");
    fComponentAnalyticField->AddPlaneY(+0.5 * kGap, -kHV, "cathode");
    
    G4cout << "[LOG] GarfieldPhysics::InitializePhysics -> E-Field (Ey): " << kEy << " V/cm" << G4endl;


    fSensor = new Garfield::Sensor();
    fSensor->AddComponent(fComponentAnalyticField);
    fSensor->SetArea(-kHalfX, -0.5 * kGap, -kHalfZ,
                      kHalfX,  0.5 * kGap,  kHalfZ);
    G4cout << "[LOG] GarfieldPhysics::InitializePhysics -> Sensor Area Set." << G4endl;

    fTrackHeed = new Garfield::TrackHeed(fSensor);
    fTrackHeed->EnableDeltaElectronTransport();
    G4cout << "[LOG] GarfieldPhysics::InitializePhysics -> TrackHeed enabled. Initialization finished." << G4endl;
}

double GarfieldPhysics::GetEnergyDeposit_MeV() {
  return fEnergyDeposit / 1.e6;
}

void GarfieldPhysics::DoIt(std::string particleName, double ekin_MeV,
                           double time, double x_cm, double y_cm, double z_cm,
                           double dx, double dy, double dz) {
  G4cout << "\n[LOG] GarfieldPhysics::DoIt -> Simulating track in Garfield++" << G4endl;
  G4cout << "    -> Particle: " << particleName << " at (" << x_cm << ", " << y_cm << ", " << z_cm << ") cm" << G4endl;

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  fEnergyDeposit = 0;
  fSecondaryParticles.clear();
  fAvalancheSize = 0;
  nsum = 0;
  fDriftLines.clear(); // Limpa as linhas de drift do evento anterior

  Garfield::AvalancheMC drift(fSensor);
  drift.SetDistanceSteps(1.e-4);

  constexpr double yMin = -0.5 * kGap;
  constexpr double yMax = +0.5 * kGap;
  double eKin_eV = ekin_MeV * 1e+6;

  if (particleName == "gamma") {
    Garfield::TrackHeed::Cluster cl = fTrackHeed->TransportPhoton(x_cm, y_cm, z_cm, time, eKin_eV, dx, dy, dz);
    for (const auto& electron : cl.electrons) {
      if (electron.y < yMin || electron.y > yMax || std::abs(electron.x) > kHalfX || std::abs(electron.z) > kHalfZ) continue;
      nsum++;
      fEnergyDeposit += fTrackHeed->GetW();
      drift.DriftElectron(electron.x, electron.y, electron.z, electron.t);
    }
  } else {
    fTrackHeed->SetParticle(particleName);
    fTrackHeed->SetKineticEnergy(eKin_eV);
    fTrackHeed->NewTrack(x_cm, y_cm, z_cm, time, dx, dy, dz);

    for (const auto& cluster : fTrackHeed->GetClusters()) {
      if (cluster.y < yMin || cluster.y > yMax || std::abs(cluster.x) > kHalfX || std::abs(cluster.z) > kHalfZ) continue;
      
      nsum += cluster.electrons.size();
      fEnergyDeposit += cluster.energy;
      
      for (const auto& electron : cluster.electrons) {
        if (electron.y < yMin || electron.y > yMax || std::abs(electron.x) > kHalfX || std::abs(electron.z) > kHalfZ) continue;

        analysisManager->FillH3(1, electron.y * 10, electron.x * 10, electron.z * 10);
        drift.DriftElectron(electron.x, electron.y, electron.z, electron.t);
      }
    }
  }

  unsigned int ne = 0, ni = 0;
  drift.GetAvalancheSize(ne, ni);
  fAvalancheSize = ne;
  fGain = (nsum > 0) ? (static_cast<double>(fAvalancheSize) / nsum) : 0.0;

  // --- CORREÇÃO: Assinatura da função GetElectronEndpoint corrigida ---
  const unsigned int nEndpoints = drift.GetNumberOfElectronEndpoints();
  for (unsigned int i = 0; i < nEndpoints; ++i) {
      double x1, y1, z1, t1; // Ponto inicial
      double x2, y2, z2, t2; // Ponto final
      int status;
      // Chamada correta com 10 argumentos
      drift.GetElectronEndpoint(i, x1, y1, z1, t1, x2, y2, z2, t2, status);

      G4ThreeVector start(x1 * CLHEP::cm, y1 * CLHEP::cm, z1 * CLHEP::cm);
      G4ThreeVector end(x2 * CLHEP::cm, y2 * CLHEP::cm, z2 * CLHEP::cm);
      fDriftLines.push_back(std::make_pair(start, end));
  }
  // ------------------------------------------------------------------

  G4cout << "[LOG] GarfieldPhysics::DoIt -> Ionization electrons created (nsum): " << nsum << G4endl;
  G4cout << "[LOG] GarfieldPhysics::DoIt -> Total avalanche size: " << fAvalancheSize << G4endl;
  G4cout << "[LOG] GarfieldPhysics::DoIt -> Calculated Gain: " << fGain << G4endl;
}