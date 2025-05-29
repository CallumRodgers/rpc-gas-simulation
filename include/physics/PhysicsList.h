
#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H

#include <G4VModularPhysicsList.hh>

class PhysicsList : public G4VModularPhysicsList {
    public:
        PhysicsList();
        ~PhysicsList() override;
    protected:
        static void ConstructLeptons();
        static void ConstructBosons();
        static void ConstructBaryons();
        static void ConstructMesons();
        void ConstructEMAndDecay();
};

#endif //PHYSICSLIST_H
