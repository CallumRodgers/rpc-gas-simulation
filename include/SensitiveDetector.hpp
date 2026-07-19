#ifndef SENSITIVEDETECTOR_HPP
#define SENSITIVEDETECTOR_HPP

#include "G4VSensitiveDetector.hh"
#include "globals.hh"
#include <vector>
#include <set>

namespace RPC {

    class SensitiveDetector final : public G4VSensitiveDetector {
    public:
        explicit SensitiveDetector(G4String name);
        ~SensitiveDetector() override;

        void Initialize(G4HCofThisEvent* hitCollection) override;
        void EndOfEvent(G4HCofThisEvent* hitCollection) override;

    protected:
        G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

    private:
        G4double fTotalEnergyDeposited;
        std::vector<G4int> fHitsPerPad;
        std::vector<G4double> fEdepPerPad;
        std::set<G4int> fPadsHitThisEvent;


    };
}

#endif
