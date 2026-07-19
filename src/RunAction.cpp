#include "RunAction.hpp"

#include <Garfield/AvalancheMicroscopic.hh>

#include "G4AnalysisManager.hh"
#include "Analysis.hpp"
#include "DetectorConstructionIRPC.hpp"
#include "G4SystemOfUnits.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"
#include "Analysis.hpp"
#include "HeedModel.hpp"

using namespace RPC;

RunAction::RunAction()
{
    Analysis::CreateHistogramsMarta();
}

RunAction::~RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->OpenFile("output.root");

    for (int i = 0; i < Geometry::nStrips; i++) {
        G4int signalId = analysisManager->CreateH1("signal_" + std::to_string(i), "Induced Current", Analysis::SIGNAL_WINDOW_BINS, 0., Analysis::SIGNAL_WINDOW_NS);
        analysisManager->SetH1YAxisTitle(signalId, "Current (fC/ns)");
        analysisManager->SetH1XAxisTitle(signalId, "Time (ns)");
        G4int chargeId = analysisManager->CreateH1("charge_" + std::to_string(i), "Induced Charge", Analysis::SIGNAL_WINDOW_BINS, 0., Analysis::SIGNAL_WINDOW_NS);
        analysisManager->SetH1YAxisTitle(chargeId, "Charge (fC)");
        analysisManager->SetH1XAxisTitle(chargeId, "Time (ns)");
    }

    // Creating heat map.
    analysisManager->CreateH2("HeatMap", "Event Signal Map",
        Geometry::nStrips, -Geometry::GARFIELD_VOLUME_Z_IRPC / 2.0, Geometry::GARFIELD_VOLUME_Z_IRPC / 2.0,
        1, -Geometry::GARFIELD_VOLUME_X_IRPC / 2.0, Geometry::GARFIELD_VOLUME_X_IRPC / 2.0
    );

    G4cout << "Starting run " << run->GetRunID() << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
    G4cout << "Finished run " << run->GetRunID() << G4endl;
}
