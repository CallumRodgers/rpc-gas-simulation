#ifndef PMMAINMESSENGER_HH
#define PMMAINMESSENGER_HH

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "PMMainParameters.hh"

class PMMainParameters;

class PMMainMessenger : G4UImessenger {
    public:
        explicit PMMainMessenger(PMMainParameters *);
        ~PMMainMessenger() override;

        void SetNewValue(G4UIcommand *command, G4String newValue) override;

    private:
        PMMainParameters* params;

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

#endif // PMMAINMESSENGER_HH
