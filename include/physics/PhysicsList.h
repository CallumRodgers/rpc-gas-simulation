
#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H

#include "G4VUserPhysicsList.hh"

class PhysicsList : public G4VUserPhysicsList {
    public:
        PhysicsList() = default;
        ~PhysicsList() override = default;
        void ConstructParticle() override;
        void ConstructProcess() override;
};

#endif //PHYSICSLIST_H
