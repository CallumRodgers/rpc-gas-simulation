#ifndef MAINMESSENGER_HH
#define MAINMESSENGER_HH

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "MainParameters.hpp"

namespace RPC {
    // Forward declaring.
    class MainParameters;

    class MainMessenger : G4UImessenger {
    public:
        explicit MainMessenger(MainParameters *);
        ~MainMessenger() override;

        void SetNewValue(G4UIcommand *command, G4String newValue) override;

    private:
        MainParameters* params;

        // Diretórios dos comandos
        G4UIdirectory* dirMain;
        G4UIdirectory* dirSetup;
        G4UIdirectory* dirBeam;

        // Comandos
        G4UIcmdWithAnInteger* cmdRPCType;
        G4UIcmdWithAString* cmdGasFile;
        G4UIcmdWithADoubleAndUnit* cmdGasHV;
        G4UIcmdWithADoubleAndUnit* cmdBeamEnergy;
    };
}

#endif // MAINMESSENGER_HH
