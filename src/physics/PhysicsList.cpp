//
// Created by callum on 26/11/24.
//

#include "physics/PhysicsList.h"

// ###################### //
// Particle construction. //
// ###################### //

// Lepton headers.
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4TauPlus.hh"
#include "G4TauMinus.hh"
#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4AntiNeutrinoTau.hh"

// Boson headers.
#include "G4Gamma.hh"
#include "G4OpticalPhoton.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

// Baryon headers.
#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "G4AntiProton.hh"
#include "G4AntiNeutron.hh"

// Meson headers.
#include "G4AntiKaonZero.hh"
#include "G4Eta.hh"
#include "G4EtaPrime.hh"
#include "G4Etac.hh"
#include "G4KaonMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonZero.hh"
#include "G4KaonZeroLong.hh"
#include "G4KaonZeroShort.hh"
#include "G4PionMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionZero.hh"

void PhysicsList::ConstructParticle() {
    // Constructing all particles to be simulated in the experiment.
    // It includes both primary particles and particles generated from them (decays, etc.)
    ConstructLeptons();
    ConstructBosons();
    ConstructBaryons();
    ConstructMesons();
}

void PhysicsList::ConstructLeptons() {
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    G4MuonPlus::MuonPlusDefinition();
    G4MuonMinus::MuonMinusDefinition();
    G4TauPlus::TauPlusDefinition();
    G4TauMinus::TauMinusDefinition();

    // Neutrinos.
    G4NeutrinoE::NeutrinoEDefinition();
    G4NeutrinoMu::NeutrinoMuDefinition();
    G4NeutrinoTau::NeutrinoTauDefinition();
    G4AntiNeutrinoE::AntiNeutrinoEDefinition();
    G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
    G4AntiNeutrinoTau::AntiNeutrinoTauDefinition();
}

void PhysicsList::ConstructBosons() {
    G4Gamma::GammaDefinition();
    G4Geantino::GeantinoDefinition();
    G4ChargedGeantino::ChargedGeantinoDefinition();
    G4OpticalPhoton::OpticalPhotonDefinition();
}

void PhysicsList::ConstructBaryons() {
    G4Proton::ProtonDefinition();
    G4Neutron::NeutronDefinition();
    G4AntiProton::AntiProtonDefinition();
    G4AntiNeutron::AntiNeutronDefinition();
}

void PhysicsList::ConstructMesons() {
    G4PionPlus::PionPlusDefinition();
    G4PionMinus::PionMinusDefinition();
    G4PionZero::PionZeroDefinition();
    G4KaonPlus::KaonPlusDefinition();
    G4KaonMinus::KaonMinusDefinition();
    G4KaonZero::KaonZeroDefinition();
    G4KaonZeroShort::KaonZeroShortDefinition();
    G4KaonZeroLong::KaonZeroLongDefinition();
    G4AntiKaonZero::AntiKaonZeroDefinition();
    G4Eta::EtaDefinition();
    G4Etac::EtacDefinition();
    G4EtaPrime::EtaPrimeDefinition();
}

// ############################### //
// Physics processes construction. //
// ############################### //

#include "G4ProcessManager.hh"

// Gamma processes.
#include "G4PhotoElectricEffect.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4GammaGeneralProcess.hh"
#include "G4RayleighScattering.hh"

// Electron-positron processes.
#include "G4eIonisation.hh"
#include "G4ePairProduction.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "G4AnnihiToMuPair.hh"
#include "G4TransportationWithMsc.hh"

// Muon processes.
#include "G4MuIonisation.hh"
#include "G4MuPairProduction.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuonToMuonPairProduction.hh"
#include "G4MuMultipleScattering.hh"

// Hadronic/ion processes.
#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4hPairProduction.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hMultipleScattering.hh"


void PhysicsList::ConstructProcess() {
    // Adding transportation lets our particles travel through space and time.
    AddTransportation();
    // Constructing electromagnetic interactions.
    ConstructEM();
}

void PhysicsList::ConstructEM() {
    // Particle iterator will allow us to loop through all defined particles,
    // making it easier to define the physics processes.
    auto particleIterator = GetParticleIterator();
    particleIterator->reset();

    while ((*particleIterator)()) {
    	G4ParticleDefinition *particle = particleIterator->value();
        G4ProcessManager *processManager = particle->GetProcessManager();
        G4String particleName = particle->GetParticleName();
        if (particleName == "gamma") {
            // Photon.
            processManager->AddDiscreteProcess(new G4PhotoElectricEffect);
            processManager->AddDiscreteProcess(new G4ComptonScattering);
            processManager->AddDiscreteProcess(new G4GammaConversion);
            processManager->AddDiscreteProcess(new G4GammaConversionToMuons);
            processManager->AddDiscreteProcess(new G4GammaGeneralProcess);
            processManager->AddDiscreteProcess(new G4RayleighScattering);
        } else if (particleName == "e-") {
            // Electron.
            processManager->AddProcess(new G4TransportationWithMsc, -1, 1, 1);
            processManager->AddProcess(new G4eIonisation, -1, 2, 2);
            processManager->AddProcess(new G4eBremsstrahlung, -1, 3, 3);
        } else if (particleName == "e+") {
            // Positron.
            processManager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
            processManager->AddProcess(new G4eIonisation, -1, 2, 2);
            processManager->AddProcess(new G4eBremsstrahlung, -1, 3, 3);
            processManager->AddProcess(new G4eplusAnnihilation, 0, -1, 4);
        } else if (particleName == "mu+" || particleName == "mu-") {
            // Either muon.
            processManager->AddProcess(new G4MuMultipleScattering, -1, 1, 1);
            processManager->AddProcess(new G4MuIonisation, -1, 2, 2);
            processManager->AddProcess(new G4MuBremsstrahlung, -1, 3, 3);
            processManager->AddProcess(new G4MuPairProduction, -1, 4, 4);
        } else if ((!particle->IsShortLived()) &&
                   (particle->GetPDGCharge() != 0.0) &&
                   (particle->GetParticleName() != "chargedgeantino")) {
            // all others charged particles except geantino
            processManager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
            processManager->AddProcess(new G4hIonisation, -1, 2, 2);
        }
    }
}
