#include "AbortionHandler.hpp"

#include "G4AnalysisManager.hh"

using namespace RPC;

AbortionHandler::AbortionHandler() {}

AbortionHandler::~AbortionHandler() {}

G4bool AbortionHandler::Notify(G4ApplicationState requestedState) {
    if (requestedState == G4State_Abort) {
        // O Kernel irá abortar por conta de alguma exceção. Devemos
        // salvar os histogramas atuais.
        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->Write();
        analysisManager->CloseFile();
        G4cout << "Successfully saved analysis during kernel abortion." << G4endl;
    }
    return true;
}