//
// Created by callum on 23/04/25.
//

#ifndef SENSITIVEDETECTOR_H
#define SENSITIVEDETECTOR_H

#include "G4VSensitiveDetector.hh"

#include "G4RunManager.hh"

class SensitiveDetector : public G4VSensitiveDetector {
    public:
        explicit SensitiveDetector(G4String);
        ~SensitiveDetector() override;

    private:
        G4bool ProcessHits(G4Step *, G4TouchableHistory *) override;

        void Initialize(G4HCofThisEvent*) override;
        void EndOfEvent(G4HCofThisEvent *) override;

        G4double fTotalEnergyDeposited;
};

#endif //SENSITIVEDETECTOR_H
