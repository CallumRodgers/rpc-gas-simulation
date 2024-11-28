
#ifndef PRIMARYGENERATORACTION_H
#define PRIMARYGENERATORACTION_H

#include "G4VUserPrimaryGeneratorAction.hh"

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
    public:
        PrimaryGeneratorAction() = default;
        ~PrimaryGeneratorAction() override = default;
        void GeneratePrimaries(G4Event*);
};



#endif //PRIMARYGENERATORACTION_H
