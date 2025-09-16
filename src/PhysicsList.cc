#include "PhysicsList.hh"
#include "G4SystemOfUnits.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4IonPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4EmStandardPhysics_option4.hh" 
#include "Physics.hh"
#include "G4FastSimulationManagerProcess.hh"
#include "G4EmConfigurator.hh"
#include "G4LossTableManager.hh"
#include "G4PAIPhotModel.hh"
#include "G4ProcessManager.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCuts.hh"
#include "G4EmParameters.hh"


class CustomEMPhysics : public G4EmStandardPhysics_option4 {
public:
    CustomEMPhysics(G4int ver = 1) : G4EmStandardPhysics_option4(ver) {}
    ~CustomEMPhysics() override = default;

    void ConstructProcess() override {
        G4EmStandardPhysics_option4::ConstructProcess();

        G4cout << "[LOG] CustomEMPhysics::ConstructProcess -> Adicionando a parameterização do Garfield..." << G4endl;
        GarfieldPhysics* garfieldPhysics = GarfieldPhysics::GetInstance();

        auto theParticleIterator = GetParticleIterator();
        theParticleIterator->reset();
        while ((*theParticleIterator)()) {
            G4ParticleDefinition* particle = theParticleIterator->value();
            G4ProcessManager* pmanager = particle->GetProcessManager();
            auto particleName = particle->GetParticleName();

            if (garfieldPhysics->FindParticleName(particleName, "garfield")) {
                auto fastSimProcess_garfield = new G4FastSimulationManagerProcess("G4FSMP_garfield");
                G4cout << "[LOG] PhysicsList -> Anexando o processo Garfield FastSim a: " << particleName << G4endl;
                pmanager->AddDiscreteProcess(fastSimProcess_garfield);
            }

            if (garfieldPhysics->FindParticleName(particleName, "geant4")) {
                 if (particleName == "gamma") continue;

                G4EmConfigurator* config = G4LossTableManager::Instance()->EmConfigurator();
                G4PAIPhotModel* paiPhot = new G4PAIPhotModel(particle, "G4PAIModel");
                 G4cout << "[LOG] PhysicsList -> Anexando o modelo PAIPhot a: " << particleName << G4endl;

                if (particleName == "e-" || particleName == "e+") {
                    config->SetExtraEmModel(particleName, "eIoni", paiPhot, "RegionGarfield", 0, 1e8*MeV, paiPhot);
                } else if (particleName == "mu-" || particleName == "mu+") {
                    config->SetExtraEmModel(particleName, "muIoni", paiPhot, "RegionGarfield", 0, 1e8*MeV, paiPhot);
                } else if (particleName == "proton" || particleName == "pi+" || particleName == "pi-") {
                    config->SetExtraEmModel(particleName, "hIoni", paiPhot, "RegionGarfield", 0, 1e8*MeV, paiPhot);
                } else if (particleName == "alpha" || particleName == "He3" || particleName == "GenericIon") {
                    config->SetExtraEmModel(particleName, "ionIoni", paiPhot, "RegionGarfield", 0, 1e8*MeV, paiPhot);
                }
            }
        }
    }
};

PhysicsList::PhysicsList() : G4VModularPhysicsList() {
    SetVerboseLevel(0);
    defaultCutValue = 1 * CLHEP::mm;

    G4EmParameters::Instance()->SetIntegral(false);

    RegisterPhysics(new G4DecayPhysics());
    RegisterPhysics(new G4RadioactiveDecayPhysics());
    RegisterPhysics(new G4HadronElasticPhysicsHP());
    RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP());
    RegisterPhysics(new G4IonPhysics());
    RegisterPhysics(new G4NeutronTrackingCut());
    RegisterPhysics(new CustomEMPhysics());

    GarfieldPhysics* garfieldPhysics = GarfieldPhysics::GetInstance();
    garfieldPhysics->SetIonizationModel("Heed");
}

PhysicsList::~PhysicsList() {}

void PhysicsList::SetCuts() {
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(100. * eV, 100. * TeV);
    SetCutsWithDefault();
    G4Region* region = G4RegionStore::GetInstance()->GetRegion("RegionGarfield");
    G4ProductionCuts* cuts = new G4ProductionCuts();
    cuts->SetProductionCut(1 * um, G4ProductionCuts::GetIndex("gamma"));
    cuts->SetProductionCut(1 * um, G4ProductionCuts::GetIndex("e-"));
    cuts->SetProductionCut(1 * um, G4ProductionCuts::GetIndex("e+"));
    if (region) {
        G4cout << "[LOG] PhysicsList::SetCuts -> Aplicando cortes de produção especiais à RegionGarfield." << G4endl;
        region->SetProductionCuts(cuts);
    } else {
        G4cout << "[LOG] PhysicsList::SetCuts -> ERRO: RegionGarfield não encontrada!" << G4endl;
    }
    DumpCutValuesTable();
}

void PhysicsList::ConstructParticle()
{
    G4VModularPhysicsList::ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
    G4VModularPhysicsList::ConstructProcess();
}