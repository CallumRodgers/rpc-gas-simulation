#ifndef EventAction_h
#define EventAction_h 

#include "G4UserEventAction.hh"
#include "globals.hh"

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    void AddAbs(G4double de, G4double dl) { fEnergyAbs += de; fTrackLAbs += dl; }
    void AddGas(G4double de) { fEnergyGas += de; }

  private:
    G4double fEnergyAbs;
    G4double fEnergyGas;
    G4double fTrackLAbs;
    G4int    fAvalancheSize;
    G4double fGain;
};

#endif