#ifndef PMSensitiveDetector_h
#define PMSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"
#include <vector>
#include <set>

class G4Step;
class G4TouchableHistory;

class PMSensitiveDetector : public G4VSensitiveDetector
{
public:
    PMSensitiveDetector(G4String name);
    virtual ~PMSensitiveDetector();

    virtual void Initialize(G4HCofThisEvent* hitCollection) override;
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    virtual void EndOfEvent(G4HCofThisEvent* hitCollection) override;

private:
    G4double fTotalEnergyDeposited;
    std::vector<G4int> fHitsPerPad;
    std::vector<G4double> fEdepPerPad;
    std::set<G4int> fPadsHitThisEvent;
};

#endif
