#include "Analysis.hpp"

#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

using namespace RPC;

void Analysis::CreateHistogramsMarta() {
    auto analysisManager = G4AnalysisManager::Instance();
    // Histogramas
    analysisManager->CreateH1("Edep", "Total energy deposit per event", 100, 0., 1.1 * MeV);
    analysisManager->CreateH1("HitsPerPad", "Number of hits per pad", 64, 0., 64.);
    analysisManager->CreateH1("EdepPerPad", "Energy deposit per pad", 64, 0., 64.);
}

