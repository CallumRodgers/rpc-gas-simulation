
#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public: 
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();
    virtual void GeneratePrimaries(G4Event* event);
    void SetRandomFlag(G4bool value);

private:
    G4ParticleGun *fParticleGun;
};

#endif