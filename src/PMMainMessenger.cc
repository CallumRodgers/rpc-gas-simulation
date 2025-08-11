#include "PMMainMessenger.hh"

// Construtor
PMMainMessenger::PMMainMessenger(PMMainParameters *params) {
    this->params = params;

    // Criando diretórios
    dirMain = new G4UIdirectory("/rpc/");
    dirSetup = new G4UIdirectory("/rpc/setup/");
    dirBeam = new G4UIdirectory("/rpc/beam/");

    dirMain->SetGuidance("Comandos customizados para a simulação de RPCs");
    dirSetup->SetGuidance("Comandos que controlam o setup: geometria, mistura de gás, voltagem, etc.");
    dirBeam->SetGuidance("Comandos que controlam o feixe de partículas incidentes");

    // Criando comandos.
    cmdRPCType = new G4UIcmdWithAnInteger("/rpc/setup/rpcType", this);
    cmdGasFile = new G4UIcmdWithAString("/rpc/setup/gasFile", this);
    cmdGasHV = new G4UIcmdWithADoubleAndUnit("/rpc/setup/gasHV", this);
    cmdBeamEnergy = new G4UIcmdWithADoubleAndUnit("/rpc/beam/energy", this);

    cmdRPCType->SetGuidance("Seleciona o tipo de RPC:");
    cmdRPCType->SetGuidance("    - 0: RPC Marta do CBPF");
    cmdRPCType->SetGuidance("    - 1: Protótipo de iRPC do CERN");

    cmdGasFile->SetGuidance("Seleciona o caminho para o arquivo de gás.");
    cmdGasHV->SetGuidance("Seleciona a voltagem da RPC.");
    cmdBeamEnergy->SetGuidance("Seleciona a energia da fonte de partículas.");

    cmdRPCType->SetParameterName("type number", false);
    cmdGasFile->SetParameterName("path", false);
    cmdGasHV->SetParameterName("voltage", false);
    cmdBeamEnergy->SetParameterName("energy", false);
}

// Destruidor
PMMainMessenger::~PMMainMessenger() {
    // Deletando diretórios
    delete dirMain;
    delete dirSetup;
    delete dirBeam;

    // Deletando comandos
    delete cmdRPCType;
    delete cmdGasFile;
    delete cmdGasHV;
    delete cmdBeamEnergy;
}

void PMMainMessenger::SetNewValue(G4UIcommand *command, G4String newValue) {
    if (command == cmdRPCType) {
        G4cout << "Setting RPC Type to " << newValue << G4endl;
        params->SetRPCType(G4UIcmdWithAnInteger::ConvertToInt(newValue));
    } else if (command == cmdGasFile) {
        G4cout << "Setting Gas File to " << newValue << G4endl;
        params->SetGasFile(newValue);
    } else if (command == cmdGasHV) {
        G4cout << "Setting Voltage to " << newValue << G4endl;
        params->SetGasVoltage(G4UIcmdWithADoubleAndUnit::ConvertToDimensionedDouble(newValue));
    } else if (command == cmdBeamEnergy) {
        G4cout << "Setting Beam Energy to " << newValue << G4endl;
        params->SetBeamEnergy(G4UIcmdWithADoubleAndUnit::ConvertToDimensionedDouble(newValue));
    }
}



