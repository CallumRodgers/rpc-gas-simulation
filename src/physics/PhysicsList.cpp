
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
#include <G4ProcessManager.hh>


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

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4DecayPhysics.hh"

PhysicsList::PhysicsList() {
    // EM Physics
    RegisterPhysics(new G4EmStandardPhysics());

    // Radioactive decay physics
    RegisterPhysics(new G4RadioactiveDecayPhysics());

    // Decay physics
    RegisterPhysics(new G4DecayPhysics());
}

 PhysicsList::~PhysicsList() {}


//void PhysicsList::ConstructParticle() {
//    // Constructing all particles to be simulated in the experiment.
//    // It includes both primary particles and particles generated from them (decays, etc.)
//    ConstructLeptons();
//    ConstructBosons();
//    ConstructBaryons();
//    ConstructMesons();
//}

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

#include "G4PhysicsListHelper.hh"

// Gamma processes.
#include "G4PhotoElectricEffect.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4GammaGeneralProcess.hh"

// Electron-positron processes.
#include "G4eIonisation.hh"
#include "G4eMultipleScattering.hh"
#include "G4ePairProduction.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "G4AnnihiToMuPair.hh"
#include "G4eeToHadrons.hh"

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

// Decay.
#include "G4Decay.hh"

//void PhysicsList::ConstructProcess() {
//    // Adding transportation lets our particles travel through space and time.
//    AddTransportation();
//    // Constructing electromagnetic interactions.
//    ConstructEMAndDecay();
//}

void PhysicsList::ConstructEMAndDecay() {

    // PhysicsListHelper simplifies registering processes.
    G4PhysicsListHelper *helper = G4PhysicsListHelper::GetPhysicsListHelper();

    // Particle iterator will allow us to loop through all defined particles,
    // making it easier to define the physics processes.
    auto particleIterator = GetParticleIterator();
    particleIterator->reset();

    // Decay process.
    auto decayProcess = new G4Decay();

    while ((*particleIterator)()) {
    	G4ParticleDefinition *particle = particleIterator->value();
        G4String particleName = particle->GetParticleName();
        G4ProcessManager *pm = particle->GetProcessManager();

        // Registering decay processes.
        if (decayProcess->IsApplicable(*particle)) {
            pm->AddProcess(decayProcess);
            // Process ordering.
            pm->SetProcessOrdering(decayProcess, idxPostStep);
            pm->SetProcessOrdering(decayProcess, idxAtRest);
        }

        if (particleName == "gamma") {
            // Photon.
            helper->RegisterProcess(new G4PhotoElectricEffect, particle);
            helper->RegisterProcess(new G4ComptonScattering, particle);
            helper->RegisterProcess(new G4GammaConversion, particle);
            helper->RegisterProcess(new G4GammaConversionToMuons, particle);
            helper->RegisterProcess(new G4GammaGeneralProcess, particle);
        } else if (particleName.starts_with("e")) {
            // Either electron or positron.
            helper->RegisterProcess(new G4eIonisation, particle);
            helper->RegisterProcess(new G4eMultipleScattering, particle);
            helper->RegisterProcess(new G4ePairProduction, particle);
            helper->RegisterProcess(new G4eBremsstrahlung, particle);

            if (particleName.ends_with("+")) {
                // Positron.
                helper->RegisterProcess(new G4eplusAnnihilation, particle);
                helper->RegisterProcess(new G4AnnihiToMuPair, particle);
                helper->RegisterProcess(new G4eeToHadrons, particle);
            }
        } else if (particleName.starts_with("mu")) {
            // Either muon.
            helper->RegisterProcess(new G4MuIonisation, particle);
            helper->RegisterProcess(new G4MuMultipleScattering, particle);
            helper->RegisterProcess(new G4MuPairProduction, particle);
            helper->RegisterProcess(new G4MuBremsstrahlung, particle);
            helper->RegisterProcess(new G4MuonToMuonPairProduction, particle);
        } else if ((particle->GetPDGCharge() != 0.0) and (particleName != "chargedgeantino")) {
            // By now we're dealing with only charged ions/hadrons
            if (particle->IsGeneralIon()) {
                helper->RegisterProcess(new G4ionIonisation, particle);
            } else {
                helper->RegisterProcess(new G4hIonisation, particle);
                helper->RegisterProcess(new G4hMultipleScattering, particle);
                helper->RegisterProcess(new G4hPairProduction, particle);
                helper->RegisterProcess(new G4hBremsstrahlung, particle);
            }
        }
    }

    delete decayProcess;
}
