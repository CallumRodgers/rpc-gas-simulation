#ifndef MAINPARAMETERS_HH
#define MAINPARAMETERS_HH

#include "G4String.hh"
#include "MainMessenger.hpp"

namespace RPC {
    // Forward declaring.
    class MainMessenger;

    class MainParameters {
    public:
        MainParameters();
        ~MainParameters();

        // --------------------------------
        // Getters e Setters dos parâmetros
        // --------------------------------

        // SETUP
        void SetRPCType(const G4int &newValue) { rpcType = newValue; }
        G4int GetRPCType() { return rpcType; }

        void SetGasFile(const G4String &newValue) { gasFile = newValue; }
        G4String GetGasFile() { return gasFile; }

        void SetGasVoltage(const G4double &newValue) { gasHV = newValue; }
        G4double GetGasVoltage() { return gasHV; }

        // BEAM
        void SetBeamEnergy(const G4double &newValue) { beamEnergy = newValue; }
        G4double GetBeamEnergy() { return beamEnergy; }
    private:
        MainMessenger* messenger;

        // -------------------
        // Os parâmetros em si
        // -------------------

        // SETUP
        G4int rpcType;
        G4String gasFile;
        G4double gasHV;

        // BEAM
        G4double beamEnergy;
    };
}

#endif // MAINPARAMETERS_HH
